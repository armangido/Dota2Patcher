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

enum class ENTITY_NAME_TYPE {
    INTERNAL_NAME,
    ENTITY_NAME,
    BINARY_NAME,
    CLASS_NAME
};

enum class PATCH_TYPE {
    JE = 0x74,
    JNE = 0x75,
    TEST = 0x85,
    JMP = 0xEB,
    CUSTOM = 0x0
};

enum class ASM_TYPE {
    LEA, // 3, 7
    CALL // 2, 6
};

enum class DOTA_WEATHER {
    WEATHER_DEFAULT = 0,
    WEATHER_SNOW = 1,
    WEATHER_RAIN = 2,
    WEATHER_MOONBEAM = 3,
    WEATHER_PESTILENCE = 4,
    WEATHER_HARVEST = 5,
    WEATHER_SIROCCO = 6,
    WEATHER_SPRING = 7,
    WEATHER_ASH = 8,
    WEATHER_AURORA = 9
};
