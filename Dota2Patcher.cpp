#include "Dota2Patcher.h"

int main() {
	Updater updater;
	updater.check_update();

	FileSystem fylesystem;

	if (!fylesystem.Init()) {
		printf("[!] An error has occured, exiting...\n");
		return 0;
	};

	Patcher patcher;

	patcher.add_patch({
		"sv_cheats",
		"engine2",
		FileSystem::PathType::game_bin,
		"75 ? 48 8B 56 ? 48 8D 0D",
		"EB"
		});

	patcher.add_patch({
		"set_rendering_enabled",
		"particles",
		FileSystem::PathType::game_bin,
		"0F 84 ? ? ? ? 4D 89 73",
		"85",
		1
		});

	for (const auto& patch : patcher.patches) {
		std::string file_path = fylesystem.library(patch.library, patch.patch_type);
		int patch_offset = patcher.find_offset(file_path, patch.pattern);
		if (patch_offset == -1) {
			printf("[!] Pattern \"%s\" not found\n", patch.patch_name.c_str());
			continue;
		}

		patcher.apply_patch(file_path, patch_offset + patch.offset, patch.replace);
		printf("[+] \"%s\" patched\n", patch.patch_name.c_str());
	}

	printf("[+] Done!\n");
	std::cin.get();
	return 0;
}
