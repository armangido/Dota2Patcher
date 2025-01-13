#pragma once
#include "interfaces.h"

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
    std::optional<bool> paused() {
        const auto paused = Memory::read_memory<bool>(this + 0x38);
        return paused.value_or(0) != 0;
    }

    std::optional<DOTA_GameState> game_state() {
        const auto game_state = Memory::read_memory<int>(this + 0x74);
        return game_state.has_value() ? static_cast<DOTA_GameState>(game_state.value()) : DOTA_GameState::INVALID;
    }

    bool in_lobby() {
        const auto gamerules_ptr = Memory::read_memory<uintptr_t>(this);
        if (gamerules_ptr.value_or(0) == 0)
            return false;

        vmt.gamerules = (CDOTAGamerules*)gamerules_ptr.value();
        printf("[+] C_DOTAGamerules -> [%p]\n", reinterpret_cast<void*>(gamerules_ptr.value()));

        return true;
    }
};
