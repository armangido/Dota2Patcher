#pragma once
#include "../Dota2Patcher.h"
#include "../Memory.h"

class Interfase {
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

	std::optional<Interfase*> next() {
		const auto next_ptr = Memory::read_memory<uintptr_t>(this + 0x10);
		if (next_ptr.value_or(0) == 0)
			return std::nullopt;

		return (Interfase*)next_ptr.value();
	}
};

class CreateInterface : public Interfase {
public:
	bool get_create_interface(std::string module_name) {
		const auto CreateInterface_base = Memory::pattern_scan(module_name, Patches::Patterns::CreateInterface);
		if (!CreateInterface_base) {
			printf("[-] Can't find CreateInterface pattern!\n");
			return false;
		}

		const auto CreateInterfacePtr = Memory::absolute_address<uintptr_t>(CreateInterface_base.value());
		if (!CreateInterfacePtr)
			return false;

		const auto CreateInterfaceFn = Memory::read_memory<uintptr_t>(CreateInterfacePtr.value());
		if (!CreateInterfaceFn)
			return false;

		printf("[+] %s CreateInterface -> [%p]\n", module_name.c_str(), (void*)CreateInterfaceFn.value());

		pCreateInterface_ = (Interfase*)CreateInterfaceFn.value();
		return true;
	}

	void load_interfaces(std::string module_name, std::string interface_name) {
		if (!pCreateInterface_ && !get_create_interface(module_name)) {
			printf("[-] Can't get %s pCreateInterface!\n", module_name.c_str());
			return;
		}

		Interfase* iface = pCreateInterface_;

		while (true) {
			const auto name = iface->name();
			if (!name) continue;

			const auto base = iface->base();
			if (!base) continue;

			printf("[~] %s -> [%p]\n", name.value().c_str(), (void*)base.value());

			if (name.value() == interface_name) {
				const auto base = iface->base();
				if (!base) break;

				printf("[+] Interface %s found -> [%p]\n", interface_name.c_str(), (void*)base.value());
			}

			auto next_ptr = iface->next();
			if (!next_ptr) break;

			iface = next_ptr.value();
		}

		printf("[+] Interface iteration done!\n");
	}

private:
	Interfase* pCreateInterface_ = nullptr;
};
