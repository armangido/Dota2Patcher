#pragma once
#include "../Memory.h"

class CEngineClient {
public:
	std::optional<bool> in_game() {
		const auto in_game_ptr = Memory::read_memory<bool>(this + 0x95);
		if (!in_game_ptr)
			return std::nullopt;

		return in_game_ptr.value();
	}
};
