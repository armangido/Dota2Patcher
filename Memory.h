#pragma once
#include <unordered_map>
#include <cstdint>
#include <type_traits>
#include "ProcessHandle.h"

class Memory {
public:
	enum class ASMType {
		LEA, // 3, 7
		CALL // 2, 6
	};

	struct ModuleInfo {
		uintptr_t start_address;
		uintptr_t end_address;
		size_t region_size;
		HMODULE hmodule;
	};

	static bool load_modules();
	static optional<uintptr_t> pattern_scan(const string target_module, const string target_pattern);
	static bool patch(const uintptr_t patch_addr, const string& replace_str);

	template<typename T, typename N>
	static optional<T> absolute_address(N instruction_ptr, ASMType instr_type = ASMType::LEA) {
		uintptr_t address = 0;

		if constexpr (std::is_pointer_v<N>)
			address = reinterpret_cast<uintptr_t>(instruction_ptr);
		else if constexpr (std::is_integral_v<N>)
			address = static_cast<uintptr_t>(instruction_ptr);

		ptrdiff_t offset = 0;
		uint32_t size = 0;
		switch (instr_type) {
		case ASMType::LEA:
			offset = 3;
			size = 7;
			break;
		case ASMType::CALL:
			offset = 2;
			size = 6;
			break;
		default:
			LOG::ERR("(absolute_address) Unsupported instruction type");
			return nullopt;
		}

		int32_t relative_offset = 0;
		if (!ReadProcessMemory(ProcessHandle::get_handle(), reinterpret_cast<LPCVOID>(address + offset), &relative_offset, sizeof(relative_offset), nullptr)) {
			LOG::ERR("(absolute_address) ReadProcessMemory failed: 0x%d", GetLastError());
			return nullopt;
		}

		uintptr_t absolute_address = address + relative_offset + size;

		return absolute_address;
	}

	template<typename T, typename N>
	static optional<T> read_memory(N address) {
		T value{};
		SIZE_T bytesRead;

		if (!ReadProcessMemory(ProcessHandle::get_handle(), reinterpret_cast<LPCVOID>(address), &value, sizeof(T), &bytesRead)) {
			LOG::ERR("Failed to read memory: 0x%X", GetLastError());
			return nullopt;
		}

		if (bytesRead != sizeof(T)) {
			LOG::ERR("Partial read at 0x%p", (void*)address);
			return nullopt;
		}

		return value;
	}

	template<typename T, typename N>
	static optional<T> virtual_function(N vmt, int function_index) {
		uintptr_t address = 0;

		if constexpr (std::is_pointer_v<N>)
			address = reinterpret_cast<uintptr_t>(vmt);
		else if constexpr (std::is_integral_v<N>)
			address = static_cast<uintptr_t>(vmt);

		const auto base_ptr = Memory::read_memory<uintptr_t>(address);
		if (!base_ptr)
			return nullopt;

		int actual_index = (function_index - 1) * 8;

		const auto instruction_ptr = Memory::read_memory<uintptr_t>(base_ptr.value() + actual_index);
		if (!instruction_ptr)
			return nullopt;

		return instruction_ptr;
	}

	template<typename T, typename N>
	static bool write_memory(N address, const T& value) {
		SIZE_T bytesWritten;
		if (!WriteProcessMemory(ProcessHandle::get_handle(), reinterpret_cast<LPVOID>(address), &value, sizeof(T), &bytesWritten)) {
			LOG::ERR("Failed to write memory at 0x%p: 0x%X", (void*)address, GetLastError());
			return false;
		}

		if (bytesWritten != sizeof(T)) {
			LOG::ERR("Partial write at 0x%p", (void*)address);
			return false;
		}

		return true;
	}

	template<typename T>
	static optional<string> read_string(T address, size_t maxLength = 512) {
		std::vector<char> buffer(maxLength, 0);
		SIZE_T bytesRead;

		LPCVOID address_casted = reinterpret_cast<LPCVOID>(address);

		if (!ReadProcessMemory(ProcessHandle::get_handle(), address_casted, buffer.data(), maxLength, &bytesRead) || bytesRead == 0)
			return nullopt;

		string result(buffer.data(), strnlen(buffer.data(), bytesRead));

		return result.empty() ? nullopt : optional<string>(std::move(result));
	}

	static std::unordered_map<string, ModuleInfo> loaded_modules;
};
