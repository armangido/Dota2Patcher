#pragma once
#include "..\Utils\Memory.h"

class VMatrix { // github.com/LWSS/McDota/blob/master/src/SDK/vmatrix.h
public:

    VMatrix() : m{} {};

    VMatrix(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33
    ) {
        m[0][0] = m00;
        m[0][1] = m01;
        m[0][2] = m02;
        m[0][3] = m03;

        m[1][0] = m10;
        m[1][1] = m11;
        m[1][2] = m12;
        m[1][3] = m13;

        m[2][0] = m20;
        m[2][1] = m21;
        m[2][2] = m22;
        m[2][3] = m23;

        m[3][0] = m30;
        m[3][1] = m31;
        m[3][2] = m32;
        m[3][3] = m33;
    }

    inline float* operator[](int i) {
        return m[i];
    }

    inline const float* operator[](int i) const {
        return m[i];
    }

    inline float* Base() {
        return &m[0][0];
    }

    inline const float* Base() const {
        return &m[0][0];
    }

    float m[4][4];
};

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
        VMatrix matrix = Memory::read_memory<VMatrix>(g_VMatrix).value();

        float x = matrix[0][0] * spot.x + matrix[0][1] * spot.y + matrix[0][2] * spot.z + matrix[0][3];
        float y = matrix[1][0] * spot.x + matrix[1][1] * spot.y + matrix[1][2] * spot.z + matrix[1][3];
        float w = matrix[3][0] * spot.x + matrix[3][1] * spot.y + matrix[3][2] * spot.z + matrix[3][3];

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
