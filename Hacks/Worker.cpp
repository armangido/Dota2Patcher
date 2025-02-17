#include "Hacks.h"
#include "../SourceSDK/interfaces.h"
#include "../SourceSDK/CBaseEntity.h"
#include "..\Utils\Config.h"

void GameData::reset() {
	local_player = nullptr;
	local_hero = nullptr;
	vmt.gamerules = nullptr;
}

void Hacks::start_worker() {
    while (true) {
        while (!Scanner::find_CDOTAGamerules())
            std::this_thread::sleep_for(std::chrono::seconds(1));

        if (!vmt.gamerules->in_game()) {
            std::this_thread::yield();
            continue;
        }

        LOG::DEBUG("Game started, looking for a Local Player and Hero...");

        while (!GameData::local_player || !GameData::local_hero) {
            if (!GameData::local_player && Hacks::find_local_player())
                LOG::INFO("Local Player -> [%p]", (void*)GameData::local_player);

            if (!GameData::local_hero && Hacks::find_local_hero()) {
                LOG::INFO("Local Hero: [%s] -> [%p]", GameData::local_hero->identity()->entity_name().value().c_str(), (void*)GameData::local_hero);
                if (ConfigManager::roshan_timer_hack)
                    Hacks::hack_roshan_timer();
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        LOG::DEBUG("Entering in-game loop...");

        while (vmt.gamerules->in_game()) {
            for (auto ident = vmt.entity_system->first_identity(); ident; ident = ident->m_pNext().value_or(nullptr)) {
                if (auto current_ent = ident->base_entity(); current_ent && current_ent->is_hero()) {
                    if (ConfigManager::visible_by_enemy)
                        current_ent->set_custom_health_label(current_ent->visible() ? "0" : "");

                    if (ConfigManager::illusions_detection && current_ent->is_illusion())
                        current_ent->set_client_seen_illusion_modifier(true);
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        LOG::DEBUG("Exiting lobby...");
        GameData::reset();
    }
}
