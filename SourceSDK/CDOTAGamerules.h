#pragma once
#include "../Utils/Memory.h"

class CDOTAGamerules {
public:
    bool paused() const {
        const auto paused = Memory::read_memory<bool>(this + 0x38);
        return paused.value_or(0) != 0;
    }

    DOTA_GAME_STATE game_state() const {
        const auto game_state = Memory::read_memory<int>(this + 0x74);
        return game_state.has_value() ? static_cast<DOTA_GAME_STATE>(game_state.value()) : DOTA_GAME_STATE::INVALID;
    }

    bool in_game() const {
        const auto game_state = this->game_state();
        return game_state >= DOTA_GAME_STATE::HERO_SELECTION && game_state <= DOTA_GAME_STATE::GAME_IN_PROGRESS;
    }
};

class C_DOTAGamerulesProxy {
public:
    CDOTAGamerules* gamerules() const {
        return Memory::read_memory<CDOTAGamerules*>(this + 0x510).value_or(nullptr);
    }
};
