#pragma once
#include <format>
#include <vector>
#include <Windows.h>

class Registry {
public:
	bool find_dota_path();
	bool regex_fix_path();

	std::string get_dota_path() const {
		return dota_path_fixed;
	}

private:
	std::string dota_path_from_reg;
	std::string dota_path_fixed;
};

class FileSystem : public Registry {
public:
	enum class PathType {
		game_bin,
		game_dota
	};

	bool Init() {
		return find_dota_path() && regex_fix_path();
	}

	std::string dota_path() const {
		return get_dota_path();
	}

	std::string library(const std::string& name, PathType type) {
		std::string sub_path = get_path_by_type(type);
		return get_dota_path() + std::format("{}\\{}.dll", sub_path, name);
	}

private:
	std::string get_path_by_type(PathType type) const {
		switch (type) {
		case PathType::game_bin:
			return "\\game\\bin\\win64";
		case PathType::game_dota:
			return "\\game\\dota\\bin\\win64";
		default:
			return "";
		}
	}
};

class Patcher {
public:
	struct PatchInfo {
		std::string patch_name;
		std::string library;
		FileSystem::PathType patch_type;
		std::string pattern;
		std::string replace;
		int offset = 0;
	};

	int find_offset(const std::string& file_path, const std::string& pattern);
	void apply_patch(const std::string& file_path, int patch_offset, const std::string& replace_str);

	void add_patch(const PatchInfo& patch) {
		patches.push_back(patch);
	}

	std::vector<PatchInfo> patches;
};
