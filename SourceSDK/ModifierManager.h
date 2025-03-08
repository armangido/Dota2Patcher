#pragma once
#include "..\Utils\Memory.h"

class Modifier {
public:
	optional <string> name() const {
		const auto name_ptr = Memory::read_memory<uintptr_t>(this + 0x28);
		return !name_ptr ? nullopt : Memory::read_string(name_ptr.value());
	}
};

class ModifiersList {
public:
	Modifier* modifier(size_t index) {
		return Memory::read_memory<Modifier*>(this + index * 8).value_or(nullptr);
	}
};

class ModifierManager {
public:
	size_t count() const {
		return Memory::read_memory<size_t>(this + 0x10).value_or(0);
	}

	ModifiersList* list() const {
		return Memory::read_memory<ModifiersList*>(this + 0x18).value_or(nullptr);
	}

	std::vector<Modifier*> get_vector(ModifiersList* current_list) const {
		std::vector<Modifier*> found;
		size_t count = this->count();

		for (size_t i = 0; i <= count; i++) {
			const auto current_modifier = current_list->modifier(i);
			if (!current_modifier)
				continue;
			found.push_back(current_modifier);
		}

		return found;
	}

	optional <Modifier*> find_by_name(ModifiersList* current_list, const string& name) const {
		size_t count = this->count();

		for (size_t i = 0; i <= count; i++) {
			const auto current_modifier = current_list->modifier(i);
			if (!current_modifier)
				continue;

			const auto current_name = current_modifier->name();
			if (!current_name)
				continue;

			if (current_name.value() == name)
				return current_modifier;
		}

		return nullopt;
	}
};
