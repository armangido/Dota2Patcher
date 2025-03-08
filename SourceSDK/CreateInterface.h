#pragma once
#include <unordered_set>
#include <functional>
#include "../Dota2Patcher.h"
#include "../Utils/Memory.h"

class Interface {
public:
	uintptr_t base() const {
		const auto base_ptr = Memory::read_memory<uintptr_t>(this);
		return Memory::absolute_address<uintptr_t>(base_ptr.value()).value_or(0);
	}

	optional<string> name() const {
		const auto name_ptr = Memory::read_memory<uintptr_t>(this + 0x8);
		return !name_ptr ? nullopt : Memory::read_string(name_ptr.value());
	}

	optional<Interface*> next() const {
		return Memory::read_memory<Interface*>(this + 0x10);
	}
};

class CreateInterface : public Interface {
public:
	struct ModuleInterfaces {
		string module_name;
		std::unordered_map<string, std::function<void(uintptr_t)>> interface_handlers;
	};

	static optional<Interface*> get_first_interface(const string& module_name) {
		const auto CreateInterfaceFn = Memory::pattern_scan(module_name, Patches::Patterns::create_interface);
		if (!CreateInterfaceFn) {
			LOG::CRITICAL("Can't find [{}] CreateInterface pattern!", module_name);
			return nullopt;
		}

		LOG::INFO("{} CreateInterface -> [{}]", module_name, TO_VOID(CreateInterfaceFn.value()));

		const auto first_interface_base = Memory::absolute_address<uintptr_t>(CreateInterfaceFn.value());
		if (!first_interface_base)
			return nullopt;

		return Memory::read_memory<Interface*>(first_interface_base.value());
	}

	static void load_interfaces(const ModuleInterfaces& module, bool dump_to_file = false) {
		const auto interface_ptr = get_first_interface(module.module_name);
		if (!interface_ptr)
			return;

		Interface* iface = interface_ptr.value();
		std::unordered_set<std::string> seen_interfaces;

		std::ofstream dump_file;
		if (dump_to_file) {
			std::filesystem::create_directories("C:\\interfaces");
			dump_file.open("C:\\interfaces\\" + module.module_name + ".txt");
		}

		while (iface) {
			const auto name = iface->name();
			if (!name || seen_interfaces.contains(name.value())) {
				iface = iface->next().value_or(nullptr);
				continue;
			}

			const auto base = iface->base();
			if (!base || base == 0) {
				iface = iface->next().value_or(nullptr);
				continue;
			}

			seen_interfaces.insert(name.value());
			
			if (dump_to_file)
				dump_file << "[" << name.value() << "] -> [" << TO_VOID(base) << "]\n";
			
			if (module.interface_handlers.find(name.value()) != module.interface_handlers.end()) {
				LOG::INFO("Interface [{}] -> [{}]", name.value(), TO_VOID(base));
				module.interface_handlers.at(name.value())(base);
			}

			iface = iface->next().value_or(nullptr);
		}

		if (dump_file.is_open())
			dump_file.close();
	}
};
