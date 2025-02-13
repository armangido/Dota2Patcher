#pragma once
#include "Utils/Memory.h"

class CDOTACamera {
public:
    void set_distance(int distance) {
        Memory::write_memory(this, static_cast<float>(distance));
    }

    void set_fow(int fow) {
        Memory::write_memory(this + 0x4, static_cast<float>(fow));
    }

    void set_r_farz(int r_farz) {
        Memory::write_memory(this + 0x14, static_cast<float>(r_farz));
    }

    optional <float> get_distance() {
        return Memory::read_memory<float>(this);
    }

    optional <float> get_fow() {
        return Memory::read_memory<float>(this + 0x4);
    }

    optional <float> get_r_farz() {
        return Memory::read_memory<float>(this + 0x14);
    }
};
