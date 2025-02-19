#pragma once
#include "interfaces.h"

class CEntityIdentity;

class CBaseEntity {
public:
	CEntityIdentity* identity() const {
		return Memory::read_memory<CEntityIdentity*>(this + 0x10).value_or(nullptr);
	}

	bool is_hero() const {
		return this->identity()->is_hero();
	}
	
	int team_num() const {
		const auto ptr = vmt.schema_system->get_netvar(this, "C_BaseEntity", "m_iTeamNum");
		return Memory::read_memory<int>(ptr.value()).value_or(-1);
	}

	bool visible() const {
		const auto ptr = vmt.schema_system->get_netvar(this, "C_DOTA_BaseNPC", "m_iTaggedAsVisibleByTeam");
		const auto value = Memory::read_memory<int>(ptr.value());
		return value.value() == 14 || value.value() == 30;
	}

	void set_custom_health_label(const string& label) {
		const auto ptr = vmt.schema_system->get_netvar(this, "C_DOTA_BaseNPC", "m_CustomHealthLabel");
		Memory::write_string(ptr.value(), label);
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
		const auto result = Memory::read_memory<int>(ptr.value());
		return result.value() != -1;
	}
};
