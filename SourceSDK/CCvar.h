#pragma once
#include "../Utils/Memory.h"

constexpr size_t CVAR_NODE_SIZE = 0x8;

class CCvarNode {
public:
	optional <string> name() const {
		const auto name_ptr = Memory::read_memory<uintptr_t>(this);
		return !name_ptr ? nullopt : Memory::read_string(name_ptr.value());
	}

	float get_float() const {
		return Memory::read_memory<float>(this + 0x40).value_or(0);
	}

	void set_int(int value) {
		Memory::write_memory<int>(this + 0x40, value);
	}
};

class CCvarNodes {
public:
	CCvarNode* node_by_index(const size_t index) const {
		return Memory::read_memory<CCvarNode*>(this + index * CVAR_NODE_SIZE).value_or(nullptr);
	}
};

class CCvar {
public:
	CCvarNodes* node() const {
		return Memory::read_memory<CCvarNodes*>(this + 0x40).value_or(nullptr);
	}

	optional<CCvarNode*> cvar_by_name(const string& name_to_find) const {
		const auto nodes = this->node();

		for (size_t index = 0; auto current_node = nodes->node_by_index(index); ++index) {
			if (const auto current_name = current_node->name(); current_name && current_name.value() == name_to_find)
				return current_node;
		}

		return nullopt;
	}
};
