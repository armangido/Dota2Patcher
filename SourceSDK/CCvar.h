#pragma once
#include "../Utils/Memory.h"

class CCvarNode {
public:
	optional <string> name() const {
		const auto name_ptr = Memory::read_memory<uintptr_t>(this);
		return !name_ptr ? nullopt : Memory::read_string(name_ptr.value());
	}

	template <typename T>
	T get() const {
		return Memory::read_memory<T>(this + 0x40);
	}

	template <typename T>
	void set(const T value) {
		Memory::write_memory<T>(this + 0x40, value);
	}
};

class CCvarNodes {
public:
	CCvarNode* node_by_index(const size_t index) const {
		return Memory::read_memory<CCvarNode*>(this + index * 8).value_or(nullptr);
	}
};

class CCvar {
public:
	CCvarNodes* node() const {
		return Memory::read_memory<CCvarNodes*>(this + 0x40).value_or(nullptr);
	}

	optional<CCvarNode*> find_by_name(const string& name_to_find) const {
		for (size_t index = 0; auto current_node = this->node()->node_by_index(index); ++index) {
			if (const auto current_name = current_node->name(); current_name && current_name.value() == name_to_find)
				return current_node;
		}

		return nullopt;
	}

	size_t load_convars() const {
		size_t count = 0;
		for (size_t index = 0; auto current_node = this->node()->node_by_index(index); ++index) {
			if (const auto current_name = current_node->name(); current_name) {
				g_convars[current_name.value()] = current_node;
				count++;
			}
		}

		return count;
	}

	static inline std::unordered_map<string, CCvarNode*> g_convars;
};
