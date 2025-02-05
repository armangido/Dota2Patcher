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
		const auto name_ptr = Memory::read_memory<uintptr_t>(this + 0x20);
		return !name_ptr ? std::nullopt : Memory::read_string(name_ptr.value());
	}

	std::optional<std::string> entity_name() const {
		const auto name_ptr = Memory::read_memory<uintptr_t>(this + 0x28);
		return !name_ptr ? std::nullopt : Memory::read_string(name_ptr.value());
	}

	std::optional<CEntityIdentity*> m_pPrev() const {
		const auto prev_ident = Memory::read_memory<CEntityIdentity*>(this + 0x58);
		return prev_ident;
	}

	std::optional<CEntityIdentity*> m_pNext() const {
		const auto next_ident = Memory::read_memory<CEntityIdentity*>(this + 0x60);
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
	void iterate_entities() { // for testing purposes
		for (int i = 0; i < this->highest_entity_index().value(); i++) {
			auto entity = this->get_base_entity(i);
			if (!entity)
				continue;

			CEntityIdentity* ident = entity.value();

			auto internal_name = ident->internal_name();
			if (!internal_name)
				continue;

			printf("%s -> [%p]\n", internal_name.value().c_str(), (void*)ident->base_entity().value());
		}
	}

	std::optional<CEntityIdentity*> get_base_entity(int index) {
		const auto chunk = get_identity_chunk();
		if (!chunk)
			return std::nullopt;

		uintptr_t identityPtr = reinterpret_cast<uintptr_t>(chunk.value()) + (index % MAX_ENTITIES_IN_LIST) * ENTITY_IDENTITY_SIZE;

		return reinterpret_cast<CEntityIdentity*>(identityPtr);
	}

	std::optional<CEntityIdentities*> get_identity_chunk() {
		const auto identity_chunks_address = Memory::read_memory<CEntityIdentities*>(this + 0x10);
		return identity_chunks_address;
	}

	std::optional<int> highest_entity_index() {
		const auto index = Memory::read_memory<int>(this + 0x2100);
		return index;
	}
};
