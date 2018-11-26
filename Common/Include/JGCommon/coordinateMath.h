#pragma once
#include"common.h"
class jgVec2;
class jgVec3;
class jgVec4;
class jgMatrix2x2;
class jgMatrix3x3;
class jgMatrix4x4;
//
class jgVec2
{
public:
	real x, y;
public: // 생성자 및 소멸자
	jgVec2(const real x = 0, const real y = 0);
	jgVec2(const jgVec2& p1, const jgVec2& p2);
	jgVec2(const jgVec2& copy);
	~jgVec2();
public:
	void set(const real x, const real y) noexcept;
	void set(const jgVec2& p1, const jgVec2& p2) noexcept;
public: //
	real length() const noexcept;
	real squardLength() const noexcept;
	real distance(const jgVec2& vec) const noexcept;
	real squardDistance(const jgVec2& vec) const noexcept;
	real dot(const jgVec2& vec) const noexcept;
	void clear() noexcept;
	void normalize() noexcept;
public: // 비교함수
	bool is_zero(const real errorRange = 0) const noexcept;
	bool is_equal(const jgVec2& vec, const real errorRange = 0) const noexcept;
public:
	jgVec2 & operator=(const jgVec2& vec) noexcept;
	jgVec2  operator+(const jgVec2& vec) const noexcept;
	jgVec2  operator-(const jgVec2& vec) const noexcept;
	jgVec2  operator*(const jgVec2& vec) const noexcept;
	jgVec2  operator*(const real scalar) const noexcept;
	jgVec2  operator/(const real scalar) const noexcept;
	friend jgVec2 operator*(const real scalr, const jgVec2& vec) noexcept;
public:
	jgVec2 & operator+=(const jgVec2& vec) noexcept;
	jgVec2& operator-=(const jgVec2& vec) noexcept;
	jgVec2& operator*=(const jgVec2& vec) noexcept;
	jgVec2& operator*=(const real scalar) noexcept;
	jgVec2& operator/=(const real scalar) noexcept;
public:
	bool operator==(const jgVec2& vec) noexcept;
	bool operator!=(const jgVec2& vec) noexcept;
public:
	void print_cpp() const noexcept;
};
jgVec2 operator*(const real scalar, const jgVec2& vec) noexcept;
//
class jgVec3
{
public:
	real x, y, z;
public:  // 생성자 및 소멸자
	jgVec3(const real x = 0, const real y = 0, const real z = 0);
	jgVec3(const jgVec2& vec, const real z = 0);
	jgVec3(const real x, const jgVec2& vec);
	jgVec3(const jgVec3& p1, const jgVec3& p2);
	jgVec3(const jgVec3& copy);
	~jgVec3();
public:  // 접근자
	void set(const real x, const real y, const real z) noexcept;
	void set(const jgVec3& p1, const jgVec3& p2) noexcept;
public:  // 
	real length() const noexcept;
	real squardLength() const noexcept;
	real distance(const jgVec3& vec) const noexcept;
	real squardDistance(const jgVec3& vec) const noexcept;
	real dot(const jgVec3& vec) const noexcept;
	void clear() noexcept;
	void normalize() noexcept;
	jgVec3 cross(const jgVec3& vec) const noexcept;
public:// 비교 함수
	bool is_zero(const real errorRange = 0) const noexcept;
	bool is_equal(const jgVec3& vec, const real errorRange = 0) const noexcept;
public: // 연산자
	jgVec3 & operator=(const jgVec3& vec) noexcept;
	jgVec3  operator+(const jgVec3& vec) const noexcept;
	jgVec3  operator-(const jgVec3& vec) const noexcept;
	jgVec3  operator*(const jgVec3& vec) const noexcept;
	jgVec3  operator*(const real scalar) const noexcept;
	jgVec3  operator/(const real scalar) const noexcept;
	friend 	jgVec3 operator*(const real scalar, const jgVec3& vec) noexcept;
public:
	jgVec3 & operator+=(const jgVec3& vec) noexcept;
	jgVec3& operator-=(const jgVec3& vec) noexcept;
	jgVec3& operator*=(const jgVec3& vec) noexcept;
	jgVec3& operator*=(const real scalar) noexcept;
	jgVec3& operator/=(const real scalar) noexcept;
public:
	bool operator==(const jgVec3& vec) noexcept;
	bool operator!=(const jgVec3& vec) noexcept;
public:
	void print_cpp() const noexcept;
};
jgVec3 operator*(const real scalar, const jgVec3& vec) noexcept;
//
class jgVec4
{
public:
	real x, y, z, w;
public:
	jgVec4(const real x = 0, const real y = 0, const real z = 0, const real w = 0);
	jgVec4(const jgVec3& vec, const real w = 0);
	jgVec4(const real x, const jgVec3& vec);
	jgVec4(const jgVec2& vec1, const jgVec2& vec2);
	jgVec4(const jgVec2& vec, const real z = 0, const real w = 0);
	jgVec4(const real x, const real y, const jgVec2& vec);
	jgVec4(const jgVec4& copy);
	~jgVec4();
public:
	void set(const real x, const real y, const real z, const real w) noexcept;
public:
	bool is_zero(const real errorRange = 0) const noexcept;
	bool is_equal(const jgVec4& vec, const real errorRange = 0) const noexcept;
public:
	jgVec4 & operator=(const jgVec4& vec) noexcept;
	jgVec4  operator+(const jgVec4& vec) const noexcept;
	jgVec4  operator-(const jgVec4& vec) const noexcept;
	jgVec4  operator*(const jgVec4& vec) const noexcept;
	jgVec4  operator*(const real scalar) const noexcept;
	jgVec4  operator/(const real scalar) const noexcept;
	friend 	jgVec4 operator*(const real scalar, const jgVec4& vec) noexcept;
public:
	jgVec4 & operator+=(const jgVec4& vec) noexcept;
	jgVec4& operator-=(const jgVec4& vec) noexcept;
	jgVec4& operator*=(const jgVec4& vec) noexcept;
	jgVec4& operator*=(const real scalar) noexcept;
	jgVec4& operator/=(const real scalar) noexcept;
public:
	bool operator==(const jgVec4& vec) noexcept;
	bool operator!=(const jgVec4& vec) noexcept;
public:
	void print_cpp() const noexcept;
};
jgVec4 operator*(const real scalar, const jgVec4& vec) noexcept;
//
class jgMatrix2x2
{
private:
	real mat[2][2];
public:
	jgMatrix2x2(const real _11 = 0, const real _12 = 0, const real _21 = 0, const real _22 = 0);
	jgMatrix2x2(const jgVec2& rowVec1, const jgVec2& rowVec2);
	jgMatrix2x2(const jgMatrix2x2& copy);
public:
	void identity() noexcept;
	void transpose() noexcept;
	void transpose(jgMatrix2x2* output) noexcept;
	void inverse() noexcept;
	void inverse(jgMatrix2x2* output) noexcept;
	void zero_standard(const real errorRange = ERRORRANGE) noexcept;
	void clear() noexcept;
	real det() noexcept;
	jgVec2 mulVector(const jgVec2& vec) noexcept;
public:
	real & operator[](const uint idx);
	real & operator()(const uint row, const uint col) noexcept;
public:
	jgMatrix2x2 & operator=(const jgMatrix2x2& m) noexcept;
	jgMatrix2x2  operator+(const jgMatrix2x2& m) const noexcept;
	jgMatrix2x2  operator-(const jgMatrix2x2& m) const noexcept;
	jgMatrix2x2  operator*(const jgMatrix2x2& m) const noexcept;
	jgMatrix2x2  operator*(const real scalar) const noexcept;
	friend 	jgMatrix2x2 operator*(const real scalar, const jgMatrix2x2& m) noexcept;
public:
	jgMatrix2x2 & operator+=(const jgMatrix2x2& m) noexcept;
	jgMatrix2x2& operator-=(const jgMatrix2x2& m) noexcept;
	jgMatrix2x2& operator*=(const jgMatrix2x2& m) noexcept;
	jgMatrix2x2& operator*=(const real scalar) noexcept;
public:
	void print_cpp() const noexcept;
};
jgMatrix2x2 operator*(const real scalar, const jgMatrix2x2& m) noexcept;
class jgMatrix3x3
{
private:
	real mat[3][3];
public:
	jgMatrix3x3(const real _11 = 0, const real _12 = 0, const real _13 = 0,
		const real _21 = 0, const real _22 = 0, const real _23 = 0,
		const real _31 = 0, const real _32 = 0, const real _33 = 0);
	jgMatrix3x3(const jgVec3& rowVec1, const jgVec3& rowVec2, const jgVec4& rowVec3);
	jgMatrix3x3(const jgMatrix3x3& copy);
	~jgMatrix3x3();
public:
	void identity() noexcept;
	void transpose() noexcept;
	void transpose(jgMatrix3x3* output) noexcept;
	void cofactor() noexcept;
	void cofactor(jgMatrix3x3* output) noexcept;
	void adjoint() noexcept;
	void adjoint(jgMatrix3x3* output) noexcept;
	void inverse() noexcept;
	void inverse(jgMatrix3x3* output) noexcept;
	void zero_standard(const real errorRange = ERRORRANGE) noexcept;
	void clear() noexcept;
	real det() noexcept;
	jgVec3 mulVector(const jgVec3& vec) noexcept;
public:
	real& operator[](const uint idx);
	real  cofactor(const uint row, const uint col) noexcept;
	real& operator()(const uint row, const uint col) noexcept;
public:
	jgMatrix3x3 & operator=(const jgMatrix3x3& m) noexcept;
	jgMatrix3x3  operator+(const jgMatrix3x3& m) const noexcept;
	jgMatrix3x3  operator-(const jgMatrix3x3& m) const noexcept;
	jgMatrix3x3  operator*(const jgMatrix3x3& m) const noexcept;
	jgMatrix3x3  operator*(const real scalar) const noexcept;
	friend 	jgMatrix3x3 operator*(const real scalar, const jgMatrix3x3& m) noexcept;
public:
	jgMatrix3x3 & operator+=(const jgMatrix3x3& m) noexcept;
	jgMatrix3x3& operator-=(const jgMatrix3x3& m) noexcept;
	jgMatrix3x3& operator*=(const jgMatrix3x3& m) noexcept;
	jgMatrix3x3& operator*=(const real scalar) noexcept;
public:
	void print_cpp() const noexcept;
};
jgMatrix3x3 operator*(const real scalar, const jgMatrix3x3& m) noexcept;
class jgMatrix4x4
{
private:
	real mat[4][4];
public:
	jgMatrix4x4(const real _11 = 0, const real _12 = 0, const real _13 = 0, const real _14 = 0,
		const real _21 = 0, const real _22 = 0, const real _23 = 0, const real _24 = 0,
		const real _31 = 0, const real _32 = 0, const real _33 = 0, const real _34 = 0,
		const real _41 = 0, const real _42 = 0, const real _43 = 0, const real _44 = 0);
	jgMatrix4x4(const jgVec4& rowVec1, const jgVec4& rowVec2, const jgVec4& rowVec3, const jgVec4& rowVec4);
	jgMatrix4x4(const jgMatrix4x4& copy);
	~jgMatrix4x4();
public:
	void identity() noexcept;
	void transpose() noexcept;
	void transpose(jgMatrix4x4* output) noexcept;
	void cofactor() noexcept;
	void cofactor(jgMatrix4x4* output) noexcept;
	void adjoint() noexcept;
	void adjoint(jgMatrix4x4* output) noexcept;
	void inverse() noexcept;
	void inverse(jgMatrix4x4* output) noexcept;
	void zero_standard(const real errorRange = ERRORRANGE) noexcept;
	void clear() noexcept;
	real det() noexcept;
	jgVec4 mulVector(const jgVec4& vec) noexcept;
	jgVec3 mulVector(const jgVec3& vec) noexcept;
public:
	void translation(const jgVec3& vec) noexcept;
	void translation(const real x, const real y, const real z) noexcept;
	void rotationX(const real roll) noexcept;
	void rotationY(const real pitch) noexcept;
	void rotationZ(const real yaw) noexcept;
	void rotationYawPitchRoll(const real yaw, const real pitch, const real roll) noexcept;
	void scaling(const real x, const real y, const real z) noexcept;
	void scaling(const real n) noexcept;
	void lookAtLH(const jgVec3& eye, const jgVec3& lookAt, const jgVec3& up) noexcept;
	void perspectiveFovLH(const real fov, const real aspect, const real nearZ, const real farZ);
	void orthoLH(const real width, const real height, const real nearZ, const real farZ);
public:
	real& operator[](const uint idx);
	real& operator()(const uint row, const uint col) noexcept;
	real cofactor(const uint row, const uint col) noexcept;
public:
	jgMatrix4x4 & operator=(const jgMatrix4x4& m) noexcept;
	jgMatrix4x4  operator+(const jgMatrix4x4& m) const noexcept;
	jgMatrix4x4  operator-(const jgMatrix4x4& m) const noexcept;
	jgMatrix4x4  operator*(const jgMatrix4x4& m) const noexcept;
	jgMatrix4x4  operator*(const real scalar) noexcept;
	friend jgMatrix4x4 operator*(const real scalar, const jgMatrix4x4& m) noexcept;
public:
	jgMatrix4x4 & operator+=(const jgMatrix4x4& m) noexcept;
	jgMatrix4x4& operator-=(const jgMatrix4x4& m) noexcept;
	jgMatrix4x4& operator*=(const jgMatrix4x4& m) noexcept;
	jgMatrix4x4& operator*=(const real scalar) noexcept;
public:
	void print_cpp() const noexcept;
};
jgMatrix4x4 operator*(const real scalar, const jgMatrix4x4& m) noexcept;

class jgLine
{
public:
	jgLine();
	~jgLine();
};

class jgPlane
{
public:
	jgVec3 n;
	real   d;
public:
	jgPlane(const jgVec3& normal, const jgVec3& vec);
	jgPlane(const jgVec3& v1, const jgVec3& v2, const jgVec3& v3);
	jgPlane(const jgPlane& copy);
	~jgPlane();
public:
	real distance(const jgVec3& dot);
	real dotAssign(const jgVec3& dot);
public:
	void print_cpp() const noexcept;
};