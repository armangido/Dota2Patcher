#pragma once

class CGameEntitySystem;

class CGameResourceService {
public:
	CGameEntitySystem* get_entity_system() const {
		const auto ptr = Memory::read_memory<CGameEntitySystem*>(this + 0x58);
		return ptr.value_or(nullptr);
	}
};
