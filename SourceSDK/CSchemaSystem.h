#pragma once
#include "utlthash.h"

#define CONTAINER_MAX_INDEX 256
#define CONTAINER_SIZE 0x20
#define CLASS_CONTAINER_SIZE 0x18

class CSchemaField {
public:
	optional<string> class_name() {
		const auto name_ptr = Memory::read_memory<uintptr_t>(this + 0x8);
		if (!name_ptr)
			return nullopt;

		return Memory::read_string(name_ptr.value());
	}
};

class CSchemaClassBindingBase {
public:
	optional<CSchemaField*> container(size_t index) {
		uintptr_t schema_field_base = reinterpret_cast<uintptr_t>(this + index * CONTAINER_SIZE);
		const auto schema_field = Memory::read_memory<CSchemaField*>(schema_field_base + 0x10);
		if (!schema_field || schema_field.value() == 0)
			return nullopt;

		return schema_field;
	}
};

class CSchemaSystemTypeScope {
public:
	optional<CSchemaClassBindingBase*> container(int index) {
		uintptr_t container_base = reinterpret_cast<uintptr_t>((this + 0x580) + (index % CONTAINER_MAX_INDEX) * CONTAINER_SIZE);
		return Memory::read_memory<CSchemaClassBindingBase*>(container_base);
	}

	optional<string> scope_name() {
		return Memory::read_string(this + 0x8);
	}
};

class CSchemaSystem {
public:
	optional<CSchemaSystemTypeScope*> get_type_scope(string scope_name) {
		const auto scopes_list = Memory::read_memory<uintptr_t>(this + 0x190);
		if (!scopes_list)
			return nullopt;

		for (uintptr_t i = 0; i < 18; i++) {
			const auto current_scope = Memory::read_memory<CSchemaSystemTypeScope*>(scopes_list.value() + i * 8);
			if (!current_scope || current_scope.value_or(nullptr) == nullptr)
				continue;

			const auto current_name = current_scope.value()->scope_name();
			if (!current_name || current_name.value() != scope_name)
				continue;

			return current_scope;
		}

		return nullopt;
	}
};
