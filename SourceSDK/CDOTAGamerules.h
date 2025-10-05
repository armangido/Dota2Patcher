#pragma once
#include "../Utils/Memory.h"

class CDOTAGamerules {
public:
    bool paused() const {
        const auto paused = Memory::read_memory<bool>(this + 0x38);
        return paused.value_or(0) != 0;
    }

    DOTA_GAME_STATE game_state() const {
        const auto game_state = Memory::read_memory<int>(this + 0x78);
        std::cout << game_state;
        return game_state.has_value() ? static_cast<DOTA_GAME_STATE>(game_state.value()) : DOTA_GAME_STATE::INVALID;
    }

    bool in_game() const {
        const auto game_state = this->game_state();
        return game_state == DOTA_GAME_STATE::HERO_SELECTION || game_state == DOTA_GAME_STATE::STRATEGY_TIME || game_state == DOTA_GAME_STATE::PRE_GAME || game_state == DOTA_GAME_STATE::GAME_IN_PROGRESS || game_state == DOTA_GAME_STATE::CUSTOM_GAME_SETUP;
    }
};

class C_DOTAGamerulesProxy {
public:
    CDOTAGamerules* gamerules() const {
        return Memory::read_memory<CDOTAGamerules*>(this + 0x5E0).value_or(nullptr);
    }
};
