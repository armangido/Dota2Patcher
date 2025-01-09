#pragma once
#include "Dota2Patcher.h"
#include "Memory.h"

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
    CDOTAGamerules() = default;

    bool paused(HANDLE hProc) {
        const auto value = memory_->read_memory<int>(hProc, gamerules_base_ + 0x38);
        return value.value_or(0) != 0;
    }

    DOTA_GameState game_state(HANDLE hProc) {
        const auto value = memory_->read_memory<int>(hProc, gamerules_base_ + 0x74);
        return value.has_value() ? static_cast<DOTA_GameState>(value.value()) : DOTA_GameState::INVALID;
    }

    bool in_lobby(HANDLE hProc) {
        const auto gamerules_ptr = memory_->read_memory<uintptr_t>(hProc, gamerules_proxy_);
        if (!gamerules_ptr.has_value() || gamerules_ptr.value() < static_cast<uintptr_t>(DOTA_GameState::HERO_SELECTION)) {
            return false;
        }

        gamerules_base_ = gamerules_ptr.value();
        printf("[+] C_DOTAGamerules: %p\n", reinterpret_cast<void*>(gamerules_base_));
        return true;
    }

    bool find_gamerules(HANDLE hProc, const Memory::ModuleInfo& module) {
        memory_ = std::make_unique<Memory>();

        const uintptr_t base = memory_->pattern_scan(hProc, module, "48 8B ? ? ? ? ? 48 85 ? 74 ? 48 8B ? 45 33 ? 66 0F");
        if (base == static_cast<uintptr_t>(-1)) {
            return false;
        }

        const uintptr_t gamerules_proxy = memory_->absolute_address(hProc, base, 3, 7);
        if (gamerules_proxy == static_cast<uintptr_t>(-1)) {
            return false;
        }

        // Not really C_DOTAGamerules_Proxy but who cares
        printf("[+] C_DOTAGamerules_Proxy: %p\n", reinterpret_cast<void*>(gamerules_proxy));
        gamerules_proxy_ = gamerules_proxy;

        return true;
    }

private:
    std::unique_ptr<Memory> memory_;
    uintptr_t gamerules_base_ = static_cast<uintptr_t>(-1);
    uintptr_t gamerules_proxy_ = static_cast<uintptr_t>(-1);
};
