#include"coordinateMath.h"
using namespace JGCommon;
using namespace std;
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////   jgVec2   ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

jgVec2::jgVec2(const real x, const real y)
	: x(x), y(y) {}
jgVec2::jgVec2(const jgVec2& p1, const jgVec2& p2)
	: x(p2.x - p1.x), y(p2.y - p1.y) {}
jgVec2::jgVec2(const jgVec2& copy)
{
	x = copy.x; y = copy.y;
}
jgVec2::~jgVec2() {}
void jgVec2::set(const real x, const real y) noexcept
{
	this->x = x; this->y = y;
}
void jgVec2::set(const jgVec2& p1, const jgVec2& p2) noexcept
{
	x = p2.x - p1.x; y = p2.y - p1.y;
}
real jgVec2::length() const noexcept
{
	return SQRT(x * x + y * y);
}
real jgVec2::squardLength() const noexcept
{
	return x * x + y * y;
}
real jgVec2::distance(const jgVec2& vec) const noexcept
{
	return (*this - vec).length();
}
real jgVec2::squardDistance(const jgVec2& vec) const noexcept
{
	return (*this - vec).squardLength();
}
real jgVec2::dot(const jgVec2& vec) const noexcept
{
	return x * vec.x + y * vec.y;
}
void jgVec2::clear() noexcept
{
	x = 0; y = 0;
}
void jgVec2::normalize() noexcept
{
	if (squardLength() == 0) return;
	real len = length();
	x = x / len;
	y = y / len;
}
bool jgVec2::is_zero(const real errorRange) const noexcept
{
	if (ABS(x) <= errorRange && ABS(y) <= errorRange)
	{
		return true;
	}
	return false;
}
bool jgVec2::is_equal(const jgVec2& vec, const real errorRange) const noexcept
{
	if (ABS(x - vec.x) <= errorRange && ABS(y - vec.y) <= errorRange)
	{
		return true;
	}
	return false;
}
jgVec2& jgVec2::operator=(const jgVec2& vec) noexcept
{
	x = vec.x; y = vec.y;
	return *this;
}
jgVec2  jgVec2::operator+(const jgVec2& vec) const noexcept
{
	return jgVec2(x + vec.x, y + vec.y);
}
jgVec2  jgVec2::operator-(const jgVec2& vec) const noexcept
{
	return jgVec2(x - vec.x, y - vec.y);
}
jgVec2  jgVec2::operator*(const jgVec2& vec) const noexcept
{
	return jgVec2(x*vec.x, y*vec.y);
}
jgVec2  jgVec2::operator*(const real scalar) const noexcept
{
	return jgVec2(x*scalar, y*scalar);
}
jgVec2  jgVec2::operator/(const real scalar) const noexcept
{
	return jgVec2(x/scalar, y/scalar);
}
jgVec2 JGCommon::operator*(const real scalar, const jgVec2& vec) noexcept
{
	return jgVec2(scalar*vec.x, scalar*vec.y);
}
jgVec2& jgVec2::operator+=(const jgVec2& vec) noexcept
{
	x += vec.x; y += vec.y;
	return *this;
}
jgVec2& jgVec2::operator-=(const jgVec2& vec) noexcept
{
	x -= vec.x; y -= vec.y;
	return *this;
}
jgVec2& jgVec2::operator*=(const jgVec2& vec) noexcept
{
	x *= vec.x; y *= vec.y;
	return *this;
}
jgVec2& jgVec2::operator*=(const real scalar) noexcept
{
	x += scalar; y += scalar;
	return *this;
}
jgVec2& jgVec2::operator/=(const real scalar) noexcept
{
	x /= scalar; y /= scalar;
	return *this;
}
bool jgVec2::operator==(const jgVec2& vec) noexcept
{
	if (x == vec.x && y == vec.y)
	{
		return true;
	}
	return false;
}
bool jgVec2::operator!=(const jgVec2& vec) noexcept
{
	return !(this->operator==(vec));
}
void jgVec2::print_cpp() const noexcept
{
	cout << "[ " << x << " , " << y << " ] " << endl;
}
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////   jgVec3   ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
jgVec3::jgVec3(const real x, const real y, const real z)
	: x(x), y(y), z(z) {}
jgVec3::jgVec3(const jgVec2& vec, const real z)
	: x(vec.x), y(vec.y), z(z) {}
jgVec3::jgVec3(const real x, const jgVec2& vec)
	: x(x), y(vec.x), z(vec.y) {}
jgVec3::jgVec3(const jgVec3& p1, const jgVec3& p2)
	: x(p2.x - p1.x), y(p2.y - p1.y), z(p2.z - p1.z) {}
jgVec3::jgVec3(const jgVec3& copy)
{
	x = copy.x;  y = copy.y;  z = copy.z;
}

jgVec3::~jgVec3() {}
void jgVec3::set(const real x, const real y, const real z) noexcept
{
	this->x = x; this->y = y; this->z = z;
}
void jgVec3::set(const jgVec3& p1, const jgVec3& p2) noexcept
{
	x = p2.x - p1.x; y = p2.y - p1.y; z = p2.z - p1.z;
}
real jgVec3::length() const noexcept
{
	return SQRT(x * x + y * y + z * z);
}
real jgVec3::squardLength() const noexcept
{
	return x * x + y * y + z * z;
}
real jgVec3::distance(const jgVec3& vec) const noexcept
{
	return (*this - vec).length();
}
real jgVec3::squardDistance(const jgVec3& vec) const noexcept
{
	return (*this - vec).squardLength();
}
real jgVec3::dot(const jgVec3& vec) const noexcept
{
	return x * vec.x + y * vec.y + z * vec.z;
}
void jgVec3::clear() noexcept
{
	x = 0; y = 0; z = 0;
}
void jgVec3::normalize() noexcept
{
	if (squardLength() == 0) return;
	real len = length();
	x = x / len;
	y = y / len;
	z = z / len;
}
jgVec3 jgVec3::cross(const jgVec3& vec) noexcept
{
	return jgVec3(y * vec.z - vec.y * z,
		          z * vec.x - vec.z * x, 
		          x * vec.y - vec.x * y);
}
bool jgVec3::is_zero(const real errorRange) const noexcept
{
	if ( ABS(x) <= errorRange &&
		 ABS(y) <= errorRange &&
		 ABS(z) <= errorRange )
	{
		return true;
	}
	return false;
}
bool jgVec3::is_equal(const jgVec3& vec, const real errorRange) const noexcept
{
	if (ABS(x - vec.x) <= errorRange && 
		ABS(y - vec.y) <= errorRange &&
		ABS(z - vec.z) <= errorRange)
	{
		return true;
	}
	return false;
}
jgVec3& jgVec3::operator=(const jgVec3& vec) noexcept
{
	x = vec.x; y = vec.y; z = vec.z;
	return *this;
}
jgVec3  jgVec3::operator+(const jgVec3& vec) const noexcept
{
	return jgVec3(x + vec.x, y + vec.y, z + vec.z);
}
jgVec3  jgVec3::operator-(const jgVec3& vec) const noexcept
{
	return jgVec3(x - vec.x, y - vec.y, z - vec.z);
}
jgVec3  jgVec3::operator*(const jgVec3& vec) const noexcept
{
	return jgVec3(x*vec.x, y*vec.y, z*vec.z);
}
jgVec3  jgVec3::operator*(const real scalar) const noexcept
{
	return jgVec3(x*scalar, y*scalar, z*scalar);
}
jgVec3  jgVec3::operator/(const real scalar) const noexcept
{
	return jgVec3(x/scalar, y/scalar, z/scalar);
}
jgVec3 JGCommon::operator*(const real scalar, const jgVec3& vec) noexcept
{
	return jgVec3(scalar*vec.x, scalar*vec.y, scalar*vec.z);
}
jgVec3& jgVec3::operator+=(const jgVec3& vec) noexcept
{
	x += vec.x; y += vec.y; z += vec.z;
	return *this;
}
jgVec3& jgVec3::operator-=(const jgVec3& vec) noexcept
{
	x -= vec.x; y -= vec.y; z -= vec.z;
	return *this;
}
jgVec3& jgVec3::operator*=(const jgVec3& vec) noexcept
{
	x *= vec.x; y *= vec.y; z *= vec.z;
	return *this;
}
jgVec3& jgVec3::operator*=(const real scalar) noexcept
{
	x *= scalar; y *= scalar; z *= scalar;
	return *this;
}
jgVec3& jgVec3::operator/=(const real scalar) noexcept
{
	x /= scalar; y /= scalar; z /= scalar;
	return *this;
}
bool jgVec3::operator==(const jgVec3& vec) noexcept
{
	if (x == vec.x && y == vec.y && z == vec.z)
	{
		return true;
	}
	return false;
}
bool jgVec3::operator!=(const jgVec3& vec) noexcept
{
	return !(this->operator==(vec));
}
void jgVec3::print_cpp() const noexcept
{
	cout << "[ " << x << " , " << y << " , " << z << " ] " << endl;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////   jgVec4   ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
jgVec4::jgVec4(const real x, const real y, const real z, const real w)
	: x(x), y(y), z(z), w(w) {}
jgVec4::jgVec4(const jgVec3& vec, const real w)
	: x(vec.x), y(vec.y), z(vec.z), w(w) {}
jgVec4::jgVec4(const real x, const jgVec3& vec)
	: x(x), y(vec.x), z(vec.y), w(vec.z) {}
jgVec4::jgVec4(const jgVec2& vec1, const jgVec2& vec2)
	: x(vec1.x), y(vec1.y), z(vec2.x), w(vec2.y) {}
jgVec4::jgVec4(const jgVec2& vec, const real z, const real w)
	: x(vec.x), y(vec.y), z(z), w(w) {}
jgVec4::jgVec4(const real x, const real y, const jgVec2& vec)
	: x(x), y(y), z(vec.x), w(vec.y) {}
jgVec4::jgVec4(const jgVec4& copy)
{
	x = copy.x; y = copy.y; z = copy.z; w = copy.w;
}
jgVec4::~jgVec4() {}

void jgVec4::set(const real x, const real y, const real z, const real w) noexcept
{
	this->x = x; this->y = y; this->z = z; this->w = w;
}
bool jgVec4::is_zero(const real errorRange) const noexcept
{
	if (ABS(x) <= errorRange && ABS(y) <= errorRange && ABS(z) <= errorRange && ABS(w) <= errorRange)
	{
		return true;
	}
	return false;
}
bool jgVec4::is_equal(const jgVec4& vec, const real errorRange) const noexcept
{
	if (ABS(x - vec.x) <= errorRange && ABS(y - vec.y) <= errorRange && ABS(z - vec.z) <= errorRange && ABS(w - vec.w) <= errorRange)
	{
		return true;
	}
	return false;
}
jgVec4& jgVec4::operator=(const jgVec4& vec) noexcept
{
	x = vec.x; y = vec.y; z = vec.z; w = vec.w;
	return *this;
}
jgVec4  jgVec4::operator+(const jgVec4& vec) const noexcept
{
	return jgVec4(x + vec.x, y + vec.y, z + vec.z, w + vec.w);
}
jgVec4  jgVec4::operator-(const jgVec4& vec) const noexcept
{
	return jgVec4(x - vec.x, y - vec.y, z - vec.z, w - vec.w);
}
jgVec4  jgVec4::operator*(const jgVec4& vec) const noexcept
{
	return jgVec4(x*vec.x, y*vec.y, z*vec.z, w*vec.w);
}
jgVec4  jgVec4::operator*(const real scalar) const noexcept
{
	return jgVec4(x*scalar, y*scalar, z*scalar, w*scalar);
}
jgVec4  jgVec4::operator/(const real scalar) const noexcept
{
	return jgVec4(x/scalar, y/scalar, z/scalar, w/scalar);
}
jgVec4 JGCommon::operator*(const real scalar, const jgVec4& vec) noexcept
{
	return jgVec4(scalar*vec.x, scalar*vec.y, scalar*vec.z, scalar*vec.w);
}
jgVec4& jgVec4::operator+=(const jgVec4& vec) noexcept
{
	x += vec.x; y += vec.y; z += vec.z; w += vec.w;
	return *this;
}
jgVec4& jgVec4::operator-=(const jgVec4& vec) noexcept
{
	x -= vec.x; y -= vec.y; z -= vec.z; w -= vec.w;
	return *this;
}
jgVec4& jgVec4::operator*=(const jgVec4& vec) noexcept
{
	x *= vec.x; y *= vec.y; z *= vec.z; w *= vec.w;
	return *this;
}
jgVec4& jgVec4::operator*=(const real scalar) noexcept
{
	x *= scalar; y *= scalar; z *= scalar; w *= scalar;
	return *this;
}
jgVec4& jgVec4::operator/=(const real scalar) noexcept
{
	x /= scalar; y /= scalar; z /= scalar; w /= scalar;
	return *this;
}
bool jgVec4::operator==(const jgVec4& vec) noexcept
{
	if (x == vec.x && y == vec.y && z == vec.z && w == vec.w)
	{
		return true;
	}
	return false;
}
bool jgVec4::operator!=(const jgVec4& vec) noexcept
{
	return !(this->operator==(vec));
}
void jgVec4::print_cpp() const noexcept
{
	cout << "[ " << x << " , " << y << " , " << z << " , " << w << " ] " << endl;
}
//////////////////////////////////////////////////////////////////////////////////
///////////////////////////   jgMatrix2x2   //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
jgMatrix2x2::jgMatrix2x2(const real _11, const real _12, const real _21, const real _22)
{
	mat[0][0] = _11; mat[0][1] = _12; mat[1][0] = _21; mat[1][1] = _22;
}
jgMatrix2x2::jgMatrix2x2(const jgVec2& rowVec1, const jgVec2& rowVec2)
{
	mat[0][0] = rowVec1.x; mat[0][1] = rowVec1.y; mat[1][0] = rowVec2.x; mat[1][1] = rowVec2.y;
}
jgMatrix2x2::jgMatrix2x2(const jgMatrix2x2& copy)
{
	for (uint row = 0; row < 2; ++row)
	{
		for (uint col = 0; col < 2; ++col)
		{
			mat[row][col] = copy.mat[row][col];
		}
	}
}
void jgMatrix2x2::identity() noexcept
{
	for (uint row = 0; row < 2; ++row)
	{
		for (uint col = 0; col < 2; ++col)
		{
			(row == col) ?
				mat[row][col] = 1 :
				mat[row][col] = 0;
		}
	}
}
void jgMatrix2x2::transpose() noexcept
{
	real tmp = mat[1][0];
	mat[1][0] = mat[0][1]; mat[0][1] = tmp;

}
void jgMatrix2x2::transpose(jgMatrix2x2* output) noexcept
{
	for (uint row = 0; row < 2; ++row)
	{
		for (uint col = 0; col < 2; ++col)
		{
			output->mat[row][col] = mat[col][row];
		}
	}
}
void jgMatrix2x2::inverse() noexcept
{
	real rdet = det();
	if (rdet == 0) return;

	real tmp = mat[1][1];
	mat[1][1] = mat[0][0]; mat[0][0] = tmp;
	mat[0][1] = -mat[0][1]; mat[1][0] = -mat[1][0];

	this->operator*=(1 / rdet);
}
void jgMatrix2x2::inverse(jgMatrix2x2* output) noexcept
{
	real rdet = det();
	if (rdet == 0) return;

	output->mat[1][1] = mat[0][0];  output->mat[0][0] = mat[1][1];
	output->mat[0][1] = -mat[0][1]; output->mat[1][0] = -mat[1][0];

	output->operator*=(1 / rdet);
}
void jgMatrix2x2::zero_standard(const real errorRange) noexcept
{
	for (uint row = 0; row < 2; ++row)
	{
		for (uint col = 0; col < 2; ++col)
		{
			if (ABS(mat[row][col]) <= errorRange)
			{
				mat[row][col] = 0;
			}
		}
	}
}
void jgMatrix2x2::clear() noexcept
{
	for (uint row = 0; row < 2; ++row)
	{
		for (uint col = 0; col < 2; ++col)
		{
			mat[row][col] = 0;
		}
	}
}
real jgMatrix2x2::det() noexcept
{
	return (mat[0][0] * mat[1][1]) - (mat[0][1] * mat[1][0]);
}
jgVec2 jgMatrix2x2::mulVector(const jgVec2& vec) noexcept
{
	return jgVec2((vec.x * mat[0][0]) + (vec.y * mat[1][0]),
		          (vec.x * mat[0][1]) + (vec.y * mat[1][1]));
}
real& jgMatrix2x2::operator()(const uint row, const uint col) noexcept
{
	assert(row >= 1 || row <= 2); assert(col >= 1 || col <= 2);
	return mat[row - 1][col -1];
}
jgMatrix2x2& jgMatrix2x2::operator=(const jgMatrix2x2& m) noexcept
{
	for (uint row = 0; row < 2; ++row)
	{
		for (uint col = 0; col < 2; ++col)
		{
			mat[row][col] = m.mat[row][col];
		}
	}
	return *this;
}
jgMatrix2x2  jgMatrix2x2::operator+(const jgMatrix2x2& m) const noexcept
{
	jgMatrix2x2 result;
	for (uint row = 0; row < 2; ++row)
	{
		for (uint col = 0; col < 2; ++col)
		{
			result.mat[row][col] = mat[row][col] + m.mat[row][col];
		}
	}
	return result;
}
jgMatrix2x2  jgMatrix2x2::operator-(const jgMatrix2x2& m) const noexcept
{
	jgMatrix2x2 result;
	for (uint row = 0; row < 2; ++row)
	{
		for (uint col = 0; col < 2; ++col)
		{
			result.mat[row][col] = mat[row][col] - m.mat[row][col];
		}
	}
	return result;
}
jgMatrix2x2  jgMatrix2x2::operator*(const jgMatrix2x2& m) const noexcept
{
	jgMatrix2x2 result;
	for (uint row = 0; row < 2; ++row)
	{
		for (uint col = 0; col < 2; ++col)
		{
			result.mat[row][col] = (mat[row][0] * m.mat[0][col]) + (mat[row][1] * m.mat[1][col]);
		}
	}
	return result;
}
jgMatrix2x2  jgMatrix2x2::operator*(const real scalar) const noexcept
{
	jgMatrix2x2 result;
	for (uint row = 0; row < 2; ++row)
	{
		for (uint col = 0; col < 2; ++col)
		{
			result.mat[row][col] = mat[row][col] * scalar;
		}
	}
	return result;
}
jgMatrix2x2 JGCommon::operator*(const real scalar, const jgMatrix2x2& m) noexcept
{
	jgMatrix2x2 result;
	for (uint row = 0; row < 2; ++row)
	{
		for (uint col = 0; col < 2; ++col)
		{
			result.mat[row][col] = m.mat[row][col] * scalar;
		}
	}
	return result;
}
jgMatrix2x2& jgMatrix2x2::operator+=(const jgMatrix2x2& m) noexcept
{
	for (uint row = 0; row < 2; ++row)
	{
		for (uint col = 0; col < 2; ++col)
		{
			mat[row][col] += m.mat[row][col];
		}
	}
	return *this;
}
jgMatrix2x2& jgMatrix2x2::operator-=(const jgMatrix2x2& m) noexcept
{
	for (uint row = 0; row < 2; ++row)
	{
		for (uint col = 0; col < 2; ++col)
		{
			mat[row][col] -= m.mat[row][col];
		}
	}
	return *this;
}
jgMatrix2x2& jgMatrix2x2::operator*=(const jgMatrix2x2& m) noexcept
{
	jgMatrix2x2 result;
	for (uint row = 0; row < 2; ++row)
	{
		for (uint col = 0; col < 2; ++col)
		{
			result.mat[row][col] = (mat[row][0] * m.mat[0][col]) + (mat[row][1] * m.mat[1][col]);
		}
	}
	(*this) = result;
	return *this;
}
jgMatrix2x2& jgMatrix2x2::operator*=(const real scalar) noexcept
{
	for (uint row = 0; row < 2; ++row)
	{
		for (uint col = 0; col < 2; ++col)
		{
			mat[row][col] *= scalar;
		}
	}
	return *this;
}
void jgMatrix2x2::print_cpp() const noexcept
{
	for (uint row = 0; row < 2; ++row)
	{
		cout << " [ ";
		for (uint col = 0; col < 2; ++col)
		{
			cout << mat[row][col];
			(col == 1) ? cout << " ] " << endl : cout << " , ";
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////
///////////////////////////   jgMatrix3x3   //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
jgMatrix3x3::jgMatrix3x3(const real _11, const real _12, const real _13,
	                     const real _21, const real _22, const real _23,
	                     const real _31, const real _32, const real _33)
{
	mat[0][0] = _11; mat[0][1] = _12; mat[0][2] = _13;
	mat[1][0] = _21; mat[1][1] = _22; mat[1][2] = _23;
	mat[2][0] = _31; mat[2][1] = _32; mat[2][2] = _33;
}
jgMatrix3x3::jgMatrix3x3(const jgVec3& rowVec1, const jgVec3& rowVec2, const jgVec4& rowVec3)
{
	mat[0][0] = rowVec1.x; mat[0][1] = rowVec1.y; mat[0][2] = rowVec1.z;
	mat[1][0] = rowVec2.x; mat[1][1] = rowVec2.y; mat[1][2] = rowVec2.z;
	mat[2][0] = rowVec3.x; mat[2][1] = rowVec3.y; mat[2][2] = rowVec3.z;
}
jgMatrix3x3::jgMatrix3x3(const jgMatrix3x3& copy)
{
	for (uint row = 0; row < 3; ++row)
	{
		for (uint col = 0; col < 3; ++col)
		{
			mat[row][col] = copy.mat[row][col];
		}
	}
}
jgMatrix3x3::~jgMatrix3x3() {}
void jgMatrix3x3::identity() noexcept
{
	for (uint row = 0; row < 3; ++row)
	{
		for (uint col = 0; col < 3; ++col)
		{
			(row == col) ?
				mat[row][col] = 1 :
				mat[row][col] = 0;
		}
	}
}
void jgMatrix3x3::transpose() noexcept
{
	real tmp1, tmp2;
	tmp1 = mat[1][0]; tmp2 = mat[2][0];
	mat[1][0] = mat[0][1]; mat[2][0] = mat[0][2];
	mat[0][1] = tmp1;      tmp1 = mat[2][1];       mat[2][1] = mat[1][2];
	mat[0][2] = tmp2;      mat[1][2] = tmp1; 
}
void jgMatrix3x3::transpose(jgMatrix3x3* output) noexcept
{
	for (uint row = 0; row < 3; ++row)
	{
		for (uint col = 0; col < 3; ++col)
		{
			output->mat[row][col] = mat[col][row];
		}
	}
}
void jgMatrix3x3::cofactor() noexcept
{
	jgMatrix3x3 m;
	for (uint row = 0; row < 3; ++row)
	{
		for (uint col = 0; col < 3; ++col)
		{
			m.mat[row][col] = cofactor(row + 1, col + 1);
		}
	}
	(*this) = m;
}
void jgMatrix3x3::cofactor(jgMatrix3x3* output) noexcept
{
	for (uint row = 0; row < 3; ++row)
	{
		for (uint col = 0; col < 3; ++col)
		{
			output->mat[row][col] = cofactor(row + 1, col + 1);
		}
	}
}
void jgMatrix3x3::adjoint() noexcept
{
	cofactor();
	transpose();
}
void jgMatrix3x3::adjoint(jgMatrix3x3* output) noexcept
{
	cofactor(output);
	output->transpose();
}
void jgMatrix3x3::inverse() noexcept
{
	real rdet = det();
	if (rdet == 0) return;

	adjoint();
	this->operator*=(1 / rdet);
}
void jgMatrix3x3::inverse(jgMatrix3x3* output) noexcept
{
	real rdet = det();
	if (rdet == 0) return;

	adjoint(output);
	output->operator*=(1 / rdet);
}
void jgMatrix3x3::zero_standard(const real errorRange) noexcept
{
	for (uint row = 0; row < 3; ++row)
	{
		for (uint col = 0; col < 3; ++col)
		{
			if (ABS(mat[row][col]) <= errorRange)
				mat[row][col] = 0;
		}
	}
}
void jgMatrix3x3::clear() noexcept
{
	for (uint row = 0; row < 3; ++row)
	{
		for (uint col = 0; col < 3; ++col)
		{
			mat[row][col] = 0;
		}
	}
}
real jgMatrix3x3::det() noexcept
{
	return (mat[0][0] * ((mat[1][1] * mat[2][2]) - (mat[1][2] * mat[2][1]))) -
		   (mat[0][1] * ((mat[1][0] * mat[2][2]) - (mat[1][2] * mat[2][0]))) +
		   (mat[0][2] * ((mat[1][0] * mat[2][1]) - (mat[1][1] * mat[2][0])));
}
jgVec3 jgMatrix3x3::mulVector(const jgVec3& vec) noexcept
{
	return jgVec3(
		vec.x * mat[0][0] + vec.y * mat[1][0] + vec.z * mat[2][0],
		vec.x * mat[0][1] + vec.y * mat[1][1] + vec.z * mat[2][1],
		vec.x * mat[0][2] + vec.y * mat[1][2] + vec.z * mat[2][2]);
}
real  jgMatrix3x3::cofactor(const uint row, const uint col) noexcept
{
	assert(row >= 1 || row <= 3); assert(col >= 1 || col <= 3);
	real tmpMat[4];
	uint count = 0;
	for (uint rowi = 0; rowi < 3; ++rowi)
	{
		for (uint colj = 0; colj < 3; ++colj)
		{
			if (rowi != row - 1 && colj != col - 1)
			{
				tmpMat[count] = mat[rowi][colj];
				count++;
			}
		}
	}
	real det = ((tmpMat[0] * tmpMat[3]) - (tmpMat[1] * tmpMat[2]));
	real scalar = 1;
	((row + col) % 2) ?
		scalar = -1 : scalar = 1;
	return scalar * det;
}
real& jgMatrix3x3::operator()(const uint row, const uint col) noexcept
{
	assert(row >= 1 || row <= 3); assert(col >= 1 || col <= 3);
	return mat[row - 1][col - 1];
}
jgMatrix3x3& jgMatrix3x3::operator=(const jgMatrix3x3& m) noexcept
{
	for (uint row = 0; row < 3; ++row)
	{
		for (uint col = 0; col < 3; ++col)
		{
			mat[row][col] = m.mat[row][col];
		}
	}
	return *this;
}
jgMatrix3x3  jgMatrix3x3::operator+(const jgMatrix3x3& m) const noexcept
{
	jgMatrix3x3 result;
	for (uint row = 0; row < 3; ++row)
	{
		for (uint col = 0; col < 3; ++col)
		{
			result.mat[row][col] = mat[row][col] + m.mat[row][col];
		}
	}
	return result;
}
jgMatrix3x3  jgMatrix3x3::operator-(const jgMatrix3x3& m) const noexcept
{
	jgMatrix3x3 result;
	for (uint row = 0; row < 3; ++row)
	{
		for (uint col = 0; col < 3; ++col)
		{
			result.mat[row][col] = mat[row][col] - m.mat[row][col];
		}
	}
	return result;
}
jgMatrix3x3  jgMatrix3x3::operator*(const jgMatrix3x3& m) const noexcept
{
	jgMatrix3x3 result;
	for (uint row = 0; row < 3; ++row)
	{
		for (uint col = 0; col < 3; ++col)
		{
			result.mat[row][col] =
				(mat[row][0] * m.mat[0][col]) +
				(mat[row][1] * m.mat[1][col]) +
				(mat[row][2] * m.mat[2][col]);
		}
	}
	return result;
}
jgMatrix3x3  jgMatrix3x3::operator*(const real scalar) const noexcept
{
	jgMatrix3x3 result;
	for (uint row = 0; row < 3; ++row)
	{
		for (uint col = 0; col < 3; ++col)
		{
			result.mat[row][col] = mat[row][col] * scalar;
		}
	}
	return result;
}
jgMatrix3x3 JGCommon::operator*(const real scalar, const jgMatrix3x3& m) noexcept
{
	jgMatrix3x3 result;
	for (uint row = 0; row < 3; ++row)
	{
		for (uint col = 0; col < 3; ++col)
		{
			result.mat[row][col] = m.mat[row][col] * scalar;
		}
	}
	return result;
}
jgMatrix3x3& jgMatrix3x3::operator+=(const jgMatrix3x3& m) noexcept
{
	for (uint row = 0; row < 3; ++row)
	{
		for (uint col = 0; col < 3; ++col)
		{
			mat[row][col] += m.mat[row][col];
		}
	}
	return *this;
}
jgMatrix3x3& jgMatrix3x3::operator-=(const jgMatrix3x3& m) noexcept
{
	for (uint row = 0; row < 3; ++row)
	{
		for (uint col = 0; col < 3; ++col)
		{
			mat[row][col] -= m.mat[row][col];
		}
	}
	return *this;
}
jgMatrix3x3& jgMatrix3x3::operator*=(const jgMatrix3x3& m) noexcept
{
	jgMatrix3x3 result;
	for (uint row = 0; row < 3; ++row)
	{
		for (uint col = 0; col < 3; ++col)
		{
			result.mat[row][col] =
				(mat[row][0] * m.mat[0][col]) +
				(mat[row][1] * m.mat[1][col]) +
				(mat[row][2] * m.mat[2][col]);
		}
	}
	(*this) = result;
	return *this;
}
jgMatrix3x3& jgMatrix3x3::operator*=(const real scalar) noexcept
{
	for (uint row = 0; row < 3; ++row)
	{
		for (uint col = 0; col < 3; ++col)
		{
			mat[row][col] *= scalar;
		}
	}
	return *this;
}
void jgMatrix3x3::print_cpp() const noexcept
{
	for (uint row = 0; row < 3; ++row)
	{
		cout << " [ ";
		for (uint col = 0; col < 3; ++col)
		{
	
			cout << mat[row][col];
			(col == 2) ? cout << " ] " << endl : cout << " , ";
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////
///////////////////////////   jgMatrix4x4   //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
jgMatrix4x4::jgMatrix4x4(const real _11, const real _12, const real _13, const real _14,
	                     const real _21, const real _22, const real _23, const real _24,
	                     const real _31, const real _32, const real _33, const real _34,
	                     const real _41, const real _42, const real _43, const real _44)
{
	mat[0][0] = _11; mat[0][1] = _12; mat[0][2] = _13; mat[0][3] = _14;
	mat[1][0] = _21; mat[1][1] = _22; mat[1][2] = _23; mat[1][3] = _24;
	mat[2][0] = _31; mat[2][1] = _32; mat[2][2] = _33; mat[2][3] = _34;
	mat[3][0] = _41; mat[3][1] = _42; mat[3][2] = _43; mat[3][3] = _44;
}
jgMatrix4x4::jgMatrix4x4(const jgVec4& rowVec1, const jgVec4& rowVec2, const jgVec4& rowVec3, const jgVec4& rowVec4)
{
	mat[0][0] = rowVec1.x; mat[0][1] = rowVec1.y; mat[0][2] = rowVec1.z; mat[0][3] = rowVec1.w;
	mat[1][0] = rowVec2.x; mat[1][1] = rowVec2.y; mat[1][2] = rowVec2.z; mat[1][3] = rowVec2.w;
	mat[2][0] = rowVec3.x; mat[2][1] = rowVec3.y; mat[2][2] = rowVec3.z; mat[2][3] = rowVec3.w;
	mat[3][0] = rowVec4.x; mat[3][1] = rowVec4.y; mat[3][2] = rowVec4.z; mat[3][3] = rowVec4.w;
}
jgMatrix4x4::jgMatrix4x4(const jgMatrix4x4& copy)
{
	for (uint row = 0; row < 4; ++row)
	{
		for (uint col = 0; col < 4; ++col)
		{
			mat[row][col] = copy.mat[row][col];
		}
	}
}
jgMatrix4x4::~jgMatrix4x4() {}
void jgMatrix4x4::identity() noexcept
{
	for (uint row = 0; row < 4; ++row)
	{
		for (uint col = 0; col < 4; ++col)
		{
			(row == col) ?
				mat[row][col] = 1 :
				mat[row][col] = 0;
		}
	}
}
void jgMatrix4x4::transpose() noexcept
{
	real tmp1, tmp2, tmp3, tmp4;
	tmp1 = mat[1][0]; tmp2 = mat[2][0]; tmp3 = mat[3][0];
	mat[1][0] = mat[0][1]; mat[2][0] = mat[0][2]; mat[3][0] = mat[0][3];

	mat[0][1] = tmp1;      tmp1 = mat[2][1];      tmp4 = mat[3][1];
	mat[2][1] = mat[1][2]; mat[3][1] = mat[1][3];

	mat[0][2] = tmp2;      mat[1][2] = tmp1;      tmp1 = mat[3][2];
	mat[3][2] = mat[2][3];

	mat[0][3] = tmp3;      mat[1][3] = tmp4;      mat[2][3] = tmp1;
}
void jgMatrix4x4::transpose(jgMatrix4x4* output) noexcept
{
	for (uint row = 0; row < 4; ++row)
	{
		for (uint col = 0; col < 4; ++col)
		{
			output->mat[row][col] = mat[col][row];
		}
	}
}
void jgMatrix4x4::cofactor() noexcept
{
	jgMatrix4x4 m;
	for (uint row = 0; row < 4; ++row)
	{
		for (uint col = 0; col < 4; ++col)
		{
			m.mat[row][col] = cofactor(row + 1, col + 1);
		}
	}
	(*this) = m;
}
void jgMatrix4x4::cofactor(jgMatrix4x4* output) noexcept
{
	for (uint row = 0; row < 4; ++row)
	{
		for (uint col = 0; col < 4; ++col)
		{
			output->mat[row][col] = cofactor(row + 1, col + 1);
		}
	}
}
void jgMatrix4x4::adjoint() noexcept
{
	cofactor();
	transpose();
}
void jgMatrix4x4::adjoint(jgMatrix4x4* output) noexcept
{
	cofactor(output);
	output->transpose();
}
void jgMatrix4x4::inverse() noexcept
{
	real rdet = det();
	if (rdet == 0) return;

	adjoint();
	this->operator*=(1 / rdet);
}
void jgMatrix4x4::inverse(jgMatrix4x4* output) noexcept
{
	real rdet = det();
	if (rdet == 0) return;

	adjoint(output);
	output->operator*=(1 / rdet);
}
void jgMatrix4x4::zero_standard(const real errorRange) noexcept
{
	for (uint row = 0; row < 4; ++row)
	{
		for (uint col = 0; col < 4; ++col)
		{
			if (ABS(mat[row][col]) <= errorRange)
				mat[row][col] = 0;
		}
	}
}
void jgMatrix4x4::clear() noexcept
{
	for (uint row = 0; row < 4; ++row)
	{
		for (uint col = 0; col < 4; ++col)
		{
			mat[row][col] = 0;
		}
	}
}
real jgMatrix4x4::det() noexcept
{
	return (mat[0][0] * jgMatrix3x3(mat[1][1], mat[1][2], mat[1][3], mat[2][1], mat[2][2], mat[2][3], mat[3][1], mat[3][2], mat[3][3]).det()) -
		   (mat[0][1] * jgMatrix3x3(mat[1][0], mat[1][2], mat[1][3], mat[2][0], mat[2][2], mat[2][3], mat[3][0], mat[3][2], mat[3][3]).det()) +
		   (mat[0][2] * jgMatrix3x3(mat[1][0], mat[1][1], mat[1][3], mat[2][0], mat[2][1], mat[2][3], mat[3][0], mat[3][1], mat[3][3]).det()) -
		   (mat[0][3] * jgMatrix3x3(mat[1][0], mat[1][1], mat[1][2], mat[2][0], mat[2][1], mat[2][2], mat[3][0], mat[3][1], mat[3][2]).det());
}
jgVec4 jgMatrix4x4::mulVector(const jgVec4& vec) noexcept
{
	return jgVec4(
		vec.x * mat[0][0] + vec.y * mat[1][0] + vec.z * mat[2][0] + vec.w * mat[3][0],
		vec.x * mat[0][1] + vec.y * mat[1][1] + vec.z * mat[2][1] + vec.w * mat[3][1],
		vec.x * mat[0][2] + vec.y * mat[1][2] + vec.z * mat[2][2] + vec.w * mat[3][2],
		vec.x * mat[0][3] + vec.y * mat[1][3] + vec.z * mat[2][3] + vec.w * mat[3][3]);
}
jgVec3 jgMatrix4x4::mulVector(const jgVec3& vec) noexcept
{
	jgVec4 vec4(vec, 1.0f);
	vec4 = mulVector(vec4);
	return jgVec3(vec4.x, vec4.y, vec4.z);
}
void jgMatrix4x4::translation(const jgVec3& vec) noexcept
{
	identity();
	mat[3][0] = vec.x; mat[3][1] = vec.y; mat[3][2] = vec.z;
}
void jgMatrix4x4::translation(const real x, const real y, const real z) noexcept
{
	identity();
	mat[3][0] = x; mat[3][1] = y; mat[3][2] = z;
}
void jgMatrix4x4::rotationX(const real roll) noexcept
{
	identity();
	real rcos = COS(roll);
	real rsin = SIN(roll);
	mat[1][1] = rcos; mat[1][2] = rsin;
	mat[2][1] = -rsin; mat[2][2] = rcos;
}
void jgMatrix4x4::rotationY(const real pitch) noexcept
{
	identity();
	real pcos = COS(pitch);
	real psin = SIN(pitch);
	mat[0][0] = pcos; mat[0][2] = -psin;
	mat[2][0] = psin; mat[2][2] = pcos;
}
void jgMatrix4x4::rotationZ(const real yaw) noexcept
{
	identity();
	real ycos = COS(yaw);
	real ysin = SIN(yaw);
	mat[0][0] = ycos; mat[0][1] = ysin;
	mat[1][0] = -ysin; mat[1][1] = ycos;

}
void jgMatrix4x4::rotationYawPitchRoll(const real yaw, const real pitch, const real roll) noexcept
{
	real cr = COS(roll);  real sr = SIN(roll);
	real cy = COS(yaw);   real sy = SIN(yaw);
	real cp = COS(pitch); real sp = SIN(pitch);
	identity();
	mat[0][0] = (cr * cy) + (sr * sp * sy);  	mat[0][1] = sr * cp;   mat[0][2] = (-cr * sy) + (sr * sp * cy);
	mat[1][0] = (-sr * cy) + (cr * sp * sy); 	mat[1][1] = cr * cp;   mat[1][2] = (sr * sy) + (cr * sp * cy);
	mat[2][0] = cp * sy;                        mat[2][1] = -sp;       mat[2][2] = cp * cy;
}
void jgMatrix4x4::scaling(const real x, const real y, const real z) noexcept
{
	identity();
	mat[0][0] = x; mat[1][1] = y; mat[2][2] = z;
}
void jgMatrix4x4::scaling(const real n) noexcept
{
	identity();
	mat[0][0] = n; mat[1][1] = n; mat[2][2] = n;
}
void jgMatrix4x4::lookAtLH(const jgVec3& eye, const jgVec3& lookAt, const jgVec3& up) noexcept
{
	clear();
	jgVec3 viewDir(lookAt - eye);
	jgVec3 viewSide;
	jgVec3 viewUp;
	viewDir.normalize();
	viewUp = up - up.dot(viewDir) * viewDir;
	viewUp.normalize();
	viewSide = viewUp.cross(viewDir);

	// 행렬 구성
	mat[0][0] = viewSide.x; mat[0][1] = viewSide.y; mat[0][2] = viewSide.z;
	mat[1][0] = viewUp.x;   mat[1][1] = viewUp.y;   mat[1][2] = viewUp.z;
	mat[2][0] = viewDir.x;  mat[2][1] = viewDir.y;  mat[2][2] = viewDir.z;
	mat[3][0] = eye.x;      mat[3][1] = eye.y;      mat[3][2] = eye.z;
	mat[3][3] = 1;
}
void jgMatrix4x4::perspectiveFovLH(const real fov, const real aspect, const real near, const real far)
{
	clear();
	real d = COT(fov / 2);
	mat[0][0] = d / aspect;
	mat[1][1] = d;
	mat[2][2] = far / (far - near); 	mat[2][3] = 1; 
	mat[3][2] = -((near * far) / (far - near));
}
void jgMatrix4x4::orthoLH(const real width, const real height, const real near, const real far)
{
	clear();
	mat[0][0] = 2 / width;
	mat[1][1] = 2 / height;
	mat[2][2] = 1 / (far - near);
	mat[3][2] = near / (near - far); 	mat[3][3] = 1;
}
real& jgMatrix4x4::operator()(const uint row, const uint col) noexcept
{
	assert(row >= 1 || row <= 4); assert(col >= 1 || col <= 4);
	return mat[row - 1][col - 1];
}
real jgMatrix4x4::cofactor(const uint row, const uint col) noexcept
{
	assert(row >= 1 || row <= 4); assert(col >= 1 || col <= 4);

	real tmpMat[9];
	uint count = 0;
	for (uint rowi = 0; rowi < 4; ++rowi)
	{
		for (uint colj = 0; colj < 4; ++colj)
		{
			if (rowi != row - 1 && colj != col - 1)
			{
				tmpMat[count] = mat[rowi][colj];
				count++;
			}
		}
	}
	real det = jgMatrix3x3(tmpMat[0], tmpMat[1], tmpMat[2], tmpMat[3], tmpMat[4], tmpMat[5], tmpMat[6], tmpMat[7], tmpMat[8]).det();
	real scalar = 1;
	((row + col) % 2) ?
		scalar = -1 : scalar = 1;
	return scalar * det;

}
jgMatrix4x4& jgMatrix4x4::operator=(const jgMatrix4x4& m) noexcept
{
	for (uint row = 0; row < 4; ++row)
	{
		for (uint col = 0; col < 4; ++col)
		{
			mat[row][col] = m.mat[row][col];
		}
	}
	return *this;
}
jgMatrix4x4  jgMatrix4x4::operator+(const jgMatrix4x4& m) const noexcept
{
	jgMatrix4x4 result;
	for (uint row = 0; row < 4; ++row)
	{
		for (uint col = 0; col < 4; ++col)
		{
			result.mat[row][col] = mat[row][col] + m.mat[row][col];
		}
	}
	return result;
}
jgMatrix4x4  jgMatrix4x4::operator-(const jgMatrix4x4& m) const noexcept
{
	jgMatrix4x4 result;
	for (uint row = 0; row < 4; ++row)
	{
		for (uint col = 0; col < 4; ++col)
		{
			result.mat[row][col] = mat[row][col] - m.mat[row][col];
		}
	}
	return result;
}
jgMatrix4x4  jgMatrix4x4::operator*(const jgMatrix4x4& m) const noexcept
{
	jgMatrix4x4 result;
	for (uint row = 0; row < 4; ++row)
	{
		for (uint col = 0; col < 4; ++col)
		{
			result.mat[row][col] =
				(mat[row][0] * m.mat[0][col]) +
				(mat[row][1] * m.mat[1][col]) +
				(mat[row][2] * m.mat[2][col]) +
				(mat[row][3] * m.mat[3][col]);
		}
	}
	return result;
}
jgMatrix4x4  jgMatrix4x4::operator*(const real scalar) noexcept
{
	jgMatrix4x4 result;
	for (uint row = 0; row < 4; ++row)
	{
		for (uint col = 0; col < 4; ++col)
		{
			result.mat[row][col] = mat[row][col] * scalar;
		}
	}
	return result;
}
jgMatrix4x4 JGCommon::operator*(const real scalar, const jgMatrix4x4& m) noexcept
{
	jgMatrix4x4 result;
	for (uint row = 0; row < 4; ++row)
	{
		for (uint col = 0; col < 4; ++col)
		{
			result.mat[row][col] = m.mat[row][col] * scalar;
		}
	}
	return result;
}
jgMatrix4x4& jgMatrix4x4::operator+=(const jgMatrix4x4& m) noexcept
{
	for (uint row = 0; row < 4; ++row)
	{
		for (uint col = 0; col < 4; ++col)
		{
			mat[row][col] += m.mat[row][col];
		}
	}
	return *this;
}
jgMatrix4x4& jgMatrix4x4::operator-=(const jgMatrix4x4& m) noexcept
{
	for (uint row = 0; row < 4; ++row)
	{
		for (uint col = 0; col < 4; ++col)
		{
			mat[row][col] -= m.mat[row][col];
		}
	}
	return *this;
}
jgMatrix4x4& jgMatrix4x4::operator*=(const jgMatrix4x4& m) noexcept
{
	jgMatrix4x4 result;
	for (uint row = 0; row < 4; ++row)
	{
		for (uint col = 0; col < 4; ++col)
		{
			result.mat[row][col] =
				(mat[row][0] * m.mat[0][col]) +
				(mat[row][1] * m.mat[1][col]) + 
				(mat[row][2] * m.mat[2][col]) +
				(mat[row][3] * m.mat[3][col]);
		}
	}
	(*this) = result;
	return *this;
}
jgMatrix4x4& jgMatrix4x4::operator*=(const real scalar) noexcept
{
	for (uint row = 0; row < 4; ++row)
	{
		for (uint col = 0; col < 4; ++col)
		{
			mat[row][col] *= scalar;
		}
	}
	return *this;
}
void jgMatrix4x4::print_cpp() const noexcept
{
	for (uint row = 0; row < 4; ++row)
	{
		cout << " [ ";
		for (uint col = 0; col < 4; ++col)
		{
			cout << mat[row][col];
			(col == 3) ? cout << " ] " << endl : cout << " , ";
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////
/////////////////////////////   jgLine   ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
jgLine::jgLine()
{

}
jgLine::~jgLine()
{


}
//////////////////////////////////////////////////////////////////////////////////
/////////////////////////////   jgPlane   ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
jgPlane::jgPlane(const jgVec3& normal, const jgVec3& vec)
{
	n = normal;
	n.normalize();
	d = -(n.x * vec.x) - (n.y * vec.y) - (n.z * vec.z);
}
jgPlane::jgPlane(const jgVec3& v1, const jgVec3& v2, const jgVec3& v3)
{
	jgVec3 v = v2 - v1; jgVec3 u = v3 - v1;

	n = v.cross(u);
	n.normalize();
	d = -(n.x * v1.x) + -(n.y * v1.y) + -(n.z * v1.z);
}
jgPlane::jgPlane(const jgPlane& copy)
{
	n = copy.n; d = copy.d;
}
jgPlane::~jgPlane() {}

real jgPlane::distance(const jgVec3& dot)
{
	return ABS(n.x * dot.x + n.y * dot.y + n.z * dot.z + d) / n.length();
}
real jgPlane::dotAssign(const jgVec3& dot)
{
	return (n.x * dot.x + n.y * dot.y + n.z * dot.z + d);
}
void jgPlane::print_cpp() const noexcept
{
	cout << n.x << "x " << n.y << "y " << n.z << "z " << d << endl;
}