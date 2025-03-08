#pragma once
#include "..\Utils\Memory.h"

class WorldToScreen {
public:
    static bool get_windows_size() {
        HWND hwnd = FindWindowW(0, L"Dota 2");
        RECT rect;

        if (!GetClientRect(hwnd, &rect))
            return false;

        g_windows_size.x = static_cast<float>(rect.right - rect.left);
        g_windows_size.y = static_cast<float>(rect.bottom - rect.top);
        
        return true;
    }

    static optional <vector2D> get(const vector3D& spot) {
        matrix3x4 matrix = Memory::read_memory<matrix3x4>(g_VMatrix).value();

        float x = matrix[0][0] * spot.x + matrix[0][1] * spot.y + matrix[0][2] * spot.z + matrix[0][3];
        float y = matrix[1][0] * spot.x + matrix[1][1] * spot.y + matrix[1][2] * spot.z + matrix[1][3];
        float w = matrix[2][0] * spot.x + matrix[2][1] * spot.y + matrix[2][2] * spot.z + matrix[2][3];

        float invw = 1.0f / w;
        x *= invw;
        y *= invw;

        if (x < -1 || x > 1 || y < -1 || y > 1)
            return nullopt;

        vector2D ret = { (x + 1.0f) * 0.5f * g_windows_size.x , (1.0f - y) * 0.5f * g_windows_size.y };
        return ret;
    }

    static inline vector2D g_windows_size;
    static inline uintptr_t g_VMatrix;
};
