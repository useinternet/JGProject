#pragma once

#include "Math.h"
#include "String/String.h"
#include <DirectXMath.h>

class PQuaternion;
class PMatrix;
class PLinearColor;
class PVector3;
class PVector2;
class PVector4;

template<typename T>
class PVector2Template
{
public:
	T x, y;
public:
	PVector2Template(T x = 0, T y = 0) : x(x), y(y) {}

public:
	bool operator==(const PVector2Template& v) const
	{
		return (v.x == x && v.y == y);
	}

	bool operator!=(const PVector2Template& v) const
	{
		return (v.x != x || v.y != y);
	}

	T& operator[](int32 idx)
	{
		switch (idx)
		{
		case 0:
			return x;
		case 1:
			return y;
		default:
			assert(false && "Vector Index exceed..");
			return x;
		}
	}
};

template<typename T>
class PVector3Template
{
public:
	T x, y, z;

public:
	PVector3Template(T x = 0, T y = 0, T z = 0) : x(x), y(y), z(z) {}

public:
	bool operator==(const PVector3Template& v) const
	{
		return (v.x == x && v.y == y && v.z == z);
	}

	bool operator!=(const PVector3Template& v) const
	{
		return (v.x != x || v.y != y || v.z != z);
	}

	T& operator[](int32 idx)
	{
		switch (idx)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		default:
			assert(false && "Vector Index exceed..");
			return x;
		}
	}
};

template<typename T>
class PVector4Template
{
public:
	T x, y, z, w;

public:
	PVector4Template(T x = 0, T y = 0, T z = 0, T w = 0) : x(x), y(y), z(z), w(w) {}

public:
	bool operator==(const PVector4Template& v) const
	{
		return (v.x == x && v.y == y && v.z == z && v.w == w);
	}

	bool operator!=(const PVector4Template& v) const
	{
		return (v.x != x || v.y != y || v.z != z || v.w != w);
	}

	T& operator[](int32 idx) {
		switch (idx)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		case 3:
			return w;
		default:
			assert(false && "Vector Index exceed..");
			return x;
		}
	}
};

using PVector2Int  = PVector2Template<int32>;
using PVector3Int  = PVector3Template<int32>;
using PVector4Int  = PVector4Template<int32>;
using PVector2Uint = PVector2Template<uint32>;
using PVector3Uint = PVector3Template<uint32>;
using PVector4Uint = PVector4Template<uint32>;

class PVector2
{
public:
	float32 x, y;

public:
	PVector2() : x(0), y(0) {}
	PVector2(float32 x, float32 y) : x(x), y(y) {}

	PVector2(const PVector2& v) = default;
	PVector2(PVector2&& v)      = default;

	PVector2& operator=(const PVector2& v) = default;
	PVector2& operator=(PVector2&& v) = default;

public:
	PVector2 operator+(const PVector2& v) const
	{
		return PVector2(x + v.x, y + v.y);
	}

	PVector2 operator-(const PVector2& v) const 
	{
		return PVector2(x - v.x, y - v.y);
	}

	PVector2& operator+=(const PVector2& v) 
	{
		x += v.x; y += v.y; 
		return *this;
	}

	PVector2& operator-=(const PVector2& v) {
		x -= v.x; y -= v.y; 
		return *this;
	}

	PVector2 operator*(float32 k) const
	{
		return PVector2(x * k, y * k);
	}

	PVector2 operator/(float32 k) const
	{
		k = 1.0f / k;
		return PVector2(x * k, y * k);
	}

	PVector2& operator*=(float32 k)
	{
		x *= k;
		y *= k;
		return *this;
	}

	PVector2& operator/=(float32 k)
	{
		k = 1.0f / k;
		x *= k; y *= k;
		return *this;
	}

	float32& operator[](int32 idx) {
		switch (idx)
		{
		case 0:
			return x;
		case 1:
			return y;
		default:
			assert(false && "Vector Index exceed..");
			return x;
		}
	}

public:
	bool operator==(const PVector2& v) const 
	{
		return (v.x == x && v.y == y);
	}

	bool operator!=(const PVector2& v) const 
	{
		return (v.x != x || v.y != y);
	}

	PString ToString()  const 
	{
		return PString::Format("[ %.3f, %3.f ]", x, y);
	}

public:
	static float32 Length(const PVector2& v) 
	{
		DirectX::XMVECTOR len = DirectX::XMVector2Length(GetSIMD(v));
		return DirectX::XMVectorGetX(len);
	}

	static PVector2 Abs(const PVector2& v)
	{
		return PVector2(std::abs(v.x), std::abs(v.y));
	}

	static float32 LengthSqrd(const PVector2& v)
	{
		DirectX::XMVECTOR len = DirectX::XMVector2LengthSq(GetSIMD(v));
		return DirectX::XMVectorGetX(len);
	}

	static PVector2 Normalize(const PVector2& v)
	{
		DirectX::XMVECTOR sim = DirectX::XMVector2Normalize(GetSIMD(v));
		return ConvertPVector2(sim);
	}

	static float32 Dot(const PVector2& v1, const PVector2& v2)
	{
		DirectX::XMVECTOR sim = DirectX::XMVector2Dot(GetSIMD(v1), GetSIMD(v2));
		return DirectX::XMVectorGetX(sim);
	}

	inline static PVector2 Cross(const PVector2& v1, const PVector2& v2) 
	{
		DirectX::XMVECTOR sim = DirectX::XMVector2Cross(GetSIMD(v1), GetSIMD(v2));
		return ConvertPVector2(sim);
	}

	static bool Equals(const PVector2& v1, const PVector2& v2, float32 error_range = ERROR_RANGE)
	{
		PVector2 delta_v = v1 - v2;
		delta_v = Abs(delta_v);
		if (delta_v.x < error_range && delta_v.y < error_range)
			return true;
		return false;
	}

	static PVector2 Max(const PVector2& v1, const PVector2& v2)
	{
		return PVector2(PMath::Max(v1.x, v2.x), PMath::Max(v1.y, v2.y));
	}

	static PVector2 Min(const PVector2& v1, const PVector2& v2)
	{
		return PVector2(PMath::Min(v1.x, v2.x), PMath::Min(v1.y, v2.y));
	}

private:
	static DirectX::XMVECTOR GetSIMD(const PVector2& v)
	{
		DirectX::XMFLOAT2 value(v.x, v.y);
		return DirectX::XMLoadFloat2(&value);
	}

	static PVector2 ConvertPVector2(DirectX::XMVECTOR sim_vec)
	{
		DirectX::XMFLOAT2 value;
		DirectX::XMStoreFloat2(&value, sim_vec);
		return PVector2(value.x, value.y);
	}

	static void ConvertPVector2(DirectX::XMVECTOR sim_vec, PVector2& in_v)
	{
		DirectX::XMFLOAT2 value;
		DirectX::XMStoreFloat2(&value, sim_vec);
		in_v.x = value.x; in_v.y = value.y;
	}
};

class PVector4
{
public:
	float32 x, y, z, w;

public:
	PVector4() : x(0), y(0), z(0), w(0) {}
	PVector4(float32 x, float32 y, float32 z, float32 w) :
		x(x), y(y), z(z), w(w) {}

	PVector4(const PVector3& v, float32 f);
	PVector4(const PLinearColor& c);

public:
	PVector4 operator*(float32 k) const
	{
		return PVector4(x * k, y * k, z * k, w * k);
	}

	PVector4 operator/(float32 k) const
	{
		float32 inv_k = 1.0f / k;
		return PVector4(x * inv_k, y * inv_k, z * inv_k, w * inv_k);
	}

	PVector4& operator*=(float32 k)
	{
		x *= k; y *= k; z *= k; w *= k;
		return *this;
	}
	PVector4& operator/=(float32 k)
	{
		float32 inv_k = 1.0f / k;
		x *= inv_k; y *= inv_k; z *= inv_k; w *= inv_k;
		return *this;
	}

public:
	float32& operator[](int32 idx) 
	{
		switch (idx)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		case 3:
			return w;
		}
		assert(false && "Vector Index exceed..");
		return x;
	}

	float32 At(int32 idx) const 
	{
		switch (idx)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		case 3:
			return w;
		}

		assert(false && "Vector Index exceed..");
		return FLT_MAX;
	}
public: // static 기능
	static float32 Length(const PVector4& v) 
	{
		DirectX::XMVECTOR length = DirectX::XMVector4Length(GetSIMD(v));
		return DirectX::XMVectorGetX(length);
	}

	static PVector4 Normalize(const PVector4& v)
	{
		DirectX::XMVECTOR sim = DirectX::XMVector4Normalize(GetSIMD(v));
		return ConvertPVector4(sim);
	}

	static PVector4 Max(const PVector4& v1, const PVector4& v2)
	{
		return PVector4(PMath::Max(v1.x, v2.x), PMath::Max(v1.y, v2.y), PMath::Max(v1.z, v2.z), PMath::Max(v1.w, v2.w));
	}

	static PVector4 Min(const PVector4& v1, const PVector4& v2)
	{
		return PVector4(PMath::Min(v1.x, v2.x), PMath::Min(v1.y, v2.y), PMath::Min(v1.z, v2.z), PMath::Min(v1.w, v2.w));
	}

public:
	static DirectX::XMVECTOR GetSIMD(const PVector4& v)
	{
		DirectX::XMFLOAT4 value(v.x, v.y, v.z, v.w);
		return DirectX::XMLoadFloat4(&value);
	}

	static PVector4 ConvertPVector4(DirectX::XMVECTOR sim_vec)
	{
		DirectX::XMFLOAT4 value;
		DirectX::XMStoreFloat4(&value, sim_vec);
		return PVector4(value.x, value.y, value.z, value.w);
	}

	static void ConvertPVector4(DirectX::XMVECTOR sim_vec, PVector4& in_v)
	{
		DirectX::XMFLOAT4 value;
		DirectX::XMStoreFloat4(&value, sim_vec);
		in_v.x = value.x; in_v.y = value.y; in_v.z = value.z; in_v.w = value.w;
	}
};

class PVector3
{
	friend PQuaternion;
	friend PMatrix;

	using SimVec = DirectX::XMVECTOR;
public:
	float32 x, y, z;

public: // 생성자 및 소멸자
	PVector3() : x(0), y(0), z(0) {}
	PVector3(float32 init_value) : x(init_value), y(init_value), z(init_value) {}
	PVector3(float32 x, float32 y, float32 z) : x(x), y(y), z(z) {}
	PVector3(const PVector4& v) : x(v.x), y(v.y), z(v.z) {}
	PVector3(const PVector2& v, float32 z) : x(v.x), y(v.y), z(z) {}
	PVector3(const PLinearColor& color);
	PVector3(const PVector3& v) = default;
	PVector3(PVector3&& v) = default;

public: // 연산자
	PVector3& operator=(const PVector3& v) = default;
	PVector3& operator=(PVector3&& v) = default;


	PVector3 operator+(const PVector3& v) const
	{
		return PVector3(x + v.x, y + v.y, z + v.z);
	}

	PVector3 operator-(const PVector3& v) const
	{
		return PVector3(x - v.x, y - v.y, z - v.z);
	}

	PVector3& operator+=(const PVector3& v)
	{
		x += v.x; y += v.y; z += v.z;
		return *this;
	}

	PVector3& operator-=(const PVector3& v)
	{
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}

	PVector3 operator*(float32 k) const
	{
		return PVector3(x * k, y * k, z * k);
	}

	PVector3 operator/(float32 k) const
	{
		float inv_k = 1.0f / k;
		return PVector3(x * inv_k, y * inv_k, z * inv_k);
	}

	PVector3& operator*=(float32 k)
	{
		x *= k; y *= k; z *= k;
		return *this;
	}

	PVector3& operator/=(float32 k)
	{
		float inv_k = 1.0f / k;
		x *= inv_k; y *= inv_k; z *= inv_k;
		return *this;
	}

	float32& operator[](int32 idx)
	{
		switch (idx)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		}
		assert(false && "Vector Index exceed..");
		return x;
	}

	float32 At(int32 idx) const
	{
		switch (idx)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		}
		assert(false && "Vector Index exceed..");
		return FLT_MAX;
	}

public:
	bool operator==(const PVector3& v) const 
	{
		return (v.x == x && v.y == y && v.z == z);
	}

	bool operator!=(const PVector3& v) const 
	{
		return (v.x != x || v.y != y || v.z != z);
	}

public: // 기능들
	void  Set(float32 x, float32 y, float32 z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	PString ToString()  const
	{
		return PString::Format("[ %.3f, %3.f, %3.f ]", x, y, z);
	}

public: // static 기능
	static float32 Length(const PVector3& v) 
	{
		DirectX::XMVECTOR length = DirectX::XMVector3Length(GetSIMD(v));
		return DirectX::XMVectorGetX(length);
	}

	static PVector3 Abs(const PVector3& v) 
	{
		return PVector3(PMath::Abs(v.x), PMath::Abs(v.y), PMath::Abs(v.z));
	}

	static float32 LengthSqrd(const PVector3& v) 
	{
		DirectX::XMVECTOR length = DirectX::XMVector3LengthSq(GetSIMD(v));
		return DirectX::XMVectorGetX(length);
	}

	static PVector3 Normalize(const PVector3& v) 
	{
		DirectX::XMVECTOR sim = DirectX::XMVector3Normalize(GetSIMD(v));
		return ConvertPVector3(sim);
	}

	static float Dot(const PVector3& v1, const PVector3& v2) 
	{
		DirectX::XMVECTOR sim = DirectX::XMVector3Dot(GetSIMD(v1), GetSIMD(v2));
		return DirectX::XMVectorGetX(sim);
	}

	static PVector3 Cross(const PVector3& v1, const PVector3& v2) 
	{
		DirectX::XMVECTOR sim = DirectX::XMVector3Cross(GetSIMD(v1), GetSIMD(v2));
		return ConvertPVector3(sim);
	}

	static float32 AnglesBetweenVectors(const PVector3& v1, const PVector3& v2)
	{
		DirectX::XMVECTOR sim = DirectX::XMVector3AngleBetweenVectors(GetSIMD(v1), GetSIMD(v2));
		return ConvertPVector3(sim).x;
	}

	static PVector3 Max(const PVector3& v1, const PVector3& v2)
	{
		return PVector3(PMath::Max(v1.x, v2.x), PMath::Max(v1.y, v2.y), PMath::Max(v1.z, v2.z));
	}

	static PVector3 Min(const PVector3& v1, const PVector3& v2)
	{
		return PVector3(PMath::Min(v1.x, v2.x), PMath::Min(v1.y, v2.y), PMath::Min(v1.z, v2.z));
	}

	static PVector3 Lerp(const PVector3& v1, const PVector3& v2, float32 a)
	{
		DirectX::XMVECTOR sim = DirectX::XMVectorLerp(GetSIMD(v1), GetSIMD(v2), a);
		return ConvertPVector3(sim);
	}

	static const PVector3& FowardVector()
	{
		static PVector3 v = PVector3(0.0f, 0.0f, 1.0f);
		return v;
	}

	static const PVector3& RightVector()
	{
		static PVector3 v = PVector3(1.0f, 0.0f, 0.0f);
		return v;
	}

	static const PVector3& UpVector()
	{
		static PVector3 v = PVector3(0.0f, 1.0f, 0.0f);
		return v;
	}

	static bool Equals(const PVector3& v1, const PVector3& v2, float32 error_range = ERROR_RANGE)
	{
		PVector3 delta_v = v1 - v2;
		delta_v = Abs(delta_v);

		if (delta_v.x < error_range && delta_v.y < error_range && delta_v.z < error_range)
			return true;

		return false;
	}

	static PVector3 UnProject(const PVector3& v, const PMatrix& proj, const PMatrix& view, const PMatrix& world, const PVector2& viewPortXY, const PVector2& viewportSize, float32 minZ = 0.0F, float32 maxZ = 1.0F);
private:
	static DirectX::XMVECTOR GetSIMD(const PVector3& v)
	{
		DirectX::XMFLOAT3 value(v.x, v.y, v.z);
		return DirectX::XMLoadFloat3(&value);
	}

	static PVector3 ConvertPVector3(DirectX::XMVECTOR sim_vec)
	{
		DirectX::XMFLOAT3 value;
		DirectX::XMStoreFloat3(&value, sim_vec);
		return PVector3(value.x, value.y, value.z);
	}

	static void ConvertPVector3(DirectX::XMVECTOR sim_vec, PVector3& in_v)
	{
		DirectX::XMFLOAT3 value;
		DirectX::XMStoreFloat3(&value, sim_vec);
		in_v.x = value.x; in_v.y = value.y; in_v.z = value.z;
	}
};

inline PVector2 operator* (float32 k, const PVector2& v);
inline PVector3 operator* (float32 k, const PVector3& v);
inline PVector4 operator* (float32 k, const PVector4& v);