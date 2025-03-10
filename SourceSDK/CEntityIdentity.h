#pragma once
#include "../Utils/Memory.h"
#include "CHandle.h"

// Forward declaration
class CBaseEntity;
//

class SchemaName {
public:
	optional<string> name() const {
		const auto name_ptr = Memory::read_memory<uintptr_t>(this + 0x8);
		return !name_ptr ? nullopt : Memory::read_string(name_ptr.value());
	}
};

class CSchemaClassBinding {
public:
	optional<string> binary_name() const { // C_DOTA_Unit_Hero_AntiMage
		optional base = Memory::read_memory<SchemaName*>(this + 0x30);
		return !base ? nullopt : base.value()->name();
	}

	optional<string> class_name() const { // C_DOTA_BaseNPC_Hero
		optional base = Memory::read_memory<SchemaName*>(this + 0x38);
		return !base ? nullopt : base.value()->name();
	}
};

class CEntityIdentity {
public:
	CBaseEntity* base_entity() const {
		return Memory::read_memory<CBaseEntity*>(this).value_or(nullptr);
	}

	bool is_hero() const {
		const auto name = this->schema_class_binding()->class_name();
		return name.value_or("") == "C_DOTA_BaseNPC_Hero";
	}

	bool is_thinker() const {
		const auto name = this->internal_name();
		return name.value_or("") == "npc_dota_thinker";
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
