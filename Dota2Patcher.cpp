#include "Dota2Patcher.h"
#include "Utils/Config.h"
#include "Utils/Updater.h"
#include "SourceSDK/CreateInterface.h"
#include "Hacks/Hacks.h"
#include "Utils/Scanner.h"

int main() {
	bool open_settings = GetAsyncKeyState(VK_SHIFT) & 0x8000;
	draw_logo();
	if (Updater::update_required())
		return 0;

	// CONFIG
	printf("\n");

	ConfigManager::load_settings();
	ConfigManager::show_settings();

	if (open_settings)
		ConfigManager::ask_for_settings();

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

	while (!Scanner::find_all())
		std::this_thread::sleep_for(std::chrono::seconds(1));

	// LOAD NETVARS
	printf("\n");

	LOG::DEBUG("Loading NetVars...");
	LOG::INFO("NetVars loaded: {}", vmt.schema_system->dump_netvars("client.dll", false));

	// LOAD CONVARS
	printf("\n");

	LOG::DEBUG("Loading ConVars...");
	LOG::INFO("ConVars loaded: {}", vmt.cvar->load_convars());

	// PATCHES
	printf("\n");
	Hacks::apply_patches();

	// MAIN GAME LOOP
	printf("\n");
	Hacks::start_worker();

	ProcessHandle::close_process_handle();
	return 0;
}
