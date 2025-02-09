#pragma once
#include "../Dota2Patcher.h"
#include "../Memory.h"
#include <unordered_set>
#include <functional>

class Interface {
public:
	optional<uintptr_t> base() {
		const auto base_ptr = Memory::read_memory<uintptr_t>(this);
		if (!base_ptr)
			return nullopt;

		return Memory::absolute_address<uintptr_t>(base_ptr.value());;
	}

	optional<string> name() {
		const auto name_ptr = Memory::read_memory<uintptr_t>(this + 0x8);
		if (!name_ptr)
			return nullopt;

		return Memory::read_string(name_ptr.value());
	}

	optional<Interface*> next() {
		const auto next_ptr = Memory::read_memory<Interface*>(this + 0x10);
		if (next_ptr.value_or(nullptr) == nullptr)
			return nullopt;

		return next_ptr;
	}
};

class CreateInterface : public Interface {
public:
	struct ModuleInterfaces {
		string module_name;
		std::unordered_map<string, std::function<void(uintptr_t)>> interface_handlers;
	};

	static optional<Interface*> get_first_interface(string module_name) {
		const auto CreateInterfaceFn = Memory::pattern_scan(module_name, Patches::Patterns::CreateInterface);
		if (!CreateInterfaceFn) {
			LOG::CRITICAL("Can't find CreateInterface pattern!");
			return nullopt;
		}

		LOG::INFO("%s CreateInterface -> [%p]", module_name.c_str(), (void*)CreateInterfaceFn.value());

		const auto first_interface_base = Memory::absolute_address<uintptr_t>(CreateInterfaceFn.value());
		if (!first_interface_base)
			return nullopt;

		return Memory::read_memory<Interface*>(first_interface_base.value());
	}

	static void load_interfaces(const ModuleInterfaces& module, bool iterate_all = false) {
		const auto interface_ptr = get_first_interface(module.module_name);
		if (!interface_ptr)
			return;

		Interface* iface = interface_ptr.value();
		
		while (true) {
			const auto name = iface->name();
			if (!name)
				continue;

			const auto base = iface->base();
			if (!base)
				continue;

			if (iterate_all)
				LOG::INFO("[%s] -> [%p]", name.value().c_str(), (void*)base.value());

			else if (module.interface_handlers.find(name.value()) != module.interface_handlers.end()) {
				LOG::INFO("Interface [%s] -> [%p]", name.value().c_str(), (void*)base.value());
				module.interface_handlers.at(name.value())(base.value());
			}

			auto next_ptr = iface->next();
			if (!next_ptr) 
				break;

			iface = next_ptr.value();
		}
	}
};
