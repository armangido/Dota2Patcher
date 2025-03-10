#pragma once
#include <iostream>
#include <fstream>
#include <shlobj.h>
#include <nlohmann/json.hpp>

class ConfigManager {
public:
    static inline std::unordered_map<string, int> config_entries = {
        { "camera_distance", 1200 },
        { "fog_enabled", 0 },
        { "fow_client_nofiltering", 0 },
        { "set_rendering_enabled", 0 },
        { "allow_rc_update", 0 },
        { "visible_by_enemy", 0 },
        { "illusions_detection", 0 },
        { "charge_of_darkness", 0},
        { "cl_weather", 0 },
        { "river_vial", 0},
    };

    static void ask_for_settings() {
        config_entries["camera_distance"] = ask_for_int("[~] Enter camera distance [default is 1200]: ", 1200, 1500);
        config_entries["fog_enabled"] = ask_for_bool("[~] Disable fog? [y/n or 1/0]: ");
        config_entries["fow_client_nofiltering"] = ask_for_bool("[~] Remove FoG anti-aliasing? (fow_client_nofiltering) [y/n or 1/0]: ");
        config_entries["set_rendering_enabled"] = ask_for_bool("[~] Show hidden particles? [y/n or 1/0]: ");
        config_entries["allow_rc_update"] = ask_for_bool("[~] Check for BETA update? [y/n or 1/0]: ");
        cout << "[HACKS]\n";
        config_entries["visible_by_enemy"] = ask_for_bool("[~] Visible By Enemy [y/n or 1/0]: ");
        config_entries["illusions_detection"] = ask_for_bool("[~] Illusions Detection [y/n or 1/0]: ");
        config_entries["charge_of_darkness"] = ask_for_bool("[~] Charge of darkness [y/n or 1/0]: ");
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
        config_entries["cl_weather"] = ask_for_int("[~] Enter Weather number: ", 1, 10) - 1;


        // River Vial
        cout <<
            "[RIVER TYPE]\n"
            "Default: 1\n"
            "Oil: 2\n"
            "Dry: 3\n"
            "Slime: 4\n"
            "Chrome: 5\n"
            "Electric: 6\n"
            "Potion: 7\n"
            "Blood: 8\n";

        //std::cin.ignore(10, '\n');
        config_entries["river_vial"] = ask_for_int("[~] Enter River type: ", 1, 8) - 1;

        save_settings();
    }

    static void show_settings() {
        cout
            << "[~] Current settings:\n"
            << "[~] Camera distance: " << config_entries["camera_distance"] << "\n"
            << "[~] Fog disabled: " << std::boolalpha << (bool)config_entries["fog_enabled"] << "\n"
            << "[~] Remove FoG anti-aliasing: " << std::boolalpha << (bool)config_entries["fow_client_nofiltering"] << "\n"
            << "[~] Show hidden particles: " << std::boolalpha << (bool)config_entries["set_rendering_enabled"] << "\n"
            << "[~] Check for BETA update: " << std::boolalpha << (bool)config_entries["allow_rc_update"] << "\n"
            << "[HACKS]\n"
            << "[~] Visible By Enemy: " << std::boolalpha << (bool)config_entries["visible_by_enemy"] << "\n"
            << "[~] Illusions Detection: " << std::boolalpha << (bool)config_entries["illusions_detection"] << "\n"
            << "[~] Charge of darkness: " << std::boolalpha << (bool)config_entries["charge_of_darkness"] << "\n"
            << "[~] Weather: " << int_to_weather((DOTA_WEATHER)config_entries["cl_weather"]) << "\n"
            << "[~] River type: " << int_to_river((DOTA_RIVER)config_entries["river_vial"]) << "\n"
            ;
    }

    static void load_settings() {
        if (auto path = app_data_path())
            config_patch = *path + "\\Dota2Patcher.json";
        else {
            LOG::CRITICAL("Failed to get AppData path");
            return;
        }

        std::ifstream file(config_patch);
        if (!file.is_open()) {
            LOG::ERR("Config file not found! Creating a new one.");
            ask_for_settings();
            return;
        }

        try {
            file >> config_file;

            for (auto& [key, value] : config_entries) {
                if (config_file.contains(key))
                    config_entries[key] = config_file[key].get<int>();
                else {
                    LOG::ERR("Config missing key '{}'. Requesting user input.", key);
                    ask_for_settings();
                }
            }
        }
        catch (const std::exception& e) {
            LOG::ERR("Error parsing config file: {}", e.what());
            ask_for_settings();
        }
    }

    static void save_settings() {
        config_file = nlohmann::json::object();

        for (const auto& [key, value] : config_entries) {
            config_file[key] = value;
        }

        std::ofstream out_file(config_patch);
        if (!out_file) {
            LOG::ERR("Failed to open config file for writing: {}", config_patch);
            return;
        }

        out_file << config_file.dump(4);
        out_file.close();
    }

private:
    static inline nlohmann::json config_file;
    static inline string config_patch;

    static string int_to_weather(DOTA_WEATHER weather) {
        switch (weather) {
        case DOTA_WEATHER::WEATHER_DEFAULT: return "Default";
        case DOTA_WEATHER::WEATHER_SNOW: return "Snow";
        case DOTA_WEATHER::WEATHER_RAIN: return "Rain";
        case DOTA_WEATHER::WEATHER_MOONBEAM: return "Moonbeam";
        case DOTA_WEATHER::WEATHER_PESTILENCE: return "Pestilence";
        case DOTA_WEATHER::WEATHER_HARVEST: return "Harvest";
        case DOTA_WEATHER::WEATHER_SIROCCO: return "Sirocco";
        case DOTA_WEATHER::WEATHER_SPRING: return "Spring";
        case DOTA_WEATHER::WEATHER_ASH: return "Ash";
        case DOTA_WEATHER::WEATHER_AURORA: return "Aurora";
        default: return "Default";
        }
    }

    static string int_to_river(DOTA_RIVER river) {
        switch (river) {
        case DOTA_RIVER::RIVER_DEFAULT: return "Default";
        case DOTA_RIVER::RIVER_OIL: return "Oil";
        case DOTA_RIVER::RIVER_DRY: return "Dry";
        case DOTA_RIVER::RIVER_SLIME: return "Slime";
        case DOTA_RIVER::RIVER_CHROME: return "Chrome";
        case DOTA_RIVER::RIVER_ELECTRIC: return "Electric";
        case DOTA_RIVER::RIVER_POTION: return "Potion";
        case DOTA_RIVER::RIVER_BLOOD: return "Blood";
        default: return "Default";
        }
    }

    static optional <string> app_data_path() {
        PWSTR path = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &path))) {
            char charPath[MAX_PATH];
            wcstombs(charPath, path, MAX_PATH);
            CoTaskMemFree(path);
            return string(charPath);
        }

        return nullopt;
    }

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
                    LOG::ERR("Input must be between {} and {}", std::to_string(min_value.value()), std::to_string(max_value.value()));
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
