#include "Dota2Patcher.h"
#include <regex>
#include <Windows.h>

bool Registry::find_dota_path() {
    HKEY h_key{};
    if (int error = RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Classes\\dota2\\Shell\\Open\\Command", 0, KEY_QUERY_VALUE, &h_key) != ERROR_SUCCESS) {
        printf("[-] Registry Read Failed at RegOpenKey, Error: %d", error);
        RegCloseKey(h_key);
        return false;
    }

    char dota_path_reg[MAX_PATH]{};
    dota_path_reg[0] = '"';
    DWORD dota_path_size = sizeof(dota_path_reg) - sizeof(char);

    if (int error = RegQueryValueExA(h_key, nullptr, nullptr, nullptr, (LPBYTE)(dota_path_reg + 1), &dota_path_size) != ERROR_SUCCESS) {
        printf("[-] Registry Read Failed at RegQueryValue, Error: %d", error);
        RegCloseKey(h_key);
        return false;
    }

    RegCloseKey(h_key);

    this->dota_path_from_reg = dota_path_reg;

    return true;
}

bool Registry::regex_fix_path() {
    std::regex rgx{ R"(([^]:\\[^]+\\dota 2 beta))" };
    std::smatch matches;

    if (!std::regex_search(this->dota_path_from_reg, matches, rgx)) {
        printf("[-] Failed to parse Dota path!");
        return false;
    }

    this->dota_path_fixed = matches[1].str();

    return true;
}
