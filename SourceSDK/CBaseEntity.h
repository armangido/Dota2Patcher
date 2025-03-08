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

	bool is_illusion() const {
		auto ptr = vmt.schema_system->get_netvar(this, "client.dll", "C_DOTA_BaseNPC_Hero", "m_hReplicatingOtherHeroModel");
		return Memory::read_memory<int>(ptr.value()).value() != -1 && !is_clone();
	}

	CGameSceneNode* scene_node() const {
		const auto ptr = vmt.schema_system->get_netvar(this, "client.dll", "C_BaseEntity", "m_pGameSceneNode");
		return Memory::read_memory<CGameSceneNode*>(ptr.value()).value_or(nullptr);
	}

	ModifierManager* modifier_manager() const {
		const auto ptr = vmt.schema_system->get_netvar(this, "server.dll", "CDOTA_BaseNPC", "m_ModifierManager");
		return reinterpret_cast<ModifierManager*>(ptr.value());
	}

	vector2D origin2D() const {
		return this->scene_node()->origin2D();
	}

	vector3D origin3D() const {
		return this->scene_node()->origin3D();
	}
};
