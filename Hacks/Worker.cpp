#include "Hacks.h"
#include "../SourceSDK/interfaces.h"
#include "../SourceSDK/CBaseEntity.h"
#include "../SourceSDK/CCvar.h"
#include "../Utils/Config.h"

void GameData::reset() {
	local_player = nullptr;
	local_hero = nullptr;
    local_team = -1;
	vmt.gamerules = nullptr;
    dota_range_display = nullptr;
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
                LOG::INFO("Local Player -> [{}]", TO_VOID(GameData::local_player));

            if (!GameData::local_hero && Hacks::find_local_hero()) {
                LOG::INFO("Local Hero: [{}] -> [{}]", GameData::local_hero->identity()->entity_name().value(), TO_VOID(GameData::local_hero));
                local_team = GameData::local_hero->team_num();
                dota_range_display = vmt.cvar->cvar_by_name("dota_range_display").value();
                vmt.cvar->cvar_by_name("fow_client_nofiltering").value()->set_int(!ConfigManager::fow_client_nofiltering);
                vmt.cvar->cvar_by_name("fog_enable").value()->set_int(!ConfigManager::fog_enabled);
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        LOG::DEBUG("Entering in-game loop...");

        while (vmt.gamerules->in_game()) {
            dota_range_display->set_int(GameData::local_hero->visible() ? 100 : 0);

            for (auto ident = vmt.entity_system->first_identity(); ident; ident = ident->m_pNext().value_or(nullptr)) {
                if (auto current_ent = ident->base_entity(); current_ent && current_ent->is_hero()) {
                    if (current_ent->team_num() != GameData::local_team) {
                        if (ConfigManager::illusions_detection && current_ent->is_illusion())
                            current_ent->set_client_seen_illusion_modifier(true);
                    }
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        LOG::DEBUG("Exiting lobby...");
        GameData::reset();
    }
}
