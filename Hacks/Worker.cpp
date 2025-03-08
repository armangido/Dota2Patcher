#include "Hacks.h"
#include "../SourceSDK/interfaces.h"
#include "../SourceSDK/CBaseEntity.h"
#include "../SourceSDK/CCvar.h"
#include "../Utils/Config.h"
#include "../Utils/Scanner.h"

void GameData::reset() {
    local_player = nullptr;
    local_hero = nullptr;
    local_hero_server = nullptr;
    modifiers_list = nullptr;
}

void Hacks::start_worker() {
    while (true) {
        LOG::DEBUG("Waiting for a Lobby...");

        while (!Scanner::find_CDOTAGamerules()) {
            if (!ProcessHandle::get_PID_by_name(L"dota2.exe"))
                return;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        LOG::DEBUG("Waiting for a Game to start...");

        while (!vmt.gamerules->in_game())
            std::this_thread::sleep_for(std::chrono::seconds(1));

        LOG::DEBUG("Game started, looking for a Local Player and Hero...");

        while (!GameData::local_player || !GameData::local_hero || !GameData::local_hero_server) {
            if (Hacks::find_local_player()) {
                LOG::INFO("Local Player -> [{}]", TO_VOID(GameData::local_player));
                // ConVars
                Hacks::find_and_set_convars();
                // Camera Hack
                vmt.camera->set_distance(ConfigManager::config_entries["camera_distance"]);
                vmt.camera->set_r_farz(ConfigManager::config_entries["camera_distance"] * 2);
            }

            if (Hacks::find_local_hero()) {
                LOG::INFO("Local Hero [client]: [{}] -> [{}]", GameData::local_hero->identity()->entity_name().value(), TO_VOID(GameData::local_hero));
                GameData::local_team = GameData::local_hero->team_num();
                // River Type
                vmt.gamerules->set_river_vial((DOTA_RIVER)ConfigManager::config_entries["river_vial"]);
            }

            if (Hacks::find_local_hero_server()) {
                LOG::INFO("Local Hero [server]: [{}] -> [{}]", GameData::local_hero_server->identity()->entity_name().value(), TO_VOID(GameData::local_hero_server));

                const auto modifier_manager = GameData::local_hero_server->modifier_manager();
                GameData::modifiers_list = modifier_manager->list();
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        LOG::DEBUG("Entering in-game loop...");

        while (vmt.gamerules->in_game()) {
            if (ConfigManager::config_entries["visible_by_enemy"])
                GameData::dota_range_display->set<float>(GameData::local_hero->visible() ? 100.f: 0.f);

            if (!ConfigManager::config_entries["illusions_detection"]) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                continue;
            }

            for (auto ident = vmt.c_entity_system->first_identity(); ident; ident = ident->m_pNext().value_or(nullptr)) {
                if (auto current_ent = ident->base_entity(); current_ent && current_ent->is_hero()) {
                    if (current_ent->team_num() != GameData::local_team) {
                        if (ConfigManager::config_entries["illusions_detection"] && current_ent->is_illusion())
                            current_ent->set_client_seen_illusion_modifier(true);
                    }
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }

        LOG::DEBUG("Exiting lobby...\n");
        GameData::reset();
    }
}
