#pragma once
#include "../Utils/Memory.h"

class C_DOTAPlayerController;
class CEntityIdentity;
class CBaseEntity;
class CCvarNode;

class GameData {
public:
	static bool find_local_player();
	static bool find_local_hero();

	static void reset();
	static inline C_DOTAPlayerController* local_player;
	static inline CBaseEntity* local_hero;
	static inline CCvarNode* dota_range_display;
	static inline int local_team;
};

class Hacks : public GameData {
public:
	static void start_worker();
};
