#pragma once
#include <iostream>
#include <Windows.h>
#include <vector>

class Patches {
public:
	struct Patterns {
		static inline const std::string CDOTAGamerules = "48 8B ? ? ? ? ? 48 85 ? 74 ? 48 8B ? 45 33 ? 66 0F";
		static inline const std::string CDOTACamera = "48 8D ? ? ? ? ? 48 83 C4 ? 5B C3 48 8D ? ? ? ? ? E8 ? ? ? ? 83 3D 76 B2 98 03";
		static inline const std::string sv_cheats = "75 ? 48 8B 56 ? 48 8D 0D";
		static inline const std::string fog_enable = "74 ? B0 ? EB ? 32 C0 88 85";
		static inline const std::string set_rendering_enabled = "0F 84 ? ? ? ? 4D 89 73";
	};

	struct PatchInfo {
		std::string name;
		std::string module;
		std::string pattern;
		std::string patch_bytes;
		int offset = 0;
	};

	void add_patch(const PatchInfo& patch) {
		patches.push_back(patch);
	}

	std::vector<PatchInfo> patches;
};

class Updater {
public:
	void check_update();

private:
	std::string get_remote_hash();
	std::string get_local_hash();

	const std::string remote_version_url = "https://raw.githubusercontent.com/Wolf49406/Dota2Patcher/main/MD5";
	const std::string update_url = "https://github.com/Wolf49406/Dota2Patcher/releases/latest";
};
