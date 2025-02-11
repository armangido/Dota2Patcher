#include "Dota2Patcher.h"
#include "Utils/ProcessHandle.h"
#include "Utils/Memory.h"
#include "Utils/Config.h"
#include "Utils/Updater.h"
#include "SourceSDK/CDOTAGamerules.h"
#include "SourceSDK/CDOTACamera.h"
#include "SourceSDK/CreateInterface.h"
#include "SourceSDK/interfaces.h"

std::vector<Patches::PatchInfo> Patches::patches;

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

	DWORD process_ID = 0;
	do {
		process_ID = ProcessHandle::get_PID_by_name(L"dota2.exe");
		std::this_thread::sleep_for(std::chrono::seconds(1));
	} while
		(process_ID == 0);

	LOG::INFO("Dota2 PID: %d", process_ID);

	ProcessHandle::open_process_handle(process_ID, PROCESS_ALL_ACCESS);
	if (!ProcessHandle::is_valid_handle()) {
		LOG::CRITICAL("Failed to open process. Error: 0x%d", ProcessHandle::get_last_error());
		system("pause");
		return 0;
	}

	LOG::INFO("Dota2 process handle: %p", ProcessHandle::get_handle());

	// LOADING MODULES
	LOG::DEBUG("Waiting for moules to load...");

	while (!Memory::load_modules(process_ID))
		std::this_thread::sleep_for(std::chrono::seconds(1));

	// INTERFACES
	printf("\n");

	std::vector<CreateInterface::ModuleInterfaces> interfaces_to_load = {
		{
			"engine2.dll", {
				{ "Source2EngineToClient001", [](uintptr_t base) { vmt.engine = (CEngineClient*)base; } },
			}
		},
		{
			"client.dll", {
				{ "Source2Client002", [](uintptr_t base) { vmt.client = (CSource2Client*)base; } },
			}
		},
		{
			"schemasystem.dll", {
				{ "SchemaSystem_001", [](uintptr_t base) { vmt.schema_system = (CSchemaSystem*)base; } },
			}
		},
	};

	for (const auto& interface_ : interfaces_to_load) {
		CreateInterface::load_interfaces(interface_);
	}

	// SCANNER
	printf("\n");

	while (!vmt.find_all())
		std::this_thread::sleep_for(std::chrono::seconds(1));

	// WAITING FOR A LOBBY
	printf("\n");
	LOG::DEBUG("Waiting for a lobby to start...");

	while (!Scanner::find_CDOTAGamerules())
		std::this_thread::sleep_for(std::chrono::seconds(1));

	// CAMERA HACK

	int camera_distance = ConfigManager::camera_distance;
	vmt.camera->set_distance(camera_distance);
	vmt.camera->set_r_farz(camera_distance * 2);
	vmt.camera->set_fow(ConfigManager::fow_amount);

	// PATCHES
	printf("\n");
	
	// CRenderingWorldSession 15' vfunc with #STR: "backColorBuffer", "ClearLayer", "MainView"
	// lea		rdx, [rbp+490h+var_510]
	// mov		[rbp+490h+var_508], eax
	// mov		rcx, rbx
	// call		sub_180219A30 <<<< #STR: "(Texture)", "Player %d%s", "output_color_texture", "output_depth_texture", "output_color_back_buffer"...
	//
	// sub_180219A30:
	// cmp		[rbp+1200h+var_1014], 0
	// jz		short loc_18021A42B
	// mov		edx, 0FFFFFFFFh
	// lea		rcx, unk_180619270
	// call		sub_1803A4AB0
	// test		rax, rax
	// jnz		short loc_18021A422
	// mov		rax, cs:qword_180619278
	// mov		rax, [rax+8]
	// cmp		byte ptr [rax], 0
	// jz		short loc_18021A42B <<<<
	// mov		al, 1
	// jmp		short loc_18021A42D
	if (ConfigManager::fog_enabled) {
		Patches::add_patch({
			"fog_enable",
			"engine2.dll",
			Patches::Patterns::fog_enable,
			Patches::JumpType::JMP
			});
	}

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
			Patches::JumpType::JMP
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
			Patches::JumpType::TEST,
			1
			});

		// idk for some reason set_rendering_enabled causes to crash without this fix
		// C_SceneEntity almost the last vfunc
		// #STR: "C_SceneEntity::SetupClientOnlyScene:  Couldn't determine d, "!self", "couldn't load scene file %s\n", "Failed to find soundevent '%s' when falling back from miss
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
			Patches::JumpType::TEST,
			1
			});
	}

	for (const auto& patch : Patches::patches) {
		const auto patch_addr = Memory::pattern_scan(patch.module, patch.pattern);
		if (!patch_addr) {
			LOG::ERR("Pattern for \"%s\" not found!", patch.name.c_str());
			continue;
		}

		LOG::INFO("\"%s\" patch addr -> [%p]", patch.name.c_str(), (void*)patch_addr.value());

		if (!Memory::patch(patch_addr.value() + patch.offset, patch.jump_type)) {
			LOG::ERR("Failed to patch \"%s\"!", patch.name.c_str());
			continue;
		}

		LOG::INFO("\"%s\" patched successfully", patch.name.c_str());
	}

	printf("\n");
	LOG::INFO("Done! Will close in 5 seconds...");
	ProcessHandle::close_process_handle();
	std::this_thread::sleep_for(std::chrono::seconds(5));
	return 0;
}
