#include <windows.h>
#include <iostream>
#include <string>
#include <limits>

class RegistryConfig {
public:
    template<typename T>
    static T get(const std::string& valueName) {
        return static_cast<T>(Read(valueName));
    }

    template<typename T>
    static void set(const std::string& valueName, const T& value) {
        Write(valueName, value);
    }

    static int Read(const std::string& valueName) {
        HKEY hKey;
        LONG result = RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Dota2Patcher", 0, KEY_READ, &hKey);

        if (result != ERROR_SUCCESS)
            return -1;

        int value = -1;
        DWORD valueSize = sizeof(value);
        result = RegGetValueW(hKey, NULL, std::wstring(valueName.begin(), valueName.end()).c_str(), RRF_RT_ANY, NULL, reinterpret_cast<BYTE*>(&value), &valueSize);

        RegCloseKey(hKey);

        if (result != ERROR_SUCCESS)
            return -1;

        return value;
    }

    template<typename T>
    static void Write(const std::string& valueName, const T& value) {
        HKEY hKey;
        LONG result = RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\Dota2Patcher", 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL);

        if (result != ERROR_SUCCESS) {
            printf("[!] (Config) RegCreateKeyExW error for %s\n", valueName.c_str());
            return;
        }

        int int_value = static_cast<int>(value);
        result = RegSetValueExW(hKey, std::wstring(valueName.begin(), valueName.end()).c_str(), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&int_value), sizeof(int_value));
        if (result != ERROR_SUCCESS)
            printf("[!] (Config) RegSetValueExW error for %s\n", valueName.c_str());

        RegCloseKey(hKey);
    }
};

class ConfigManager : public RegistryConfig {
public:
    static void ask_for_settings() {
        camera_distance = ask_for_int("[~] Enter camera distance: ");
        fow_amount = ask_for_int("[~] Enter FOW amount [default is 70]: ");
        sv_cheats = ask_for_bool("[~] Unlock sv_cheats? [y/n or 1/0]: ");
        fog_enabled = ask_for_bool("[~] Disable fog? [y/n or 1/0]: ");
        set_rendering_enabled = ask_for_bool("[~] Show hidden particles? [y/n or 1/0]: ");

        write_settings();
    }

    static void show_settings() {
        std::cout
            << "[~] Current settings:\n"
            << "[~] Camera distance: " << camera_distance << "\n"
            << "[~] FOW amount: " << fow_amount << "\n"
            << "[~] sv_heats unlock: " << (sv_cheats ? "Yes" : "No") << "\n"
            << "[~] Fog disabled: " << (fog_enabled ? "Yes" : "No") << "\n"
            << "[~] Show hidden particles: " << (set_rendering_enabled ? "Yes" : "No") << "\n";
    }

    static void write_settings() {
        RegistryConfig::set("camera_distance", camera_distance);
        RegistryConfig::set("fow_amount", fow_amount);
        RegistryConfig::set("sv_cheats", sv_cheats);
        RegistryConfig::set("fog_enabled", fog_enabled);
        RegistryConfig::set("set_rendering_enabled", set_rendering_enabled);
    }

    static void read_settings() {
        camera_distance = RegistryConfig::get<int>("camera_distance");
        fow_amount = RegistryConfig::get<int>("fow_amount");
        sv_cheats = RegistryConfig::get<bool>("sv_cheats");
        fog_enabled = RegistryConfig::get<bool>("fog_enabled");
        set_rendering_enabled = RegistryConfig::get<bool>("set_rendering_enabled");
    }

    static inline int camera_distance = 1500;
    static inline int fow_amount = 70;
    static inline bool sv_cheats = true;
    static inline bool fog_enabled = true;
    static inline bool set_rendering_enabled = true;

private:
    static int ask_for_int(const std::string& prompt) {
        int value;
        while (true) {
            std::cout << prompt;
            std::cin >> value;

            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                printf("[!] Invalid input!\n");
            }
            else {
                break;
            }
        }
        return value;
    }

    static bool ask_for_bool(const std::string& prompt) {
        bool value;
        while (true) {
            std::cout << prompt;
            std::string input;
            std::cin >> input;

            if (input == "y" || input == "1") {
                value = true;
                break;
            }
            else if (input == "n" || input == "0") {
                value = false;
                break;
            }
            else {
                printf("[!] Invalid input!\n");
            }
        }
        return value;
    }
};
