#pragma once
#include "interfaces.h"

class C_DOTAPlayerController {
public:
	bool is_hero_assigned() const {
		const auto ptr = vmt.schema_system->get_netvar(this, "client.dll", "C_DOTAPlayerController", "m_bHeroAssigned");
		const auto value = Memory::read_memory<bool>(ptr.value());
        return value.value_or(0) != 0;
	}

	CHandle assigned_hero_handle() const {
		const auto ptr = vmt.schema_system->get_netvar(this, "client.dll", "C_DOTAPlayerController", "m_hAssignedHero");
		return Memory::read_memory<CHandle>(ptr.value()).value();
	}

	bool is_local_player() const {
		const auto ptr = vmt.schema_system->get_netvar(this, "client.dll", "CBasePlayerController", "m_bIsLocalPlayerController");
		const auto value = Memory::read_memory<bool>(ptr.value());
		return value.value_or(0) != 0;
	}
};
