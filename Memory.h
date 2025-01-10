#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <optional>

class Memory {
public:
	struct ModuleInfo {
		uintptr_t start_address;
		uintptr_t end_address;
		size_t region_size;
	};

	bool load_modules(HANDLE hProc);
	uintptr_t pattern_scan(const HANDLE hProc, const ModuleInfo target_module, const std::string target_pattern);
	bool patch(const HANDLE hProc, const uintptr_t patch_addr, const std::string& replace_str);
	uintptr_t absolute_address(HANDLE hProc, uintptr_t instruction_ptr, ptrdiff_t offset, std::optional<uint32_t> size);
	uintptr_t get_pointer(HANDLE hProc, uintptr_t base_address, uintptr_t offset);
	
	template<typename T>
	bool write_memory(HANDLE hProc, uintptr_t address, const T& value) {
		SIZE_T bytesWritten;
		if (!WriteProcessMemory(hProc, reinterpret_cast<LPVOID>(address), &value, sizeof(T), &bytesWritten)) {
			printf("[-] Failed to write memory at 0x%llX: 0x%X\n", address, GetLastError());
			return false;
		}

		if (bytesWritten != sizeof(T)) {
			printf("[-] Partial write at 0x%llX\n", address);
			return false;
		}

		return true;
	}

	template<typename T>
	std::optional<T> read_memory(HANDLE hProc, uintptr_t address) {
		T value{};
		SIZE_T bytesRead;
		if (!ReadProcessMemory(hProc, reinterpret_cast<LPCVOID>(address), &value, sizeof(T), &bytesRead)) {
			printf("[-] Failed to read memory at 0x%llX: 0x%X\n", address, GetLastError());
			return std::nullopt;
		}

		if (bytesRead != sizeof(T)) {
			printf("[-] Partial read at 0x%llX\n", address);
			return std::nullopt;
		}

		return value;
	}

	std::unordered_map<std::string, ModuleInfo> loaded_modules;
};
