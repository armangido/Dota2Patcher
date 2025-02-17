#pragma once

class C_DOTAPlayerController;
class CBaseEntity;

class GameData {
public:
	static inline optional <C_DOTAPlayerController*> local_player;
	static inline optional <CBaseEntity*> local_hero;

	static inline void reset() {
		local_player.reset();
		local_hero.reset();
	}
};

class Hacks : public GameData {
public:
	static bool local_player_found() {
		return local_player != nullopt;
	}

	static bool local_hero_found() {
		return local_hero != nullopt;
	}

	static bool find_local_player();
	static bool find_local_hero();
};

//std::vector<CBaseEntity*> dota_teams = vmt.entity_system->find_vector_by_name(CGameEntitySystem::NAME_TYPE::binary_name, "C_DOTATeam");

//for (auto team : dota_teams) {
//	auto dota_team = (C_DOTATeam*)team;
//	dota_team->can_see_roshan_timer(true);
//}

//while (vmt.gamerules->in_game())
//	LOG::INFO("%d", vmt.gamerules->game_state());
