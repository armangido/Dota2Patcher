#pragma once
#include "interfaces.h"

class CBaseEntity {
public:
	CEntityIdentity* identity() const {
		return Memory::read_memory<CEntityIdentity*>(this + 0x10).value_or(nullptr);
	}

	optional <string> unit_name() const { // npc_dota_hero_antimage
		const auto ptr = vmt.schema_system->get_netvar(this, "C_DOTA_BaseNPC", "m_iszUnitName");
		return !ptr ? nullopt : Memory::read_string(ptr.value());
	}

	int unit_type() const {
		const auto ptr = vmt.schema_system->get_netvar(this, "C_DOTA_BaseNPC", "m_iUnitType");
		return Memory::read_memory<int>(ptr).value_or(-1);
	}

	bool is_hero() const {
		return this->unit_type() == 1;
	}

	bool has_client_seen_illusion_modifier() const {
		const auto ptr = vmt.schema_system->get_netvar(this, "C_DOTA_BaseNPC", "m_bHasClientSeenIllusionModifier");
		return ptr.value_or(0) != 0;
	}

	void set_client_seen_illusion_modifier(bool val) {
		const auto ptr = vmt.schema_system->get_netvar(this, "C_DOTA_BaseNPC", "m_bHasClientSeenIllusionModifier");
		Memory::write_memory(ptr.value(), val);
	}

	bool is_illusion() const {
		const auto ptr = vmt.schema_system->get_netvar(this, "C_DOTA_BaseNPC_Hero", "m_hReplicatingOtherHeroModel");
		const auto result = Memory::read_memory<uint32_t>(ptr.value());
		return result.value() != 0xFFFFFFFF;
	}
};
