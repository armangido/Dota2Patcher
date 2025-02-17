#include <limits>
#include <variant>

class RegistryConfig {
public:
    static bool get(const string& valueName, int& out_value) {
        const auto readed = Read(valueName);
        if (!readed)
            return false;
        out_value = *readed;
        return true;
    }

    static void set(const string& valueName, const int value) {
        Write(valueName, value);
    }

    static optional<int> Read(const string& valueName) {
        HKEY hKey;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Dota2Patcher", 0, KEY_READ, &hKey) != ERROR_SUCCESS)
            return nullopt;

        int value = -1;
        DWORD valueSize = sizeof(value);
        if (RegGetValueW(hKey, NULL, std::wstring(valueName.begin(), valueName.end()).c_str(), RRF_RT_ANY, NULL, reinterpret_cast<BYTE*>(&value), &valueSize) != ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return nullopt;
        }

        RegCloseKey(hKey);
        return value;
    }

    template<typename T>
    static void Write(const string& valueName, const T& value) {
        HKEY hKey;
        if (RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\Dota2Patcher", 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS) {
            LOG::ERR("(Config) RegCreateKeyExW error [%lu] for %s", GetLastError(), valueName.c_str());
            return;
        }

        int int_value = static_cast<int>(value);
        if (RegSetValueExW(hKey, std::wstring(valueName.begin(), valueName.end()).c_str(), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&int_value), sizeof(int_value)) != ERROR_SUCCESS)
            LOG::ERR("(Config) RegSetValueExW error [%lu] for %s", GetLastError(), valueName.c_str());

        RegCloseKey(hKey);
    }
};

class ConfigManager : public RegistryConfig {
public:
    struct ConfigEntry {
        string name;
        void* variable;
    };

    static inline int camera_distance = 1200;
    static inline int fow_amount = 70;
    static inline bool sv_cheats = false;
    static inline bool fog_enabled = false;
    static inline bool set_rendering_enabled = false;
    static inline bool allow_rc_update = false;

    static inline bool visible_by_enemy = false;
    static inline bool illusions_detection = false;
    static inline bool roshan_timer_hack = false;

    static inline std::vector<ConfigEntry> config_entries = {
    { "camera_distance", &camera_distance },
    { "fow_amount", &fow_amount },
    { "sv_cheats", &sv_cheats },
    { "fog_enabled", &fog_enabled },
    { "set_rendering_enabled", &set_rendering_enabled },
    { "allow_rc_update", &allow_rc_update },
    { "visible_by_enemy",&visible_by_enemy },
    { "illusions_detection", &illusions_detection },
    { "roshan_timer_hack", &roshan_timer_hack }
    };

    static void ask_for_settings() {
        camera_distance = ask_for_int("[~] Enter camera distance [default is 1200]: ", 1200, 1500);
        fow_amount = ask_for_int("[~] Enter FOW amount [default is 70]: ");
        sv_cheats = ask_for_bool("[~] Unlock sv_cheats? [y/n or 1/0]: ");
        fog_enabled = ask_for_bool("[~] Disable fog? [y/n or 1/0]: ");
        set_rendering_enabled = ask_for_bool("[~] Show hidden particles? [y/n or 1/0]: ");
        allow_rc_update = ask_for_bool("[~] Check for BETA update? [y/n or 1/0]: ");
        printf("[HACKS] Dota2Patcher will not close!\n");
        visible_by_enemy = ask_for_bool("[~] Visible By Enemy [y/n or 1/0]: ");
        illusions_detection = ask_for_bool("[~] Illusions Detection [y/n or 1/0]: ");
        roshan_timer_hack = ask_for_bool("[~] Roshan Timer Hack [y/n or 1/0]: ");

        write_settings();
    }

    static void show_settings() {
        cout
            << "[~] Current settings:\n"
            << "[~] Camera distance: " << camera_distance << "\n"
            << "[~] FOW amount: " << fow_amount << "\n"
            << "[~] sv_heats unlock: " << std::boolalpha << sv_cheats << "\n"
            << "[~] Fog disabled: " << std::boolalpha << fog_enabled << "\n"
            << "[~] Show hidden particles: " << std::boolalpha << set_rendering_enabled << "\n"
            << "[~] Check for BETA update: " << std::boolalpha << allow_rc_update << "\n"
            << "[HACKS] Dota2Patcher will not close!\n"
            << "[~] Visible By Enemy: " << std::boolalpha << visible_by_enemy << "\n"
            << "[~] Illusions Detection: " << std::boolalpha << illusions_detection << "\n"
            << "[~] Roshan Timer Hack: " << std::boolalpha << roshan_timer_hack << "\n";
    }

    static bool read_settings() {
        for (const auto& entry : config_entries) {
            if (!RegistryConfig::get(entry.name, *reinterpret_cast<int*>(entry.variable)))
                return false;
        }
        return true;
    }

    static void write_settings() {
        for (const auto& entry : config_entries) {
            RegistryConfig::set(entry.name, *reinterpret_cast<int*>(entry.variable));
        }
    }

private:
    static int ask_for_int(const string& prompt, const optional<int> min_value = nullopt, const optional<int> max_value = nullopt) {
        int value;
        while (true) {
            cout << prompt;
            string input;
            std::getline(std::cin, input);

            try {
                value = std::stoi(input);

                if (!min_value.has_value() || !max_value.has_value())
                    break;

                if (value < min_value.value() || value > max_value.value()) {
                    LOG::ERR("Input must be between %s and %s", std::to_string(min_value.value()).c_str(),  std::to_string(max_value.value()).c_str());
                    continue;
                }

                break;
            }
            catch (...) {
                LOG::ERR("Invalid input!\n");
            }
        }
        return value;
    }

    static bool ask_for_bool(const string& prompt) {
        bool value;
        while (true) {
            cout << prompt;
            string input;
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
                LOG::ERR("Invalid input!\n");
            }
        }
        return value;
    }
};
