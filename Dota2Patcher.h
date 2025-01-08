#pragma once
#include <iostream>
#include <Windows.h>
#include <vector>

class Patches {
public:
	struct PatchInfo {
		std::string name;
		std::string module;
		std::string pattern;
		std::string patch_bytes;
		int offset;
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
