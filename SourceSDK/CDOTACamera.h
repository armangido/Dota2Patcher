#pragma once
#include "../Memory.h"

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

    optional<float> get_distance() {
        const auto distance = Memory::read_memory<float>(this);
        if (!distance)
            return nullopt;
        return distance.value();
    }

    optional<float> get_fow() {
        const auto fow = Memory::read_memory<float>(this + 0x4);
        if (!fow)
            return nullopt;
        return fow.value();
    }

    optional<float> get_r_farz() {
        const auto r_farz = Memory::read_memory<float>(this + 0x14);
        if (!r_farz)
            return nullopt;
        return r_farz.value();
    }
};
