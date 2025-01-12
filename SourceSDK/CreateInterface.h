#pragma once
#include "../Dota2Patcher.h"
#include "../Memory.h"

class Interface {
public:
	std::optional<uintptr_t> base() {
		const auto base_ptr = Memory::read_memory<uintptr_t>(this);
		if (!base_ptr)
			return std::nullopt;

		const auto absolute_address = Memory::absolute_address<uintptr_t>(base_ptr.value());
		if (!absolute_address)
			return std::nullopt;

		return absolute_address;
	}

	std::optional<std::string> name() {
		const auto name_ptr = Memory::read_memory<uintptr_t>(this + 0x8);
		if (!name_ptr)
			return std::nullopt;

		const auto name = Memory::read_string(name_ptr.value());
		if (!name)
			return std::nullopt;

		return std::string(name.value());
	}

	std::optional<Interface*> next() {
		const auto next_ptr = Memory::read_memory<uintptr_t>(this + 0x10);
		if (next_ptr.value_or(0) == 0)
			return std::nullopt;

		return (Interface*)next_ptr.value();
	}
};

class CreateInterface : public Interface {
public:
	struct ModuleInterfaces {
		std::string module_name;
		std::vector<std::string> interface_names;
	};

	std::optional<Interface*> get_first_interface(std::string module_name) {
		const auto CreateInterfaceFn = Memory::pattern_scan(module_name, Patches::Patterns::CreateInterface);
		if (!CreateInterfaceFn) {
			printf("[-] Can't find CreateInterface pattern!\n");
			return std::nullopt;
		}

		printf("[+] %s CreateInterface -> [%p]\n", module_name.c_str(), (void*)CreateInterfaceFn.value());

		const auto first_interface_base = Memory::absolute_address<uintptr_t>(CreateInterfaceFn.value());
		if (!first_interface_base)
			return std::nullopt;

		const auto first_interface = Memory::read_memory<Interface*>(first_interface_base.value());
		if (!first_interface)
			return std::nullopt;

		return first_interface.value();
	}

	void load_interfaces(const ModuleInterfaces& module, bool iterate_all = false) {
		const auto interface_ptr = get_first_interface(module.module_name);
		if (!interface_ptr)
			return;

		Interface* iface = interface_ptr.value();

		while (true) {
			const auto name = iface->name();
			if (!name) continue;

			const auto base = iface->base();
			if (!base) continue;

			if (iterate_all)
				printf("[~] [%s] -> [%p]\n", name.value().c_str(), (void*)base.value());

			else if (std::find(module.interface_names.begin(), module.interface_names.end(), name.value()) != module.interface_names.end()) {
				interfaces[module.module_name][name.value()] = iface;
				printf("[+] Interface [%s] -> [%p]\n", name.value().c_str(), (void*)iface->base().value());
			}

			auto next_ptr = iface->next();
			if (!next_ptr) break;

			iface = next_ptr.value();
		}
	}


	std::unordered_map<std::string, std::unordered_map<std::string, Interface*>> interfaces;
};
