#include "Hacks.h"
#include "../SourceSDK/C_DOTAPlayerController.h"
#include "../SourceSDK/CGameEntitySystem.h"
#include "../SourceSDK/CBaseEntity.h"

bool Hacks::find_local_hero() {
	const auto hero = vmt.entity_system->find_by_handle(GameData::local_player.value()->assigned_hero_handle());
	if (!hero)
		return false;

	GameData::local_hero = hero.value();
	LOG::INFO("Local Hero: [%s] -> [%p]", hero.value()->identity()->entity_name().value().c_str(), (void*)hero.value());

	return true;
}

bool Hacks::find_local_player() {
	std::vector<CBaseEntity*> players = vmt.entity_system->find_vector_by_name(CGameEntitySystem::NAME_TYPE::entity_name, "dota_player_controller");

	for (const auto player : players) {
		const auto player_controller = (C_DOTAPlayerController*)player;

		if (player_controller->is_local_player()) {
			GameData::local_player = player_controller;
			LOG::INFO("Local Player -> [%p]", (void*)player_controller);
			return true;
		}
	}

	return false;
}
