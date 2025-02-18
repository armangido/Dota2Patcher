#include "Hacks.h"
#include "../SourceSDK/C_DOTAPlayerController.h"
#include "../SourceSDK/CGameEntitySystem.h"
#include "../SourceSDK/CBaseEntity.h"

bool GameData::find_local_hero() {
	const auto hero = vmt.entity_system->find_by_handle(GameData::local_player->assigned_hero_handle());
	if (!hero)
		return false;

	GameData::local_hero = hero.value();

	return true;
}

bool GameData::find_local_player() {
	std::vector<CBaseEntity*> players = vmt.entity_system->find_vector_by_name<CBaseEntity>(CGameEntitySystem::NAME_TYPE::entity_name, "dota_player_controller");

	for (const auto player : players) {
		const auto player_controller = (C_DOTAPlayerController*)player;

		if (player_controller->is_local_player()) {
			GameData::local_player = player_controller;
			return true;
		}
	}

	return false;
}
