#include "Hacks.h"
#include "../SourceSDK/interfaces.h"
#include "../SourceSDK/C_DOTATeam.h"
#include "../Utils/Config.h"

void Hacks::hack_roshan_timer() {
	std::vector<C_DOTATeam*> dota_teams = vmt.entity_system->find_vector_by_name<C_DOTATeam>(CGameEntitySystem::NAME_TYPE::binary_name, "C_DOTATeam");

	for (auto team : dota_teams) {
		team->can_see_roshan_timer(true);
	}
}
