#include "Dota2Patcher.h"
#include "ProcessHandle.h"
#include "Memory.h"
#include "Config.h"
#include "SourceSDK/CDOTAGamerules.h"
#include "SourceSDK/CDOTACamera.h"
#include "SourceSDK/CreateInterface.h"
#include "SourceSDK/interfaces.h"

std::vector<Patches::PatchInfo> Patches::patches;

int main() {
	draw_logo();

#ifndef _DEBUG
	Updater::check_update();
#endif

	// CONFIG

	auto camera_distance_test = ConfigManager::Read("camera_distance"); // To open config if not set
	bool shift_pressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0; // And if SHIFT pressed

	if (camera_distance_test == -1 || shift_pressed) {
		printf("[~] Opening settings...\n");
		ConfigManager::ask_for_settings();
		printf("\n");
	}

	ConfigManager::read_settings();
	ConfigManager::show_settings();

	// GET DOTA2 PROCESS
	printf("\n");

	printf("[~] Waiting for Dota2 process...\n");

	DWORD process_ID = 0;
	do {
		process_ID = ProcessHandle::get_PID_by_name(L"dota2.exe");
		Sleep(1000);
	} while
		(process_ID == 0);

	printf("[+] Dota2 PID: %d\n", process_ID);

	ProcessHandle::open_process_handle(process_ID, PROCESS_ALL_ACCESS);
	if (!ProcessHandle::is_valid_handle()) {
		printf("[-] Failed to open process. Error: 0x%d\n", ProcessHandle::get_last_error());
		system("pause");
		return 0;
	}

	printf("[+] Dota2 process handle: %p\n", ProcessHandle::get_handle());

	printf("[~] Waiting for moules to load...\n");

	if (!Memory::load_modules()) {
		printf("[-] Failed to load modules, aborting...\n");
		ProcessHandle::close_process_handle();
		system("pause");
		return 0;
	}

	printf("[+] Modules loaded: %d\n", (int)Memory::loaded_modules.size());

	// INTERFACES
	printf("\n");

	std::vector<CreateInterface::ModuleInterfaces> interfaces_to_load = {
		{
			"engine2.dll", {
				{ "Source2EngineToClient001", [](uintptr_t base) { vmt.engine = (CEngineClient*)base; } },
				// { Some other interface goes here }
			}
		},
		{
			"client.dll", {
				{ "Source2Client002", [](uintptr_t base) { vmt.client = (CSource2Client*)base; } },
			}
		}
	};

	for (const auto& interface_ : interfaces_to_load) {
		CreateInterface::load_interfaces(interface_);
	}

	if (!vmt.find_all()) {
		printf("[-] Scanner failed! Exiting...\n");
		ProcessHandle::close_process_handle();
		system("pause");
		return 0;
	}

	// WAITING FOR LOBBY
	printf("\n");

	printf("[~] Waiting for lobby to start...\n");
	while (!vmt.gamerules->in_lobby())
		Sleep(1000);

	// CAMERA HACK

	int camera_distance = ConfigManager::camera_distance;
	vmt.camera->set_distance(camera_distance);
	vmt.camera->set_r_farz(camera_distance * 2);
	vmt.camera->set_fow(ConfigManager::fow_amount);

	// PATCHES
	printf("\n");

	if (ConfigManager::fog_enabled) {
		Patches::add_patch({
			"fog_enable",
			"engine2.dll",
			Patches::Patterns::fog_enable,
			"EB"
			});
	}

	if (ConfigManager::sv_cheats) {
		Patches::add_patch({
			"sv_cheats",
			"engine2.dll",
			Patches::Patterns::sv_cheats,
			"EB"
			});
	}

	if (ConfigManager::set_rendering_enabled) {
		Patches::add_patch({
			"set_rendering_enabled",
			"particles.dll",
			Patches::Patterns::set_rendering_enabled,
			"85",
			1
			});
	}

	for (const auto& patch : Patches::patches) {
		const auto patch_addr = Memory::pattern_scan(patch.module, patch.pattern);
		if (!patch_addr) {
			printf("[!] Pattern for \"%s\" not found!\n", patch.name.c_str());
			continue;
		}

		printf("[+] \"%s\" patch addr -> [%p]\n", patch.name.c_str(), (void*)patch_addr.value());

		if (!Memory::patch(patch_addr.value() + patch.offset, patch.patch_bytes)) {
			printf("[-] Failed to patch \"%s\"!\n", patch.name.c_str());
			continue;
		}

		printf("[+] \"%s\" patched successfully\n", patch.name.c_str());
	}

	printf("\n[+] Done! Will close in 5 seconds...\n");
	ProcessHandle::close_process_handle();
	Sleep(5000);
	return 0;
}
