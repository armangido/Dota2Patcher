#pragma once
#include "memory.h"

class CBaseEntity;

class CEntityIdentity {
public:
	std::optional<CBaseEntity*> base_entity() const {
		const auto entity = Memory::read_memory<CBaseEntity*>(this);
		return entity;
	}

	std::optional<uint32_t> handle() const {
		const auto handle = Memory::read_memory<uint32_t>(this + 0x10);
		return handle;
	}

	std::optional<std::string> internal_name() const {
		const auto internal_name = Memory::read_memory<const char*>(this + 0x20);
		return internal_name;
	}

	std::optional<std::string> entity_name() const {
		const auto entity_name = Memory::read_memory<const char*>(this + 0x28);
		return entity_name;
	}

	std::optional<CEntityIdentity*> m_pPrev() const {
		const auto prev_ident = Memory::read_memory<CEntityIdentity*>(this + 0x58);
		return prev_ident;
	}

	std::optional<CEntityIdentity*> m_pNext() const {
		const auto next_ident = Memory::read_memory<CEntityIdentity*>(this + 0x58);
		return next_ident;
	}
};

#define MAX_ENTITIES_IN_LIST 512
#define MAX_ENTITY_LISTS 64
#define MAX_TOTAL_ENTITIES MAX_ENTITIES_IN_LIST * MAX_ENTITY_LISTS
#define ENTITY_IDENTITY_SIZE 0x78

class CEntityIdentities {
public:
	CEntityIdentity m_pIdentities[MAX_ENTITIES_IN_LIST];
};

class CGameEntitySystem {
public:
	std::optional<CBaseEntity*> get_base_entity(int index) {
		const auto chunk = get_identity_chunk();
		if (!chunk)
			return std::nullopt;

		uintptr_t identityPtr = reinterpret_cast<uintptr_t>(chunk.value()) + (index % MAX_ENTITIES_IN_LIST) * ENTITY_IDENTITY_SIZE;

		const auto base_entity = Memory::read_memory<CBaseEntity*>(identityPtr);
		return base_entity;
	}

	std::optional<CEntityIdentities*> get_identity_chunk() {
		const auto identity_chunks_address = Memory::read_memory<CEntityIdentities*>(this + 0x10);
		return identity_chunks_address;
	}

	std::optional<int> highest_entity_index() {
		const auto index = Memory::read_memory<int>(this + 0x1670);
		return index;
	}
};
