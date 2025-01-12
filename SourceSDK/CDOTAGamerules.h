#pragma once
#include "../Dota2Patcher.h"
#include "../Memory.h"

enum class DOTA_GameState : int {
    INVALID = -1,
    INIT = 0,
    WAIT_FOR_PLAYERS_TO_LOAD = 1,
    HERO_SELECTION = 2,
    STRATEGY_TIME = 3,
    PRE_GAME = 4,
    GAME_IN_PROGRESS = 5,
    POST_GAME = 6,
    DISCONNECT = 7,
    TEAM_SHOWCASE = 8,
    CUSTOM_GAME_SETUP = 9,
    WAIT_FOR_MAP_TO_LOAD = 10,
    SCENARIO_SETUP = 11,
    PLAYER_DRAFT = 12,
    LAST = 13,
};

class CDOTAGamerules {
public:
    static bool paused() {
        const auto value = Memory::read_memory<int>(gamerules_base_ + 0x38);
        return value.value_or(0) != 0;
    }

    static DOTA_GameState game_state(HANDLE hProc) {
        const auto value = Memory::read_memory<int>(gamerules_base_ + 0x74);
        return value.has_value() ? static_cast<DOTA_GameState>(value.value()) : DOTA_GameState::INVALID;
    }

    static bool in_lobby() {
        const auto gamerules_ptr = Memory::read_memory<uintptr_t>(gamerules_proxy_);
        if (!gamerules_ptr || gamerules_ptr.value() < static_cast<uintptr_t>(DOTA_GameState::HERO_SELECTION)) {
            return false;
        }

        gamerules_base_ = gamerules_ptr.value();
        printf("[+] C_DOTAGamerules -> [%p]\n", reinterpret_cast<void*>(gamerules_base_));
        return true;
    }

    static bool find_gamerules() {
        static const auto base = Memory::pattern_scan("client.dll", Patches::Patterns::CDOTAGamerules);
        if (!base) {
            return false;
        }

        const auto gamerules_proxy = Memory::absolute_address<uintptr_t>(base.value());
        if (gamerules_proxy.value_or(0) == 0) {
            return false;
        }

        // Not really C_DOTAGamerules_Proxy but who cares
        printf("[+] C_DOTAGamerules_Proxy -> [%p]\n", reinterpret_cast<void*>(gamerules_proxy.value()));
        gamerules_proxy_ = gamerules_proxy.value();

        return true;
    }

private:
    static inline uintptr_t gamerules_base_;
    static inline uintptr_t gamerules_proxy_;
};
