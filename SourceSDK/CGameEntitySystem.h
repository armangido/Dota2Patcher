#pragma once
#include "memory.h"

class CBaseEntity;

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
	optional<CBaseEntity*> base_entity() const {
		return Memory::read_memory<CBaseEntity*>(this);
	}

	optional<CSchemaClassBinding*> schema_class_binding() const {
		return Memory::read_memory<CSchemaClassBinding*>(this + 0x8);
	}

	optional<uint32_t> handle() const {
		return Memory::read_memory<uint32_t>(this + 0x10);
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
		std::ofstream dump_file;
		dump_file.open("C:\\entity_dump.txt");
		int ents_count = 0;

		auto first_ent = this->get_first_entity();
		if (!first_ent)
			return;

		CEntityIdentity* ident = first_ent.value();

		while (ident->m_pNext().value()) {
			auto next_ent = ident->m_pNext();
			if (!next_ent)
				break;

			ident = next_ent.value();

			auto schema = ident->schema_class_binding();
			auto internal_name = ident->internal_name();
			auto entity_name = ident->entity_name();
			auto binary_name = schema.value()->binary_name();
			auto class_name = schema.value()->class_name();

			dump_file
				<< "internal_name: " << internal_name.value_or("empty") 
				<< " | entity_name: " << entity_name.value_or("empty") 
				<< " | binary_name: " << binary_name.value_or("empty") 
				<< " | class_name: " << class_name.value_or("empty")
				<< " -> [" << (void*)ident->base_entity().value() << "]\n";

			ents_count++;
		}
		
		dump_file.close();
		printf("\n");
		LOG::INFO("iterate_entities: done. Total: %d", ents_count);
	}

	optional<CBaseEntity*> find_by_name(string name_to_find) {
		auto first_ent = this->get_first_entity();
		if (!first_ent || first_ent.value() == nullptr) 
			return nullopt;

		CEntityIdentity* ident = first_ent.value();

		while (ident->m_pNext().value()) {
			auto next_ent = ident->m_pNext();
			if (!next_ent)
				break;

			ident = next_ent.value();

			auto internal_name = ident->internal_name();
			if (internal_name && internal_name.value() == name_to_find)
				return ident->base_entity();

			auto entity_name = ident->entity_name();
			if (entity_name && entity_name.value() == name_to_find)
				return ident->base_entity();
			
			auto schema = ident->schema_class_binding();
			if (!schema)
				continue;

			auto binary_name = schema.value()->binary_name();
			if (binary_name && binary_name.value() == name_to_find)
				return ident->base_entity();
			
			auto class_name = schema.value()->class_name();
			if (class_name && class_name.value() == name_to_find)
				return ident->base_entity();
		}

		return nullopt;
	}

	optional<CEntityIdentity*> get_first_entity() {
		return Memory::read_memory<CEntityIdentity*>(this + 0x210);
	}

	optional<CEntityIdentity*> get_base_entity(size_t index) {
		const auto chunk = get_identity_chunk();
		if (!chunk)
			return nullopt;

		uintptr_t identityPtr = reinterpret_cast<uintptr_t>(chunk.value()) + (index % MAX_ENTITIES_IN_LIST) * ENTITY_IDENTITY_SIZE;

		return reinterpret_cast<CEntityIdentity*>(identityPtr);
	}

	optional<CEntityIdentities*> get_identity_chunk() {
		return Memory::read_memory<CEntityIdentities*>(this + 0x10);
	}
};
