#pragma once
#include "utlthash.h"

class CSchemaField {
public:
	optional<string> name() {
		auto pAddr = Memory::read_string(this);
		return pAddr;
	}

	optional<uint32_t> offset() {
		return Memory::read_memory<uint32_t>(this + 0x10);
	}
};

class CSchemaClassBindingBase {
public:
	optional<string> name() {
		auto pAddr = Memory::read_string(this + 0x8);
		return pAddr;
	}

	optional<string> project_name() {
		return Memory::read_string(this + 0x10);
	}

	optional<uint32_t> size() {
		return Memory::read_memory<uint32_t>(this + 0x18);
	}

	optional<uint16_t> fields_num() {
		return Memory::read_memory<uint16_t>(this + 0x1C);
	}

	optional<CSchemaField*> fields() {
		return Memory::read_memory<CSchemaField*>(this + 0x28);
	}
};


class CSchemaSystem {
public:
	optional<CUtlTSHash<CSchemaClassBindingBase*, 256, unsigned int>> bindings_table() {
		return Memory::read_memory<CUtlTSHash<CSchemaClassBindingBase*, 256, unsigned int>>(this + 0x228);
	};
};
