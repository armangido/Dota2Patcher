#include "Dota2Patcher.h"
#include "Utils/ProcessHandle.h"
#include "Utils/Config.h"
#include "Utils/Updater.h"
#include "SourceSDK/CDOTAGamerules.h"
#include "SourceSDK/CDOTACamera.h"
#include "SourceSDK/CreateInterface.h"
#include "SourceSDK/interfaces.h"
#include "Hacks/Hacks.h"

int main() {
	bool open_settings = GetAsyncKeyState(VK_SHIFT) & 1;
	draw_logo();
	if (Updater::update_required())
		return 0;

	// CONFIG
	printf("\n");

	if (!ConfigManager::read_settings() || open_settings) {
		LOG::DEBUG("Opening settings...");
		ConfigManager::ask_for_settings();
		printf("\n");
	}

	ConfigManager::show_settings();

	// GET DOTA2 PROCESS
	printf("\n");

	LOG::DEBUG("Waiting for Dota2 process...");

	DWORD process_ID;
	while ((process_ID = ProcessHandle::get_PID_by_name(L"dota2.exe")) == 0)
		std::this_thread::sleep_for(std::chrono::seconds(1));

	LOG::INFO("Dota2 PID: {}", process_ID);

	ProcessHandle::open_process_handle(process_ID, PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION);
	if (!ProcessHandle::is_valid_handle()) {
		LOG::CRITICAL("Failed to open process. Error: {}", ProcessHandle::get_last_error());
		system("pause");
		return 0;
	}

	LOG::INFO("Dota2 process handle: {}", TO_VOID(ProcessHandle::get_handle()));

	// LOADING MODULES
	LOG::DEBUG("Waiting for a modules to load...");

	while (!Memory::load_modules(process_ID))
		std::this_thread::sleep_for(std::chrono::seconds(1));

	// INTERFACES
	printf("\n");

	std::vector<CreateInterface::ModuleInterfaces> interfaces_to_load = {
		{
			"engine2.dll", {
				{ "Source2EngineToClient001", [](uintptr_t base) { vmt.engine = (CEngineClient*)base; } },
			}, 192,
		},
		{
			"client.dll", {
				{ "Source2Client002", [](uintptr_t base) { vmt.client = (CSource2Client*)base; } }
			}, 190,
		},
		{
			"schemasystem.dll", {
				{ "SchemaSystem_001", [](uintptr_t base) { vmt.schema_system = (CSchemaSystem*)base; } }
			}, 46,
		},
		{
			"tier0.dll", {
				{ "VEngineCvar007", [](uintptr_t base) { vmt.cvar = (CCvar*)base; } }
			}, 47,
		}
	};

	for (const auto& interface_ : interfaces_to_load) {
		CreateInterface::load_interfaces(interface_);
	}

	// SCANNER
	printf("\n");

	while (!vmt.find_all())
		std::this_thread::sleep_for(std::chrono::seconds(1));

	// LOAD NETVARS
	printf("\n");

	LOG::DEBUG("Loading NetVars...");

	std::vector<string> class_filter { "C_DOTA_BaseNPC", "C_DOTA_BaseNPC_Hero", "C_DOTAPlayerController", "C_BaseEntity", "C_DOTATeam" };

	vmt.schema_system->dump_netvars("client.dll", false, class_filter);
	
	size_t netvar_count = 0;
	for (const auto& [class_name, netvar_map] : vmt.schema_system->g_netvars) {
		netvar_count += netvar_map.size();
	}

	LOG::INFO("NetVars loaded: {}", netvar_count);

	// LOAD CONVARS
	printf("\n");

	LOG::DEBUG("Loading ConVars...");
	LOG::INFO("ConVars loaded: {}", vmt.cvar->load_convars());

	// WAITING FOR A LOBBY
	printf("\n");
	LOG::DEBUG("Waiting for a lobby to start...");

	while (!Scanner::find_CDOTAGamerules())
		std::this_thread::sleep_for(std::chrono::seconds(1));

	// CAMERA HACK
	vmt.camera->set_distance(ConfigManager::camera_distance);
	vmt.camera->set_r_farz(ConfigManager::camera_distance * 2);
	vmt.camera->set_fow(ConfigManager::fow_amount);

	// PATCHES
	printf("\n");

	// #STR: "SV: Convar '%s' is cheat protected, change ignored"
	// jnz		short loc_1801AA477 <<<<
	// mov		rax, cs:LOG_CONSOLE
	// mov		edx, 3
	// mov		ecx, [rax]
	// call		cs:LoggingSystem_IsChannelEnabled
	// test		al, al
	// jz		short loc_1801AA469
	// cmp		[r15+438h], r13d
	// jle		short loc_1801AA44B
	// mov		rax, [r15+440h]
	// mov		rdi, [rax]
	// mov		rax, cs:LOG_CONSOLE
	// lea		r8, aSvConvarSIsChe ; "SV: Convar '%s' is cheat protected, cha"...
	if (ConfigManager::sv_cheats) {
		Patches::add_patch({
			"sv_cheats",
			"engine2.dll",
			Patches::Patterns::sv_cheats,
			Patches::PATCH_TYPE::JMP
			});
	}

	// CParticleCollection 95'th vfunc (offset 0x5F)
	// #STR: "Error in child list of particle system %s [%p], parent: %p, "m_Children.m_pHead: [%p]\n", "Address of m_Children.m_pHead: [%p]\n"
	// mov		r11, rsp
	// push		rbp
	// push		rsi
	// push		r12
	// push		r13
	// sub		rsp, 118h
	// movzx	eax, byte ptr [rcx+0A64h]
	// xor		r13d, r13d
	// shr		al, 7
	// movzx	r12d, dl
	// mov		[rsp+138h+arg_8], r13d
	// mov		rsi, rcx
	// mov		ebp, r13d
	// cmp		dl, al
	// jz		loc_18003A334 <<<<
	if (ConfigManager::set_rendering_enabled) {
		Patches::add_patch({
			"set_rendering_enabled",
			"particles.dll",
			Patches::Patterns::set_rendering_enabled,
			Patches::PATCH_TYPE::TEST,
			1
			});

		// idk for some reason set_rendering_enabled causes to crash without this fix
		// C_SceneEntity almost the last vfunc
		// #STR: "C_SceneEntity::SetupClientOnlyScene:  Couldn't determine d, "!self", "couldn't load scene file %s\n", "Failed to find soundevent '%s' when falling back from miss"
		// In the middle there will be #STR: "blank"
		// xor		edx, edx
		// lea		rcx, [rsp+180h+var_120]
		// call		cs:?Purge@CBufferString@@QEAAXH@Z ; CBufferString::Purge(int)
		// mov		rdi, [rsi+598h]
		// lea		rdx, [rbp+80h+arg_18]
		// mov		rcx, r15
		// call		sub_1802CFE30
		// mov		r8, rax
		// mov		[rbp+80h+arg_0], rbx
		// lea		rdx, [rbp+80h+arg_0]
		// mov		rcx, rdi
		// call		sub_1829D6BD0 ; #STR: "blank" <<<<
		// jmp		loc_1814170EA
		//
		// sub_1829D6BD0:
		// mov		r13, [rax]
		// mov[rsp + 88h + arg_0],	r13
		// test		r13, r13
		// jz		loc_1829D6ED8 <<<<
		// mov		rbx, [rbx]
		// test		rbx, rbx
		// jz		short loc_1829D6C42
		// mov		rbx, [rbx]
		// loc_1829D6C42: ; CODE XREF: sub_1829D6BD0+6D↑j
		Patches::add_patch({
			"set_rendering_enabled_fix",
			"client.dll",
			Patches::Patterns::set_rendering_enabled_fix,
			Patches::PATCH_TYPE::TEST,
			1
			});
	}

	// #STR: "particles/generic_gameplay/screen_death_indicator.vpcf"
	// push		rbx
	// sub		rsp, 40h
	// mov		rbx, rcx
	// call		sub_18159CC20   ; #STR: "dota_portrait_unit_stats_changed", "dota_force_portrait_update" <<<<
	//
	// sub_18159CC20:
	// push		rbx
	// push		rdi
	// push		r14
	// sub		rsp, 40h
	// mov		[rsp+58h+arg_10], rsi
	// mov		rdi, rcx
	// mov		esi, edx
	// call		sub_180271290
	// call		sub_181281B10
	// mov		rcx, cs:qword_1851BA2C0
	// mov		ebx, eax
	// mov		r8, [rcx]
	// call		qword ptr [r8+130h]
	// xor		r14d, r14d
	// test		al, al
	// jnz		short loc_18159CCAA
	// mov		rcx, cs:qword_1851BA2C0
	// mov		rdx, [rcx]
	// call		qword ptr [rdx+288h]
	// test		al, al
	// jnz		short loc_18159CCAA <<<<
	if (ConfigManager::visible_by_enemy) {
		Patches::add_patch({
			"visible_by_enemy",
			"client.dll",
			Patches::Patterns::visible_by_enemy,
			Patches::PATCH_TYPE::JMP
			});
	}

	for (const auto& patch : Patches::g_patches) {
		const auto patch_addr = Memory::pattern_scan(patch.module, patch.pattern);
		if (!patch_addr) {
			LOG::ERR("Pattern for \"{}\" not found!", patch.name);
			continue;
		}

		LOG::INFO("\"{}\" patch addr -> [{}]", patch.name, TO_VOID(patch_addr.value()));

		if (!Memory::patch(patch_addr.value() + patch.offset, patch.patch_type, patch.custom_patch_bytes)) {
			LOG::ERR("Failed to patch \"{}\"!", patch.name);
			continue;
		}

		LOG::INFO("\"{}\" patched successfully", patch.name);
	}

	// WAITING FOR A GAME
	if (ConfigManager::hacks_enabled()) {
		printf("\n");
		LOG::DEBUG("Waiting for a Game to start...");
		Hacks::start_worker();
	}

	printf("\n");
	LOG::INFO("Done! Will close in 5 seconds...");
	ProcessHandle::close_process_handle();
	std::this_thread::sleep_for(std::chrono::seconds(5));
	return 0;
}
