#pragma once

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
    optional<bool> paused() {
        const auto paused = Memory::read_memory<bool>(this + 0x38);
        return paused.value_or(0) != 0;
    }

    optional<DOTA_GameState> game_state() {
        const auto game_state = Memory::read_memory<int>(this + 0x74);
        return game_state.has_value() ? static_cast<DOTA_GameState>(game_state.value()) : DOTA_GameState::INVALID;
    }
};

class C_DOTAGamerulesProxy {
public:
    optional<CDOTAGamerules*> gamerules() {
        const auto gamerules_ptr = Memory::read_memory<uintptr_t>(this + 0x560);
        if (!gamerules_ptr)
            return nullopt;

        return reinterpret_cast<CDOTAGamerules*>(gamerules_ptr.value());
    }
};
