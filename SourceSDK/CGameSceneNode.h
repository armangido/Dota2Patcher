#pragma once
#include "..\Utils\Memory.h"

class CGameSceneNode {
public:
	Vector origin() const {
		Vector ret{ 0, 0, 0 };
		ret.x = Memory::read_memory<float>(this + 0x10).value_or(-1);
		ret.y = Memory::read_memory<float>(this + 0x14).value_or(-1);
		ret.z = Memory::read_memory<float>(this + 0x18).value_or(-1);
		return ret;
	}
};
