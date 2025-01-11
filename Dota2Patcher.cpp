#include "Dota2Patcher.h"
#include "ProcessHandle.h"
#include "Memory.h"
#include "CDOTACamera.h"
#include "CDOTAGamerules.h"
#include "Config.h"

std::vector<Patches::PatchInfo> Patches::patches;

int main() {
	draw_logo();

#ifndef _DEBUG
	Updater::check_update();
#endif

	// CONFIG

	auto camera_distance = ConfigManager::Read("camera_distance"); // To open config if not set
	bool shift_pressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0; // And if SHIFT pressed

	if (camera_distance == -1 || shift_pressed) {
		printf("[~] Opening settings...\n");
		ConfigManager::ask_for_settings();
	}

	ConfigManager::show_settings();

	// GET DOTA2 PROCESS

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
		std::cin.get();
		return 0;
	}

	printf("[+] Dota2 process handle: %p\n", ProcessHandle::get_handle());

	printf("[~] Waiting for moules to load...\n");

	if (!Memory::load_modules()) {
		printf("[-] Failed to load modules, aborting...\n");
		ProcessHandle::close_process_handle();
		std::cin.get();
		return 0;
	}

	printf("[+] Modules loaded: %d\n", (int)Memory::loaded_modules.size());

	// FIND GAMERULES

	if (!CDOTAGamerules::find_gamerules(Memory::loaded_modules["client.dll"])) {
		printf("[-] Can't find C_DOTAGamerules_Proxy!\n");
		ProcessHandle::close_process_handle();
		std::cin.get();
		return 0;
	}

	printf("[~] Waiting for lobby to start...\n");
	while (!CDOTAGamerules::in_lobby())
		Sleep(1000);

	// CAMERA HACK

	if (!CDOTACamera::find_camera(Memory::loaded_modules["client.dll"])) {
		printf("[-] Can't find CDOTACamera! Use ConVars instead...\n");
	}
	else {
		float camera_distance = ConfigManager::get<float>("camera_distance");
		CDOTACamera::set_distance(camera_distance);
		CDOTACamera::set_r_farz(camera_distance * 2);
		CDOTACamera::set_fow(ConfigManager::get<float>("fow_amount"));
	}

	// PATCHES

	if (ConfigManager::get<bool>("fog_enable")) {
		Patches::add_patch({
			"fog_enable",
			"engine2.dll",
			Patches::Patterns::fog_enable,
			"EB"
			});
	}

	if (ConfigManager::get<bool>("sv_cheats")) {
		Patches::add_patch({
			"sv_cheats",
			"engine2.dll",
			Patches::Patterns::sv_cheats,
			"EB"
			});
	}

	if (ConfigManager::get<bool>("set_rendering_enabled")) {
		Patches::add_patch({
			"set_rendering_enabled",
			"particles.dll",
			Patches::Patterns::set_rendering_enabled,
			"85",
			1
			});
	}

	for (const auto& patch : Patches::patches) {
		uintptr_t patch_addr = Memory::pattern_scan(Memory::loaded_modules[patch.module], patch.pattern);
		if (patch_addr == -1) {
			printf("[!] Pattern for \"%s\" not found!\n", patch.name.c_str());
			continue;
		}

		printf("[+] \"%s\" patch addr: %p\n", patch.name.c_str(), (void*)patch_addr);

		if (!Memory::patch(patch_addr + patch.offset, patch.patch_bytes)) {
			printf("[-] Failed to patch \"%s\"!\n", patch.name.c_str());
			continue;
		}

		printf("[+] \"%s\" patched successfully\n", patch.name.c_str());
	}

	printf("[+] Done! Will close in 5 seconds...\n");
	ProcessHandle::close_process_handle();
	Sleep(5000);
	return 0;
}
