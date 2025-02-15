#pragma once
#include "interfaces.h"
#include "../Utils/Memory.h"

class C_DOTATeam {
public:
	void can_see_roshan_timer(bool val) {
		if (const auto ptr = vmt.schema_system->get_netvar(this, "C_DOTATeam", "m_bTeamCanSeeRoshanTimer"))
			Memory::write_memory<bool>(ptr.value(), val);
	}

	int hero_kills() const {
		const auto ptr = vmt.schema_system->get_netvar(this, "C_DOTATeam", "m_iHeroKills");
		return Memory::read_memory<int>(ptr.value_or(0)).value_or(0);
	}

	int tower_kills() const {
		const auto ptr = vmt.schema_system->get_netvar(this, "C_DOTATeam", "m_iTowerKills");
		return Memory::read_memory<int>(ptr.value_or(0)).value_or(0);
	}

	int barracks_kills() const {
		const auto ptr = vmt.schema_system->get_netvar(this, "C_DOTATeam", "m_iBarracksKills");
		return Memory::read_memory<int>(ptr.value_or(0)).value_or(0);
	}
};
