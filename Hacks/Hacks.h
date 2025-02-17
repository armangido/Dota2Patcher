#pragma once

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
};

class Hacks : public GameData {
public:
	static bool hacks_enabled();
	static void start_worker();
	static void hack_roshan_timer();
};
