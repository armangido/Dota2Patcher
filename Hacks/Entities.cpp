#include "Hacks.h"
#include "../SourceSDK/C_DOTAPlayerController.h"
#include "../SourceSDK/CGameEntitySystem.h"
#include "../SourceSDK/CBaseEntity.h"

bool GameData::find_local_hero() {
	const auto hero = vmt.entity_system->find_by_handle(GameData::local_player->assigned_hero_handle(), true);
	if (!hero)
		return false;

	GameData::local_hero = hero.value();

	return true;
}

bool GameData::find_local_player() {
	std::vector<C_DOTAPlayerController*> players = vmt.entity_system->find_vector_by_name<C_DOTAPlayerController>(CGameEntitySystem::NAME_TYPE::entity_name, "dota_player_controller");

	for (const auto player : players) {
		if (player->is_local_player()) {
			GameData::local_player = player;
			return true;
		}
	}

	return false;
}
