#pragma once

//===============================================
template<typename T>
constexpr auto CHECK_VALID(T _v) { return 0; }
//===============================================
template<typename T>
constexpr auto Assert(T _exp) { return ((void)0); }
//===============================================
template<typename T>
constexpr auto FastSqrt(T x) { return (sqrt)(x); }
//===============================================
inline unsigned long& FloatBits(float& f) {
	return *reinterpret_cast<unsigned long*>(&f);
}
//===============================================
inline bool IsFinite(float f) {
	return ((FloatBits(f) & 0x7F800000) != 0x7F800000);
}
//===============================================

class vector2D {
public:
	float x, y;

	vector2D(void);
	vector2D(float X, float Y);
	vector2D(const float* pFloat);

	void Init(float ix = 0.0f, float iy = 0.0f);
	float* Base();
	float const* Base() const;
	bool IsValid() const;
	bool IsZero(float tolerance = 0.01f) const;
	void Negate();
	
	float Length() const;
	float LengthSqr(void) const;
	
	bool IsLengthGreaterThan(float val) const;
	bool IsLengthLessThan(float val) const;
	float DistTo(const vector2D& vOther) const;
	float DistToSqr(const vector2D& vOther) const;
	vector2D Normalize() const;

	//===============================================

	bool operator==(const vector2D& v) const;
	bool operator!=(const vector2D& v) const;

	vector2D& operator+=(const vector2D& v);
	vector2D& operator-=(const vector2D& v);
	vector2D& operator*=(const vector2D& v);
	vector2D& operator*=(float s);
	vector2D& operator/=(const vector2D& v);
	vector2D& operator/=(float s);
};

//===============================================
inline float Vector2DLength(const vector2D& v) {
	Assert(v.IsValid());
	return (float)FastSqrt(v.x * v.x + v.y * v.y);
}
//===============================================
inline void Vector2DSubtract(const vector2D& a, const vector2D& b, vector2D& c) {
	Assert(a.IsValid() && b.IsValid());
	c.x = a.x - b.x;
	c.y = a.y - b.y;
}
//===============================================
inline vector2D::vector2D() : x(0.0f), y(0.0f) {}
//===============================================
inline vector2D::vector2D(float X, float Y) {
	x = X; y = Y;
	Assert(IsValid());
}
//===============================================
inline vector2D::vector2D(const float* pFloat) {
	Assert(pFloat);
	x = pFloat[0]; y = pFloat[1];
	Assert(IsValid());
}
//===============================================
inline void vector2D::Init(float ix, float iy) {
	x = ix; y = iy;
	Assert(IsValid());
}
//===============================================
inline float* vector2D::Base() {
	return (float*)this;
}
//===============================================
inline float const* vector2D::Base() const {
	return (float const*)this;
}
//===============================================
inline bool vector2D::IsValid() const {
	return IsFinite(x) && IsFinite(y);
}
//===============================================
inline bool vector2D::IsZero(float tolerance) const {
	return (x > -tolerance && x < tolerance && y > -tolerance && y < tolerance);
}
//===============================================
inline void vector2D::Negate() {
	Assert(IsValid());
	x = -x; y = -y;
}
//===============================================
inline float vector2D::Length(void) const {
	return Vector2DLength(*this);
}
//===============================================
inline float vector2D::LengthSqr(void) const {
	Assert(IsValid());
	return (x * x + y * y);
}
//===============================================
inline bool vector2D::IsLengthGreaterThan(float val) const {
	return LengthSqr() > val * val;
}
//===============================================
inline bool vector2D::IsLengthLessThan(float val) const {
	return LengthSqr() < val * val;
}
//===============================================
inline float vector2D::DistTo(const vector2D& vOther) const {
	vector2D delta;
	Vector2DSubtract(*this, vOther, delta);
	return delta.Length();
}
//===============================================
inline float vector2D::DistToSqr(const vector2D& vOther) const {
	vector2D delta;
	Vector2DSubtract(*this, vOther, delta);
	return delta.LengthSqr();
}
//===============================================
inline vector2D vector2D::Normalize() const {
	vector2D vector;
	float length = this->Length();

	if (length != 0) {
		vector.x = x / length;
		vector.y = y / length;
	}
	else {
		vector.x = 1.0f;
		vector.y = 0.0f;
	}

	return vector;
}

//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------
inline bool vector2D::operator==(const vector2D& src) const {
	Assert(src.IsValid() && IsValid());
	return (src.x == x) && (src.y == y);
}
//===============================================
inline bool vector2D::operator!=(const vector2D& src) const {
	Assert(src.IsValid() && IsValid());
	return (src.x != x) || (src.y != y);
}
//===============================================
inline vector2D& vector2D::operator+=(const vector2D& v) {
	Assert(IsValid() && v.IsValid());
	x += v.x; y += v.y;
	return *this;
}
//===============================================
inline vector2D& vector2D::operator-=(const vector2D& v) {
	Assert(IsValid() && v.IsValid());
	x -= v.x; y -= v.y;
	return *this;
}
//===============================================
inline vector2D& vector2D::operator*=(float fl) {
	x *= fl;
	y *= fl;
	Assert(IsValid());
	return *this;
}
//===============================================
inline vector2D& vector2D::operator*=(const vector2D& v) {
	x *= v.x;
	y *= v.y;
	Assert(IsValid());
	return *this;
}
//===============================================
inline vector2D& vector2D::operator/=(float fl) {
	Assert(fl != 0.0f);
	float oofl = 1.0f / fl;
	x *= oofl;
	y *= oofl;
	Assert(IsValid());
	return *this;
}
//===============================================
inline vector2D& vector2D::operator/=(const vector2D& v) {
	Assert(v.x != 0.0f && v.y != 0.0f);
	x /= v.x;
	y /= v.y;
	Assert(IsValid());
	return *this;
}
//===============================================

class vector3D {
public:
	float x, y, z;

	vector3D(void);
	vector3D(float X, float Y, float Z);

	void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f);
	void Zero();

	float* Base();
	float const* Base() const;
	bool IsValid() const;
	bool IsZero(float tolerance = 0.01f) const;

	float Length() const;
	float LengthSqr(void) const;
	float Length2D(void) const;
	float DistTo(const vector3D& vOther) const;
	float DistToSqr(const vector3D& vOther) const;
	vector3D Normalize() const;

	//===============================================

	float operator[](int i) const;
	float& operator[](int i);

	bool operator==(const vector3D& v) const;
	bool operator!=(const vector3D& v) const;

	vector3D& operator=(const vector3D& vOther);
	vector3D& operator+=(const vector3D& v);
	vector3D& operator-=(const vector3D& v);
	vector3D& operator*=(const vector3D& v);
	vector3D& operator*=(float s);
	vector3D& operator/=(const vector3D& v);
	vector3D& operator/=(float s);
	vector3D& operator+=(float fl);
	vector3D& operator-=(float fl);
};

//===============================================
inline vector3D::vector3D() : x(0.0f), y(0.0f), z(0.0f) {}
//===============================================
inline vector3D::vector3D(float X, float Y, float Z) {
	x = X; y = Y; z = Z;
	CHECK_VALID(*this);
}
//===============================================
inline void vector3D::Init(float ix, float iy, float iz) {
	x = ix; y = iy; z = iz;
	CHECK_VALID(*this);
}
//===============================================
inline void vector3D::Zero() {
	x = y = z = 0.0f;
}
//===============================================
inline float* vector3D::Base() {
	return (float*)this;
}
//===============================================
inline float const* vector3D::Base() const {
	return (float const*)this;
}
//===============================================
inline bool vector3D::IsValid() const {
	return IsFinite(x) && IsFinite(y);
}
//===============================================
inline bool vector3D::IsZero(float tolerance) const {
	return (x > -tolerance && x < tolerance && y > -tolerance && y < tolerance && z > -tolerance && z < tolerance);
}
//===============================================
inline float vector3D::Length(void) const {
	CHECK_VALID(*this);
	float root = 0.0f;
	float sqsr = x * x + y * y + z * z;
	root = sqrt(sqsr);
	return root;
}
//===============================================
inline float vector3D::LengthSqr(void) const {
	CHECK_VALID(*this);
	return (x * x + y * y + z * z);
}
//===============================================
inline float vector3D::Length2D(void) const {
	CHECK_VALID(*this);
	float root = 0.0f;
	float sqst = x * x + y * y;
	root = sqrt(sqst);
	return root;
}
//===============================================
float vector3D::DistTo(const vector3D& vOther) const {
	vector3D delta;
	delta.x = x - vOther.x;
	delta.y = y - vOther.y;
	delta.z = z - vOther.z;
	return delta.Length();
}
//===============================================
float vector3D::DistToSqr(const vector3D& vOther) const {
	vector3D delta;
	delta.x = x - vOther.x;
	delta.y = y - vOther.y;
	delta.z = z - vOther.z;
	return delta.LengthSqr();
}
//===============================================
inline vector3D vector3D::Normalize() const {
	vector3D vector;
	float length = this->Length();

	if (length != 0) {
		vector.x = x / length;
		vector.y = y / length;
		vector.z = z / length;
	}
	else
		vector.x = vector.y = 0.0f; vector.z = 1.0f;

	return vector;
}

//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------
inline float vector3D::operator[](int i) const {
	Assert((i >= 0) && (i < 3));
	return ((float*)this)[i];
}
//===============================================
inline float& vector3D::operator[](int i) {
	Assert((i >= 0) && (i < 3));
	return ((float*)this)[i];
}
//===============================================
inline bool vector3D::operator==(const vector3D& src) const {
	CHECK_VALID(src);
	CHECK_VALID(*this);
	return (src.x == x) && (src.y == y) && (src.z == z);
}
//===============================================
inline bool vector3D::operator!=(const vector3D& src) const {
	CHECK_VALID(src);
	CHECK_VALID(*this);
	return (src.x != x) || (src.y != y) || (src.z != z);
}
//===============================================
inline vector3D& vector3D::operator=(const vector3D& vOther) {
	CHECK_VALID(vOther);
	x = vOther.x; y = vOther.y; z = vOther.z;
	return *this;
}
//===============================================
inline vector3D& vector3D::operator+=(const vector3D& v) {
	CHECK_VALID(*this);
	CHECK_VALID(v);
	x += v.x; y += v.y; z += v.z;
	return *this;
}
//===============================================
inline vector3D& vector3D::operator-=(const vector3D& v) {
	CHECK_VALID(*this);
	CHECK_VALID(v);
	x -= v.x; y -= v.y; z -= v.z;
	return *this;
}
//===============================================
inline vector3D& vector3D::operator*=(float fl) {
	x *= fl;
	y *= fl;
	z *= fl;
	CHECK_VALID(*this);
	return *this;
}
//===============================================
inline vector3D& vector3D::operator*=(const vector3D& v) {
	CHECK_VALID(v);
	x *= v.x;
	y *= v.y;
	z *= v.z;
	CHECK_VALID(*this);
	return *this;
}
//===============================================
inline vector3D& vector3D::operator+=(float fl) {
	x += fl;
	y += fl;
	z += fl;
	CHECK_VALID(*this);
	return *this;
}
//===============================================
inline vector3D& vector3D::operator-=(float fl) {
	x -= fl;
	y -= fl;
	z -= fl;
	CHECK_VALID(*this);
	return *this;
}
//===============================================
inline vector3D& vector3D::operator/=(float fl) {
	Assert(fl != 0.0f);
	float oofl = 1.0f / fl;
	x *= oofl;
	y *= oofl;
	z *= oofl;
	CHECK_VALID(*this);
	return *this;
}
//===============================================
inline vector3D& vector3D::operator/=(const vector3D& v) {
	CHECK_VALID(v);
	Assert(v.x != 0.0f && v.y != 0.0f && v.z != 0.0f);
	x /= v.x;
	y /= v.y;
	z /= v.z;
	CHECK_VALID(*this);
	return *this;
}
