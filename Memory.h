#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <optional>
#include <cstdint>
#include <type_traits>
#include "ProcessHandle.h"

class Memory {
public:
	struct ModuleInfo {
		uintptr_t start_address;
		uintptr_t end_address;
		size_t region_size;
		HMODULE hmodule;
	};

	static bool load_modules();
	static std::optional<uintptr_t> pattern_scan(const std::string target_module, const std::string target_pattern);
	static bool patch(const uintptr_t patch_addr, const std::string& replace_str);

	template<typename T, typename N>
	static std::optional<T> absolute_address(N instruction_ptr, ptrdiff_t offset, std::optional<uint32_t> size) {
		uintptr_t address = 0;

		if constexpr (std::is_pointer_v<N>) {
			address = reinterpret_cast<uintptr_t>(instruction_ptr);
		}
		else if constexpr (std::is_integral_v<N>) {
			address = static_cast<uintptr_t>(instruction_ptr);
		}

		int32_t relative_offset = 0;

		if (!ReadProcessMemory(ProcessHandle::get_handle(), reinterpret_cast<LPCVOID>(address + offset), &relative_offset, sizeof(relative_offset), nullptr)) {
			printf("[-] (absolute_address) ReadProcessMemory failed: 0x%d\n", GetLastError());
			return std::nullopt;
		}

		uintptr_t absolute_address = address + relative_offset + size.value_or(offset + sizeof(int32_t));

		return absolute_address;
	}

	template<typename T, typename N>
	static std::optional<T> read_memory(N address) {
		T value{};
		SIZE_T bytesRead;

		if (!ReadProcessMemory(ProcessHandle::get_handle(), reinterpret_cast<LPCVOID>(address), &value, sizeof(T), &bytesRead)) {
			printf("[-] Failed to read memory: 0x%X\n", GetLastError());
			return std::nullopt;
		}

		if (bytesRead != sizeof(T)) {
			printf("[-] Partial read at 0x%p\n", (void*)address);
			return std::nullopt;
		}

		return value;
	}

	template<typename T>
	static bool write_memory(uintptr_t address, const T& value) {
		SIZE_T bytesWritten;
		if (!WriteProcessMemory(ProcessHandle::get_handle(), reinterpret_cast<LPVOID>(address), &value, sizeof(T), &bytesWritten)) {
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
	static std::optional<std::string> read_string(const T address, size_t maxLength = 512) {
		std::vector<char> buffer(maxLength, 0);
		SIZE_T bytesRead;

		if (!ReadProcessMemory(ProcessHandle::get_handle(), reinterpret_cast<LPCVOID>(static_cast<uintptr_t>(address)), buffer.data(), buffer.size(), &bytesRead) && bytesRead == buffer.size())
			return std::nullopt;

		return std::string(buffer.data());
	}

	static std::unordered_map<std::string, ModuleInfo> loaded_modules;
};
