#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include <unordered_map>

class Memory {
public:
	struct ModuleInfo {
		uintptr_t start_address;
		uintptr_t end_address;
		size_t region_size;
	};

	bool load_modules(HANDLE hProc);
	uintptr_t pattern_scan(const HANDLE hProc, const ModuleInfo target_module, const std::string target_pattern);
	bool patch(const HANDLE hProc, const uintptr_t patch_addr, const std::string& replace_str, int patch_offset = 0);

	std::unordered_map<std::string, ModuleInfo> loaded_modules;
};
