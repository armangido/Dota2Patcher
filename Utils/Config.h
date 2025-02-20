#include <limits>
#include <variant>
#include <algorithm>
#include <array>

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
            LOG::ERR("(Config) RegCreateKeyExW error [{}] for {}", GetLastError(), valueName);
            return;
        }

        int int_value = static_cast<int>(value);
        if (RegSetValueExW(hKey, std::wstring(valueName.begin(), valueName.end()).c_str(), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&int_value), sizeof(int_value)) != ERROR_SUCCESS)
            LOG::ERR("(Config) RegSetValueExW error [{}] for {}", GetLastError(), valueName);

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
    static inline bool sv_cheats = false;
    static inline bool fog_enabled = false;
    static inline bool fow_client_nofiltering = false;
    static inline bool set_rendering_enabled = false;
    static inline bool allow_rc_update = false;
    static inline DOTA_WEATHER cl_weather = DOTA_WEATHER::WEATHER_DEFAULT;
    // HACKS
    static inline bool visible_by_enemy = false;
    static inline bool illusions_detection = false;

    static inline std::vector<ConfigEntry> config_entries = {
    { "camera_distance", &camera_distance },
    { "sv_cheats", &sv_cheats },
    { "fog_enabled", &fog_enabled },
    { "fow_client_nofiltering", &fow_client_nofiltering },
    { "set_rendering_enabled", &set_rendering_enabled },
    { "allow_rc_update", &allow_rc_update },
    { "visible_by_enemy",&visible_by_enemy },
    { "illusions_detection", &illusions_detection },
    { "cl_weather", &cl_weather },
    };

    static void ask_for_settings() {
        camera_distance = ask_for_int("[~] Enter camera distance [default is 1200]: ", 1200, 1500);
        sv_cheats = ask_for_bool("[~] Unlock sv_cheats? [y/n or 1/0]: ");
        fog_enabled = ask_for_bool("[~] Disable fog? [y/n or 1/0]: ");
        fow_client_nofiltering = ask_for_bool("[~] Remove FoG anti-aliasing? (fow_client_nofiltering) [y/n or 1/0]: ");
        set_rendering_enabled = ask_for_bool("[~] Show hidden particles? [y/n or 1/0]: ");
        allow_rc_update = ask_for_bool("[~] Check for BETA update? [y/n or 1/0]: ");
        visible_by_enemy = ask_for_bool("[~] Visible By Enemy [y/n or 1/0]: ");
        illusions_detection = ask_for_bool("[~] Illusions Detection [y/n or 1/0]: ");
        // Weather
        cout <<
            "[WEATHER]\n"
            "Default: 1\n"
            "Snow: 2\n"
            "Rain: 3\n"
            "Moonbeam: 4\n"
            "Pestilence: 5\n"
            "Harvest: 6\n"
            "Sirocco: 7\n"
            "Spring: 8\n"
            "Ash: 9\n"
            "Aurora: 10\n";

        std::cin.ignore(10, '\n');
        cl_weather = (DOTA_WEATHER)(ask_for_int("[~] Enter Weather number: ", 1, 10) - 1);

        write_settings();
    }

    static void show_settings() {
        cout
            << "[~] Current settings:\n"
            << "[~] Camera distance: " << camera_distance << "\n"
            << "[~] sv_heats unlock: " << std::boolalpha << sv_cheats << "\n"
            << "[~] Fog disabled: " << std::boolalpha << fog_enabled << "\n"
            << "[~] Remove FoG anti-aliasing: " << std::boolalpha << fow_client_nofiltering << "\n"
            << "[~] Show hidden particles: " << std::boolalpha << set_rendering_enabled << "\n"
            << "[~] Check for BETA update: " << std::boolalpha << allow_rc_update << "\n"
            << "[~] Visible By Enemy: " << std::boolalpha << visible_by_enemy << "\n"
            << "[~] Illusions Detection: " << std::boolalpha << illusions_detection << "\n"
            << "[~] Weather: " << int_to_weather(cl_weather) << "\n"
            ;
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

    static string int_to_weather(DOTA_WEATHER weather) {
        switch (weather) {
        case DOTA_WEATHER::WEATHER_DEFAULT: {
            return "Default";
        }
        case DOTA_WEATHER::WEATHER_SNOW: {
            return "Snow";
        }
        case DOTA_WEATHER::WEATHER_RAIN: {
            return "Rain";
        }
        case DOTA_WEATHER::WEATHER_MOONBEAM: {
            return "Moonbeam";
        }
        case DOTA_WEATHER::WEATHER_PESTILENCE: {
            return "Pestilence";
        }
        case DOTA_WEATHER::WEATHER_HARVEST: {
            return "Harvest";
        }
        case DOTA_WEATHER::WEATHER_SIROCCO: {
            return "Sirocco";
        }
        case DOTA_WEATHER::WEATHER_SPRING: {
            return "Spring";
        }
        case DOTA_WEATHER::WEATHER_ASH: {
            return "Ash";
        }
        case DOTA_WEATHER::WEATHER_AURORA: {
            return "Aurora";
        }
        default: {
            return "Default";
        }
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
                    LOG::ERR("Input must be between {} and {}", std::to_string(min_value.value()),  std::to_string(max_value.value()));
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
