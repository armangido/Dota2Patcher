#pragma once

class vector2D {
public:
	float x, y;

	vector2D(void) : x(0.0f), y(0.0f) {}
	vector2D(float X, float Y) { x = X; y = Y; }

	bool IsValid() const { return std::isfinite(x) && std::isfinite(y); }
	bool IsZero(float tolerance = 0.01f) const { return std::abs(x) < tolerance && std::abs(y) < tolerance; }
	void Negate() { x = -x; y = -y; }
	float Length() const { return sqrt(x * x + y * y); }
	float LengthSqr() const { return x * x + y * y;}
	bool IsLengthGreaterThan(float val) const { return LengthSqr() > val * val; }
	bool IsLengthLessThan(float val) const { return LengthSqr() < val * val; }
	void Subtract(const vector2D& other, vector2D& result) const { result.x = x - other.x; result.y = y - other.y; }
	float DistTo(const vector2D& vOther) const { vector2D delta; Subtract(vOther, delta); return delta.Length(); }
	float DistToSqr(const vector2D& vOther) const { vector2D delta; Subtract(vOther, delta); return delta.LengthSqr(); }
	vector2D Normalize() const { vector2D vector; float length = this->Length(); if (length != 0) { vector.x = x / length; vector.y = y / length; } else { vector.x = 1.0f; vector.y = 0.0f; } return vector; }

	float operator[](int i) const { return (&x)[i]; }
	float& operator[](int i) { return (&x)[i]; }
	bool operator==(const vector2D& v) const { return (x == v.x) && (y == v.y); }
	bool operator!=(const vector2D& v) const { return (v.x != x) || (v.y != y); }

	vector2D& operator=(const vector2D& v) { x = v.x; y = v.y; return *this; }
	vector2D& operator+=(const vector2D& v) { x += v.x; y += v.y;return *this; }
	vector2D& operator-=(const vector2D& v) { x -= v.x; y -= v.y; return *this; }
	vector2D& operator*=(const vector2D& v) { x *= v.x; y *= v.y; return *this; }
	vector2D& operator/=(const vector2D& v) { x /= v.x; y /= v.y; return *this; }
	vector2D& operator*=(float s) { x *= s; y *= s; return *this; }
	vector2D& operator/=(float s) { x /= s; y /= s; return *this; }
	vector2D& operator+=(float f) { x += f; y += f; return *this; }
	vector2D& operator-=(float f) { x -= f; y -= f; return *this; }
};

class vector3D {
public:
	float x, y, z;

	vector3D(void) : x(0.0f), y(0.0f), z(0.0f) {}
	vector3D(float X, float Y, float Z) : x(X), y(Y), z(Z) {}

	bool IsValid() const { return std::isfinite(x) && std::isfinite(y) && std::isfinite(z); }
	bool IsZero(float tolerance = 0.01f) const { return std::abs(x) < tolerance && std::abs(y) < tolerance && std::abs(z) < tolerance; }
	void Negate() { x = -x; y = -y; z = -z; }
	float Length() const { return std::sqrt(x * x + y * y + z * z); }
	float LengthSqr() const { return (x * x + y * y + z * z); }
	float Length2D() const { return std::sqrt(x * x + y * y); }
	bool IsLengthGreaterThan(float val) const { return LengthSqr() > val * val; }
	bool IsLengthLessThan(float val) const { return LengthSqr() < val * val; }
	void Subtract(const vector3D& other, vector3D& result) const { result.x = x - other.x; result.y = y - other.y; result.z = z - other.z; }
	float DistTo(const vector3D& vOther) const { vector3D delta; Subtract(vOther, delta); return delta.Length(); }
	float DistToSqr(const vector3D& vOther) const { vector3D delta; Subtract(vOther, delta); return delta.LengthSqr(); }
	vector3D Normalize() const { float len = Length(); return len > 0.0f ? vector3D(x / len, y / len, z / len) : vector3D(1.0f, 0.0f, 0.0f); }

	float operator[](int i) const { return (&x)[i]; }
	float& operator[](int i) { return (&x)[i]; }
	bool operator==(const vector3D& v) const { return (x == v.x) && (y == v.y) && (z == v.z); }
	bool operator!=(const vector3D& v) const { return (v.x != x) || (v.y != y) || (v.z != z); }

	vector3D& operator=(const vector3D& v) { x = v.x; y = v.y; z = v.z; return *this; }
	vector3D& operator+=(const vector3D& v) { x += v.x; y += v.y; z += v.z; return *this; }
	vector3D& operator-=(const vector3D& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
	vector3D& operator*=(const vector3D& v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
	vector3D& operator/=(const vector3D& v) { x /= v.x; y /= v.y; z /= v.z; return *this; }
	vector3D& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }
	vector3D& operator/=(float s) { x /= s; y /= s; z /= s; return *this; }
	vector3D& operator+=(float f) { x += f; y += f; z += f; return *this; }
	vector3D& operator-=(float f) { x -= f; y -= f; z -= f; return *this; }
};
