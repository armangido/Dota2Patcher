#pragma once
#include "interfaces.h"
#include "CGameSceneNode.h"
#include "ModifierManager.h"

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
		const auto ptr = vmt.schema_system->get_netvar(this, "client.dll", "C_BaseEntity", "m_iTeamNum");
		return Memory::read_memory<int>(ptr.value()).value_or(-1);
	}

	bool visible() const {
		const auto ptr = vmt.schema_system->get_netvar(this, "client.dll", "C_DOTA_BaseNPC", "m_iTaggedAsVisibleByTeam");
		const auto value = Memory::read_memory<int>(ptr.value());
		return value.value_or(14) == 14 || value.value_or(30) == 30;
	}

	void set_client_seen_illusion_modifier(bool val) {
		const auto ptr = vmt.schema_system->get_netvar(this, "client.dll", "C_DOTA_BaseNPC", "m_bHasClientSeenIllusionModifier");
		Memory::write_memory(ptr.value(), val);
	}

	bool is_clone() const {
		const auto ptr = vmt.schema_system->get_netvar(this, "client.dll", "C_DOTA_BaseNPC", "m_bIsClone");
		return Memory::read_memory<bool>(ptr.value()).value();
	}

	DOTA_HERO_ID heroID() const {
		const auto ptr = vmt.schema_system->get_netvar(this, "client.dll", "C_DOTA_BaseNPC_Hero", "m_iHeroID");
		return Memory::read_memory<DOTA_HERO_ID>(ptr.value()).value();
	}

	bool is_illusion() const {
		const auto ptr = vmt.schema_system->get_netvar(this, "client.dll", "C_DOTA_BaseNPC_Hero", "m_hReplicatingOtherHeroModel");

		if (Memory::read_memory<int>(ptr.value()).value_or(-1) == -1)
			return false;

		switch (this->heroID()) {
		case DOTA_HERO_ID::npc_dota_hero_meepo:
			return !this->is_clone();
		case DOTA_HERO_ID::npc_dota_hero_morphling:
			return !this->c_modifier_manager()->find_by_name("modifier_morphling_replicate");
		case DOTA_HERO_ID::npc_dota_hero_arc_warden:
			return !this->c_modifier_manager()->find_by_name("modifier_arc_warden_tempest_double");
		default:
			return true;
		}
	}

	CGameSceneNode* scene_node() const {
		const auto ptr = vmt.schema_system->get_netvar(this, "client.dll", "C_BaseEntity", "m_pGameSceneNode");
		return Memory::read_memory<CGameSceneNode*>(ptr.value()).value_or(nullptr);
	}

	ModifierManager* c_modifier_manager() const {
		const auto ptr = vmt.schema_system->get_netvar(this, "client.dll", "C_DOTA_BaseNPC", "m_ModifierManager");
		return reinterpret_cast<ModifierManager*>(ptr.value());
	}

	ModifierManager* s_modifier_manager() const {
		const auto ptr = vmt.schema_system->get_netvar(this, "server.dll", "CDOTA_BaseNPC", "m_ModifierManager");
		return reinterpret_cast<ModifierManager*>(ptr.value());
	}

	Vector origin() const {
		return this->scene_node()->origin();
	}
};
