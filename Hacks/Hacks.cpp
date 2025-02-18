#include "Hacks.h"
#include "../SourceSDK/interfaces.h"
#include "../SourceSDK/C_DOTATeam.h"
#include "../Utils/Config.h"

void Hacks::hack_roshan_timer() {
	std::vector<CBaseEntity*> dota_teams = vmt.entity_system->find_vector_by_name(CGameEntitySystem::NAME_TYPE::binary_name, "C_DOTATeam");

	for (auto team : dota_teams) {
		const auto dota_team = (C_DOTATeam*)team;
		dota_team->can_see_roshan_timer(true);
	}
}
