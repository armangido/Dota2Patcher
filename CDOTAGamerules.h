#pragma once
#include "Dota2Patcher.h"
#include "Memory.h"

enum DOTA_GameState : int {
    DOTA_GAMERULES_STATE_INVALID = -1,
    DOTA_GAMERULES_STATE_INIT = 0,
    DOTA_GAMERULES_STATE_WAIT_FOR_PLAYERS_TO_LOAD = 1,
    DOTA_GAMERULES_STATE_HERO_SELECTION = 2,
    DOTA_GAMERULES_STATE_STRATEGY_TIME = 3,
    DOTA_GAMERULES_STATE_PRE_GAME = 4,
    DOTA_GAMERULES_STATE_GAME_IN_PROGRESS = 5,
    DOTA_GAMERULES_STATE_POST_GAME = 6,
    DOTA_GAMERULES_STATE_DISCONNECT = 7,
    DOTA_GAMERULES_STATE_TEAM_SHOWCASE = 8,
    DOTA_GAMERULES_STATE_CUSTOM_GAME_SETUP = 9,
    DOTA_GAMERULES_STATE_WAIT_FOR_MAP_TO_LOAD = 10,
    DOTA_GAMERULES_STATE_SCENARIO_SETUP = 11,
    DOTA_GAMERULES_STATE_PLAYER_DRAFT = 12,
    DOTA_GAMERULES_STATE_LAST = 13,
};

class CDOTAGamerules {
public:
    CDOTAGamerules() = default;

    bool paused(HANDLE hProc) {
        auto value = memory_->read_memory<int>(hProc, gamerules_base_ + 0x38);
        if (!value.has_value())
            return false;
        return value.value();
    }

    DOTA_GameState game_state(HANDLE hProc) {
        auto value = memory_->read_memory<int>(hProc, gamerules_base_ + 0x74);
        if (!value.has_value())
            return DOTA_GAMERULES_STATE_INVALID;

        return static_cast<DOTA_GameState>(value.value());
    }

    bool in_lobby(HANDLE hProc) {
        auto gamerules_ptr = memory_->read_memory<uintptr_t>(hProc, gamerules_proxy_);
        if (!gamerules_ptr.has_value() || gamerules_ptr.value() < DOTA_GAMERULES_STATE_HERO_SELECTION)
            return false;

        gamerules_base_ = gamerules_ptr.value();
        printf("[+] C_DOTAGamerules: %p\n", reinterpret_cast<void*>(gamerules_base_));
        return true;
    }

    bool find_gamerules(HANDLE hProc, const Memory::ModuleInfo& module) {
        memory_ = std::make_unique<Memory>();

        uintptr_t base = memory_->pattern_scan(hProc, module, "48 8B ? ? ? ? ? 48 85 ? 74 ? 48 8B ? 45 33 ? 66 0F");
        if (base == -1)
            return false;

        uintptr_t gamerules_proxy = memory_->absolute_address(hProc, base, 3, 7);
        if (gamerules_proxy == -1)
            return false;
        
        // Not really C_DOTAGamerules_Proxy but who cares
        printf("[+] C_DOTAGamerules_Proxy: %p\n", reinterpret_cast<void*>(gamerules_proxy));
        gamerules_proxy_ = gamerules_proxy;

        return true;
    }

private:
    std::unique_ptr<Memory> memory_;
    uintptr_t gamerules_base_ = -1;
    uintptr_t gamerules_proxy_ = -1;
};
