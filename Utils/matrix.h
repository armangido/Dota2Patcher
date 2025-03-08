#pragma once

class Matrix {
public:
    float m[4][4];

    Matrix() : m{} {}

    Matrix(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33
    ) {
        m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
        m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
        m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
        m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
    }

    Matrix operator*(float scalar) const { Matrix result; for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j) result.m[i][j] = m[i][j] * scalar; return result; }
    Matrix operator*(const Matrix& other) const { Matrix result; for (int i = 0; i < 4; ++i) { for (int j = 0; j < 4; ++j) { result.m[i][j] = m[i][0] * other.m[0][j] + m[i][1] * other.m[1][j] + m[i][2] * other.m[2][j] + m[i][3] * other.m[3][j]; } } return result; }
    Matrix operator+(const Matrix& other) const { Matrix result; for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j) result.m[i][j] = m[i][j] + other.m[i][j]; return result; }
    Matrix Transpose() const { Matrix result; for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j) result.m[j][i] = m[i][j]; return result; }

    float* operator[](int i) { return m[i]; }
    const float* operator[](int i) const { return m[i]; }
};
