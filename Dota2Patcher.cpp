#include "Dota2Patcher.h"
#include "ProcessHandle.h"
#include "Memory.h"
#include "CDOTACamera.h"
#include "CDOTAGamerules.h"

int main(int argc, char* argv[]) {
#ifndef _DEBUG
	Updater updater;
	updater.check_update();
#endif

	printf("[~] Waiting for Dota2 process...\n");

	DWORD processID = 0;
	do {
		processID = ProcessHandle::GetPIDByName(L"dota2.exe");
		Sleep(1000);
	} while
		(processID == 0);

	printf("[+] Dota2 PID: %d\n", processID);

	ProcessHandle process(processID, PROCESS_ALL_ACCESS);
	if (!process.isValid()) {
		printf("[-] Failed to open process. Error: 0x%d\n", process.getLastError());
		std::cin.get();
		return 0;
	}

	printf("[+] Dota2 process handle: %p\n", process.get());

	Memory memory;

	printf("[~] Waiting for moules to load...\n");

	if (!memory.load_modules(process.get())) {
		printf("[-] Failed to load modules, aborting...\n");
		std::cin.get();
		return 0;
	}

	printf("[+] Modules loaded: %d\n", (int)memory.loaded_modules.size());

	// GAMERULES

	CDOTAGamerules game_rules;

	if (!game_rules.find_gamerules(process.get(), memory.loaded_modules["client.dll"])) {
		printf("[-] Can't find C_DOTAGamerules_Proxy!\n");
		std::cin.get();
		return 0;
	}

	printf("[~] Waiting for lobby to start...\n");
	while (!game_rules.in_lobby(process.get()))
		Sleep(1000);

	// CAMERA HACK

	CDOTACamera camera;

	if (argc >= 3) {
		for (int i = 0; i < argc; i++) {
			if (!strcmp(argv[i], "-camera_distance")) {
				camera.DEFAULT_DISTANCE = (float)std::stoi(argv[i + 1]);
				continue;
			}
			if (!strcmp(argv[i], "-fow_amount")) {
				camera.DEFAULT_FOW = (float)std::stoi(argv[i + 1]);
				continue;
			}
		}
	}

	if (!camera.find_camera(process.get(), memory.loaded_modules["client.dll"])) {
		printf("[-] Can't find CDOTACamera! Use ConVars instead...\n");
	}
	else {
		camera.set_distance(process.get(), camera.DEFAULT_DISTANCE);
		camera.set_r_farz(process.get(), camera.DEFAULT_DISTANCE * 2);
		camera.set_fow(process.get(), camera.DEFAULT_FOW);
	}

	// PATCHES

	Patches ptch;

	ptch.add_patch({
		"sv_cheats",
		"engine2.dll",
		Patches::Patterns::sv_cheats,
		"EB"
		});

	ptch.add_patch({
		"fog_enable",
		"engine2.dll",
		Patches::Patterns::fog_enable,
		"EB"
		});

	ptch.add_patch({
		"set_rendering_enabled",
		"particles.dll",
		Patches::Patterns::set_rendering_enabled,
		"85",
		1
		});

	for (const auto& patch : ptch.patches) {
		uintptr_t patch_addr = memory.pattern_scan(process.get(), memory.loaded_modules[patch.module], patch.pattern);
		if (patch_addr == -1) {
			printf("[!] Pattern for \"%s\" not found!\n", patch.name.c_str());
			continue;
		}

		printf("[+] \"%s\" patch addr: %p\n", patch.name.c_str(), (void*)patch_addr);

		if (!memory.patch(process.get(), patch_addr + patch.offset, patch.patch_bytes, patch.offset)) {
			printf("[-] Failed to patch \"%s\"\n", patch.name.c_str());
			continue;
		}

		printf("[+] \"%s\" patched successfully\n", patch.name.c_str());
	}

	printf("[+] Done!\n");
	return 0;
}
