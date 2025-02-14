#pragma once

#define CLASS_DESCRIPTION_CONTAINERS_ARRAY_OFFSET 0x580
#define CLASS_DESCRIPTION_CONTAINERS_ARRAY_SIZE 0x28
#define CLASS_DESCRIPTION_CONTAINERS_ARRAY_MAX_INDEX 256
#define CLASS_DESCRIPTION_CONTAINER_SIZE 0x20
#define SCHEMA_CLASS_FIELD_DATA_SIZE 0x20

class CSchemaType_Builtin {
public:
	optional <string> type_name() {
		const auto type_name_ptr = Memory::read_memory<uintptr_t>(this + 0x8);
		return !type_name_ptr ? nullopt : Memory::read_string(type_name_ptr.value());
	}
};

class SchemaClassFieldData_t {
public:
	optional <string> netvar_name() {
		const auto netvar_name_ptr = Memory::read_memory<uintptr_t>(this);
		return !netvar_name_ptr ? nullopt : Memory::read_string(netvar_name_ptr.value());
	}

	CSchemaType_Builtin* m_type() {
		return Memory::read_memory<CSchemaType_Builtin*>(this + 0x8).value_or(nullptr);
	}

	optional <int32_t> offset() {
		return Memory::read_memory<int32_t>(this + 0x10);
	}

	bool is_netvar() {
		return Memory::read_memory<int32_t>(this + 0x14).value_or(10) < 10;
	}
};

class ClassDescription;

class SchemaParentInfo {
public:
	ClassDescription* parent() {
		return Memory::read_memory<ClassDescription*>(this + 0x8).value_or(nullptr);
	}
};

class ClassDescription {
public:
	optional <string> class_name() {
		const auto class_name_ptr = Memory::read_memory<uintptr_t>(this + 0x8);
		return !class_name_ptr ? nullopt : Memory::read_string(class_name_ptr.value());
	}

	optional <uint32_t> class_size() {
		return Memory::read_memory<uint32_t>(this + 0x18);
	}

	optional <uint32_t> members_size() { // could be 0
		return Memory::read_memory<uint32_t>(this + 0x1C);
	}

	SchemaClassFieldData_t* members_description(size_t index) { // empty if members_size() == 0, go to parent_info
		const auto schema_class_field_data_base = Memory::read_memory<uintptr_t>(this + 0x28);
		if (!schema_class_field_data_base || schema_class_field_data_base.value_or(0) == 0)
			return nullptr;

		const auto schema_class_field_data = schema_class_field_data_base.value() + SCHEMA_CLASS_FIELD_DATA_SIZE * index ;
		if (!Memory::is_valid_ptr(schema_class_field_data))
			return nullptr;

		return reinterpret_cast<SchemaClassFieldData_t*>(schema_class_field_data);
	}

	SchemaParentInfo* parent_info() {
		return Memory::read_memory<SchemaParentInfo*>(this + 0x38).value_or(nullptr);
	}
};

class ClassDescription_Container {
public:
	ClassDescription* class_description(size_t index) {
		auto class_description_ptr = reinterpret_cast<uintptr_t>(this) + CLASS_DESCRIPTION_CONTAINER_SIZE * index;
		return Memory::is_valid_ptr(class_description_ptr) ? Memory::read_memory<ClassDescription*>(class_description_ptr + 0x10).value_or(nullptr) : nullptr;
	}
};

class CSchemaSystemTypeScope {
public:
	optional <string> scope_name() {
		return Memory::read_string(this + 0x8);
	}

	ClassDescription_Container* class_description_container(size_t index) {
		if (index > CLASS_DESCRIPTION_CONTAINERS_ARRAY_MAX_INDEX)
			return nullptr;

		const auto container_base = (uintptr_t)this + CLASS_DESCRIPTION_CONTAINERS_ARRAY_OFFSET;
		const auto container_ptr = container_base + CLASS_DESCRIPTION_CONTAINERS_ARRAY_SIZE * index;
		if (!Memory::is_valid_ptr(container_ptr))
			return nullptr;

		const auto container = Memory::read_memory<ClassDescription_Container*>(container_ptr + 0x18);
		if (!container || container.value_or(nullptr) == nullptr)
			return nullptr;

		return container.value_or(nullptr);
	}
};

class CSchemaSystem {
public:
	CSchemaSystemTypeScope* type_scope(string scope_name) {
		const auto scopes_list = Memory::read_memory<uintptr_t>(this + 0x190);
		if (!scopes_list)
			return nullptr;

		for (size_t i = 0; i < 18; i++) {
			const auto current_scope = Memory::read_memory<CSchemaSystemTypeScope*>(scopes_list.value() + i * 8);
			if (!current_scope || current_scope.value_or(nullptr) == nullptr)
				continue;

			const auto current_name = current_scope.value()->scope_name();
			if (!current_name || current_name.value_or(scope_name) != scope_name)
				continue;

			return current_scope.value_or(nullptr);
		}

		return nullptr;
	}

	// DUMPER

	std::stringstream iterate_netvars(string class_name, ClassDescription* class_description, bool dump_to_file) {
		std::stringstream dump_content;

		for (size_t i = 0; auto members_description = class_description->members_description(i); ++i) {
			if (!members_description || !members_description->is_netvar())
				break;

			const auto netvar_name = members_description->netvar_name();
			const auto offset = members_description->offset();
			const auto m_type = members_description->m_type()->type_name();

			if (netvar_name && offset.value_or(0) != 0) {
				g_netvars[class_name][netvar_name.value()] = offset.value();
				if (dump_to_file)
					dump_content << netvar_name.value() << " | " << std::hex << offset.value() << " | " << m_type.value_or("unknow") << " | [" << (void*)members_description << "]\n";
			}
		}

		return dump_content;
	}

	void dump_netvars(string scope_name, bool dump_to_file = false) {
		const auto scope = this->type_scope(scope_name);
		if (!scope)
			return;

		if (dump_to_file)
			std::filesystem::create_directories("C:\\netvars\\" + scope_name);

		std::unordered_map<string, std::stringstream> class_dumps;
		std::unordered_set<string> processed_classes;

		size_t container_index = 0;
		while (true) {
			const auto class_description_container = scope->class_description_container(container_index);
			if (!class_description_container)
				break;

			size_t class_description_index = 0;
			auto class_description = class_description_container->class_description(class_description_index);
			while (class_description) {
				const auto class_name = class_description->class_name();
				if (!class_name) {
					class_description_index++;
					class_description = class_description_container->class_description(class_description_index);
					continue;
				}

				if (processed_classes.contains(class_name.value())) {
					class_description_index++;
					class_description = class_description_container->class_description(class_description_index);
					continue;
				}

				processed_classes.insert(class_name.value());

				std::stringstream dump_content = iterate_netvars(class_name.value(), class_description, dump_to_file);
				class_dumps[class_name.value()] = std::move(dump_content);

				if (dump_to_file && class_dumps[class_name.value()].tellp() != std::streampos(0)) {
					cout << "[" << container_index << " / 255] " << class_name.value() << " -> [" << (void*)class_description << "]\n";

					string filename = "C:\\netvars\\" + scope_name + "\\" + class_name.value() + ".txt";
					std::ofstream dump_file(filename);
					dump_file << class_dumps[class_name.value()].rdbuf();
					dump_file.close();
				}

				const auto parent_info = class_description->parent_info();
				if (parent_info)
					class_description = parent_info->parent();
				else {
					class_description_index++;
					class_description = class_description_container->class_description(class_description_index);
				}
			}

			container_index++;
		}
	}

	static inline std::unordered_map<std::string, std::unordered_map<std::string, int32_t>> g_netvars;
};
