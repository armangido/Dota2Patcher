#pragma once

class Vector {
public:
    float x, y, z;

    Vector(float X = 0.0f, float Y = 0.0f, float Z = 0.0f) : x(X), y(Y), z(Z) {}

    bool IsValid() const { return std::isfinite(x) && std::isfinite(y) && std::isfinite(z); }
    bool IsZero(float tolerance = 0.01f) const { return (std::abs(x) < tolerance && std::abs(y) < tolerance && std::abs(z) < tolerance); }
    void Negate() { x = -x; y = -y; z = -z; }
    float Length() const { return std::sqrt(x * x + y * y + z * z); }
    float LengthSqr() const { return (x * x + y * y + z * z); }
    float Length2D() const { return std::sqrt(x * x + y * y); }
    bool IsLengthGreaterThan(float val) const { return LengthSqr() > val * val; }
    bool IsLengthLessThan(float val) const { return LengthSqr() < val * val; }
    void Subtract(const Vector& other, Vector& result) const { result.x = x - other.x; result.y = y - other.y; result.z = z - other.z; }
    float DistTo(const Vector& vOther) const { Vector delta; Subtract(vOther, delta); return delta.Length(); }
    float DistToSqr(const Vector& vOther) const { Vector delta; Subtract(vOther, delta); return delta.LengthSqr(); }
    Vector Normalize() const { float len = Length(); if (len > 0.0f) return Vector(x / len, y / len, z / len); return Vector(1.0f, 0.0f, 0.0f); }

    float operator[](int i) const { return (&x)[i]; }
    float& operator[](int i) { return (&x)[i]; }
    bool operator==(const Vector& v) const { return (x == v.x) && (y == v.y) && (z == v.z); }
    bool operator!=(const Vector& v) const { return (v.x != x) || (v.y != y) || (v.z != z); }

    Vector& operator=(const Vector& v) { x = v.x; y = v.y; z = v.z; return *this; }
    Vector& operator+=(const Vector& v) { x += v.x; y += v.y; z += v.z; return *this; }
    Vector& operator-=(const Vector& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    Vector& operator*=(const Vector& v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
    Vector& operator/=(const Vector& v) { x /= v.x; y /= v.y; z /= v.z; return *this; }
    Vector& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }
    Vector& operator/=(float s) { x /= s; y /= s; z /= s; return *this; }
    Vector& operator+=(float f) { x += f; y += f; z += f; return *this; }
    Vector& operator-=(float f) { x -= f; y -= f; z -= f; return *this; }
};
