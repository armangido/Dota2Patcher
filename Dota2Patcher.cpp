#include "Dota2Patcher.h"
#include "ProcessHandle.h"
#include "CDOTACamera.h"
#include "Memory.h"

int main() {
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

	printf("[~] Loading moules...\n");

	if (!memory.load_modules(process.get())) {
		printf("[-] Failed to load modules, aborting...\n");
		std::cin.get();
		return 0;
	}

	printf("[+] Modules loaded: %d\n", (int)memory.loaded_modules.size());

	Patches ptch;

	ptch.add_patch({
		"sv_cheats",
		"engine2.dll",
		"75 ? 48 8B 56 ? 48 8D 0D",
		"EB"
		});

	ptch.add_patch({
		"set_rendering_enabled",
		"particles.dll",
		"0F 84 ? ? ? ? 4D 89 73",
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

	CDOTACamera camera;
	if (!camera.find_camera(process.get(), memory.loaded_modules["client.dll"])) {
		printf("[-] Can't find CDOTACamera! Use ConVars instead...\n");
	}
	else {
		camera.set_distance(process.get(), 1500);
		camera.set_r_farz(process.get(), 18000);
	}

	printf("[+] Done!\n");
	std::cin.get();
	return 0;
}
