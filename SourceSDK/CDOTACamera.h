#pragma once
#include "../Utils/Memory.h"

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

    float get_distance() const {
        return Memory::read_memory<float>(this).value_or(0);
    }

    float get_fow() const {
        return Memory::read_memory<float>(this + 0x4).value_or(0);
    }

    float get_r_farz() const {
        return Memory::read_memory<float>(this + 0x14).value_or(0);
    }
};
