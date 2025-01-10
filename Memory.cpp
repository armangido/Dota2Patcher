#include "Memory.h"
#include <tlhelp32.h>
#include <psapi.h>
#include <iostream>
#include <sstream>

static std::vector<BYTE> parse_pattern(const std::string& pattern) {
    std::vector<BYTE> parsed_pattern;
    std::stringstream ss(pattern);
    std::string byte_str;
    while (ss >> byte_str) {
        if (byte_str == "?") {
            parsed_pattern.push_back(0xFF);
        }
        else {
            parsed_pattern.push_back(static_cast<BYTE>(std::stoi(byte_str, nullptr, 16)));
        }
    }
    return parsed_pattern;
}

static std::string wchar_to_string(const WCHAR* wcharStr) {
    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wcharStr, -1, nullptr, 0, nullptr, nullptr);
    if (bufferSize <= 0)
        return "";
    std::string utf8Str(bufferSize - 1, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wcharStr, -1, utf8Str.data(), bufferSize, nullptr, nullptr);
    return utf8Str;
}

#define InRange(x, a, b) (x >= a && x <= b)
#define getBit(x) (InRange((x & (~0x20)), 'A', 'F') ? ((x & (~0x20)) - 'A' + 0xA): (InRange(x, '0', '9') ? x - '0': 0))
#define getByte(x) (getBit(x[0]) << 4 | getBit(x[1]))

uintptr_t Memory::pattern_scan(const HANDLE hProc, const ModuleInfo target_module, const std::string target_pattern) {
    auto* buffer = new unsigned char[target_module.region_size];
    SIZE_T bytesRead;

    if (!ReadProcessMemory(hProc, reinterpret_cast<LPCVOID>(target_module.start_address), buffer, target_module.region_size, &bytesRead)) {
        printf("[-] (PatternScan) ReadProcessMemory failed: 0x%d\n", GetLastError());
        delete[] buffer;
        return NULL;
    }

    const char* pattern = target_pattern.c_str();
    uintptr_t first_match = 0;

    for (uintptr_t i = 0; i < bytesRead; ++i) {
        if (!*pattern) {
            delete[] buffer;
            return target_module.start_address + first_match;
        }

// Retarder visual studio
#pragma warning(push)
#pragma warning(disable : 6385)
        const unsigned char pattern_current = *reinterpret_cast<const unsigned char*>(pattern);
        const unsigned char memory_current = buffer[i];
#pragma warning(pop)

        if (pattern_current == '\?' || memory_current == getByte(pattern)) {
            if (!first_match) {
                first_match = i;
            }

            if (!pattern[2]) {
                delete[] buffer;
                return target_module.start_address + first_match;
            }

            pattern += pattern_current != '\?' ? 3 : 2;
        }
        else {
            pattern = target_pattern.c_str();
            first_match = 0;
        }
    }

    delete[] buffer;
    return -1;
}

bool Memory::load_modules(HANDLE hProc) {
    std::unordered_map<std::string, ModuleInfo> modules;
    MODULEENTRY32 module_entry;
    module_entry.dwSize = sizeof(MODULEENTRY32);
    HANDLE snapshot;

    while (true) {
        snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetProcessId(hProc));
        if (snapshot == INVALID_HANDLE_VALUE) {
            printf("[-] (load_modules) Failed to create snapshot: 0x%d\n", GetLastError());
            return false;
        }

        if (Module32First(snapshot, &module_entry)) {
            do {
                ModuleInfo info{ 0 };
                info.start_address = reinterpret_cast<uintptr_t>(module_entry.modBaseAddr);
                info.end_address = info.start_address + module_entry.modBaseSize;
                info.region_size = info.end_address - info.start_address;
                modules[wchar_to_string(module_entry.szModule)] = info;
            } while (Module32Next(snapshot, &module_entry));
        }

        if (modules.size() > 150)
            break;

        modules.clear();
        Sleep(2000);
    }


    loaded_modules = modules;
    CloseHandle(snapshot);
    return true;
}

bool Memory::patch(const HANDLE hProc, const uintptr_t patch_addr, const std::string& replace_str) {
    std::vector<BYTE> patchData = parse_pattern(replace_str);

    DWORD oldProtect;
    if (!VirtualProtectEx(hProc, reinterpret_cast<LPVOID>(patch_addr), patchData.size(), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        printf("[-] (Patch) Failed to change memory protection: 0x%d\n", GetLastError());
        return false;
    }

    SIZE_T bytesWritten;
    if (!WriteProcessMemory(hProc, reinterpret_cast<LPVOID>(patch_addr), patchData.data(), patchData.size(), &bytesWritten)) {
        printf("[-] (Patch) Failed to write to process memory: 0x%d\n", GetLastError());
        VirtualProtectEx(hProc, reinterpret_cast<LPVOID>(patch_addr), patchData.size(), oldProtect, &oldProtect);
        return false;
    }

    if (!VirtualProtectEx(hProc, reinterpret_cast<LPVOID>(patch_addr), patchData.size(), oldProtect, &oldProtect)) {
        printf("[-] (Patch) Failed to restore memory protection: 0x%d\n", GetLastError());
        return false;
    }

    return true;
}

uintptr_t Memory::absolute_address(HANDLE hProc, uintptr_t instruction_ptr, ptrdiff_t offset, std::optional<uint32_t> size) {
    int32_t relative_offset = 0;

    if (!ReadProcessMemory(hProc, reinterpret_cast<LPCVOID>(instruction_ptr + offset), &relative_offset, sizeof(relative_offset), nullptr)) {
        printf("[-] (absolute_address) ReadProcessMemory failed: 0x%d\n", GetLastError());
        return -1;
    }

    uintptr_t absolute_address = instruction_ptr + relative_offset + size.value_or(offset + sizeof(int32_t));

    return absolute_address;
}

uintptr_t Memory::get_pointer(HANDLE hProc, uintptr_t base_address, uintptr_t offset) {
    uintptr_t instance_address = 0;

    if (!ReadProcessMemory(hProc, reinterpret_cast<LPCVOID>(base_address + offset), &instance_address, sizeof(instance_address), nullptr)) {
        printf("[-] (get_pointer) ReadProcessMemory failed: 0x%d\n", GetLastError());
        return -1;
    }

    return instance_address;
}
