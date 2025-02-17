#pragma once

constexpr int HANDLE_INDEX_MASK = 0x1FFF; // 13 bit index
constexpr int HANDLE_SERIAL_SHIFT = 13;   // 14+ bit == serial number

class CHandle {
public:
    explicit CHandle(uint32_t handle = 0) : m_handle(handle) {}

    int to_index() const {
        return m_handle & HANDLE_INDEX_MASK;
    }

    int to_serial() const {
        return m_handle >> HANDLE_SERIAL_SHIFT;
    }

    uint32_t get() const {
        return m_handle;
    }

    bool operator==(const CHandle& other) const {
        return m_handle == other.m_handle;
    }

    bool operator!=(const CHandle& other) const {
        return m_handle != other.m_handle;
    }

private:
    uint32_t m_handle;
};
