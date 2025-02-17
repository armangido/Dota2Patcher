#pragma once
#include "../Utils/Memory.h"
#include "CGameEntitySystem.h"

// Forward declaration
class CBaseEntity;
class CSchemaClassBinding;
//

class CEntityIdentity {
public:
	CBaseEntity* base_entity() const {
		return Memory::read_memory<CBaseEntity*>(this).value_or(nullptr);
	}

	CSchemaClassBinding* schema_class_binding() const {
		return Memory::read_memory<CSchemaClassBinding*>(this + 0x8).value_or(nullptr);
	}

	CHandle handle() const {
		return Memory::read_memory<CHandle>(this + 0x10).value_or(0);
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
