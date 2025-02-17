#pragma once
#include "interfaces.h"

class C_DOTAPlayerController {
public:
	int player_ID() const {
		const auto ptr = vmt.schema_system->get_netvar(this, "C_DOTAPlayerController", "m_nPlayerID");
		return Memory::read_memory<int>(ptr).value_or(0);
	}

	bool is_hero_assigned() const {
		const auto ptr = vmt.schema_system->get_netvar(this, "C_DOTAPlayerController", "m_bHeroAssigned");
		return ptr.value_or(0) != 0;
	}

	CHandle assigned_hero_handle() const {
		const auto ptr = vmt.schema_system->get_netvar(this, "C_DOTAPlayerController", "m_hAssignedHero");
		return Memory::read_memory<CHandle>(ptr).value_or(0);
	}

	bool is_local_player() const {
		const auto ptr = vmt.schema_system->get_netvar(this, "CBasePlayerController", "m_bIsLocalPlayerController");
		return ptr.value_or(0) != 0;
	}
};
