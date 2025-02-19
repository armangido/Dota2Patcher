#pragma once
#include "../Utils/Memory.h"
#include "CGameEntitySystem.h"
#include "CHandle.h"

// Forward declaration
class CBaseEntity;
class CSchemaClassBinding;
//

class CEntityIdentity {
public:
	CBaseEntity* base_entity() const {
		return Memory::read_memory<CBaseEntity*>(this).value_or(nullptr);
	}

	bool is_hero() const {
		const auto name = this->internal_name();
		return strstr(name.value_or("").c_str(), "npc_dota_hero_");
	}

	CSchemaClassBinding* schema_class_binding() const {
		return Memory::read_memory<CSchemaClassBinding*>(this + 0x8).value_or(nullptr);
	}

	optional <CHandle> handle() const {
		return Memory::read_memory<CHandle>(this + 0x10);
	}

	optional<string> internal_name() const { // npc_dota_hero_antimage
		const auto name_ptr = Memory::read_memory<uintptr_t>(this + 0x18);
		return !name_ptr ? nullopt : Memory::read_string(name_ptr.value());
	}

	optional<string> entity_name() const { // npc_dota_hero_antimage
		const auto name_ptr = Memory::read_memory<uintptr_t>(this + 0x20);
		return !name_ptr ? nullopt : Memory::read_string(name_ptr.value());
	}

	optional<CEntityIdentity*> m_pPrev() const {
		return Memory::read_memory<CEntityIdentity*>(this + 0x58);
	}

	optional<CEntityIdentity*> m_pNext() const {
		return Memory::read_memory<CEntityIdentity*>(this + 0x60);
	}
};
