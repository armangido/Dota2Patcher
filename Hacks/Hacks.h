#pragma once
#include "../Utils/Memory.h"

class C_DOTAPlayerController;
class CEntityIdentity;
class CBaseEntity;

class GameData {
public:
	static bool find_local_player();
	static bool find_local_hero();

	static void reset();
	static inline C_DOTAPlayerController* local_player;
	static inline CBaseEntity* local_hero;
	static inline int local_team;
	static inline uintptr_t dota_range_display;
};

class Hacks : public GameData {
public:
	static void start_worker();

	static void range_display(float range) {
		Memory::write_memory<float>(dota_range_display + 0x40, range);
	}
};
