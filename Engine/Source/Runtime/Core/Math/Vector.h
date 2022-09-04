#pragma once

#include "Math.h"
#include "String/String.h"
#include <DirectXMath.h>

class HQuaternion;
class HMatrix;
class HLinearColor;
class HVector3;
class HVector2;
class HVector4;

template<typename T>
class HVector2Template
{
public:
	T x, y;
public:
	HVector2Template(T x = 0, T y = 0) : x(x), y(y) {}

public:
	bool operator==(const HVector2Template& v) const
	{
		return (v.x == x && v.y == y);
	}

	bool operator!=(const HVector2Template& v) const
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
class HVector3Template
{
public:
	T x, y, z;

public:
	HVector3Template(T x = 0, T y = 0, T z = 0) : x(x), y(y), z(z) {}

public:
	bool operator==(const HVector3Template& v) const
	{
		return (v.x == x && v.y == y && v.z == z);
	}

	bool operator!=(const HVector3Template& v) const
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
class HVector4Template
{
public:
	T x, y, z, w;

public:
	HVector4Template(T x = 0, T y = 0, T z = 0, T w = 0) : x(x), y(y), z(z), w(w) {}

public:
	bool operator==(const HVector4Template& v) const
	{
		return (v.x == x && v.y == y && v.z == z && v.w == w);
	}

	bool operator!=(const HVector4Template& v) const
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

using HVector2Int  = HVector2Template<int32>;
using HVector3Int  = HVector3Template<int32>;
using HVector4Int  = HVector4Template<int32>;
using HVector2Uint = HVector2Template<uint32>;
using HVector3Uint = HVector3Template<uint32>;
using HVector4Uint = HVector4Template<uint32>;

class HVector2
{
public:
	float32 x, y;

public:
	HVector2() : x(0), y(0) {}
	HVector2(float32 x, float32 y) : x(x), y(y) {}

	HVector2(const HVector2& v) = default;
	HVector2(HVector2&& v)      = default;

	HVector2& operator=(const HVector2& v) = default;
	HVector2& operator=(HVector2&& v) = default;

public:
	HVector2 operator+(const HVector2& v) const
	{
		return HVector2(x + v.x, y + v.y);
	}

	HVector2 operator-(const HVector2& v) const 
	{
		return HVector2(x - v.x, y - v.y);
	}

	HVector2& operator+=(const HVector2& v) 
	{
		x += v.x; y += v.y; 
		return *this;
	}

	HVector2& operator-=(const HVector2& v) {
		x -= v.x; y -= v.y; 
		return *this;
	}

	HVector2 operator*(float32 k) const
	{
		return HVector2(x * k, y * k);
	}

	HVector2 operator/(float32 k) const
	{
		k = 1.0f / k;
		return HVector2(x * k, y * k);
	}

	HVector2& operator*=(float32 k)
	{
		x *= k;
		y *= k;
		return *this;
	}

	HVector2& operator/=(float32 k)
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
	bool operator==(const HVector2& v) const 
	{
		return (v.x == x && v.y == y);
	}

	bool operator!=(const HVector2& v) const 
	{
		return (v.x != x || v.y != y);
	}

	PString ToString()  const 
	{
		return PString::Format("[ %.3f, %3.f ]", x, y);
	}

public:
	static float32 Length(const HVector2& v) 
	{
		DirectX::XMVECTOR len = DirectX::XMVector2Length(GetSIMD(v));
		return DirectX::XMVectorGetX(len);
	}

	static HVector2 Abs(const HVector2& v)
	{
		return HVector2(std::abs(v.x), std::abs(v.y));
	}

	static float32 LengthSqrd(const HVector2& v)
	{
		DirectX::XMVECTOR len = DirectX::XMVector2LengthSq(GetSIMD(v));
		return DirectX::XMVectorGetX(len);
	}

	static HVector2 Normalize(const HVector2& v)
	{
		DirectX::XMVECTOR sim = DirectX::XMVector2Normalize(GetSIMD(v));
		return ConvertPVector2(sim);
	}

	static float32 Dot(const HVector2& v1, const HVector2& v2)
	{
		DirectX::XMVECTOR sim = DirectX::XMVector2Dot(GetSIMD(v1), GetSIMD(v2));
		return DirectX::XMVectorGetX(sim);
	}

	inline static HVector2 Cross(const HVector2& v1, const HVector2& v2) 
	{
		DirectX::XMVECTOR sim = DirectX::XMVector2Cross(GetSIMD(v1), GetSIMD(v2));
		return ConvertPVector2(sim);
	}

	static bool Equals(const HVector2& v1, const HVector2& v2, float32 error_range = ERROR_RANGE)
	{
		HVector2 delta_v = v1 - v2;
		delta_v = Abs(delta_v);
		if (delta_v.x < error_range && delta_v.y < error_range)
			return true;
		return false;
	}

	static HVector2 Max(const HVector2& v1, const HVector2& v2)
	{
		return HVector2(HMath::Max(v1.x, v2.x), HMath::Max(v1.y, v2.y));
	}

	static HVector2 Min(const HVector2& v1, const HVector2& v2)
	{
		return HVector2(HMath::Min(v1.x, v2.x), HMath::Min(v1.y, v2.y));
	}

private:
	static DirectX::XMVECTOR GetSIMD(const HVector2& v)
	{
		DirectX::XMFLOAT2 value(v.x, v.y);
		return DirectX::XMLoadFloat2(&value);
	}

	static HVector2 ConvertPVector2(DirectX::XMVECTOR sim_vec)
	{
		DirectX::XMFLOAT2 value;
		DirectX::XMStoreFloat2(&value, sim_vec);
		return HVector2(value.x, value.y);
	}

	static void ConvertPVector2(DirectX::XMVECTOR sim_vec, HVector2& in_v)
	{
		DirectX::XMFLOAT2 value;
		DirectX::XMStoreFloat2(&value, sim_vec);
		in_v.x = value.x; in_v.y = value.y;
	}
};

class HVector4
{
public:
	float32 x, y, z, w;

public:
	HVector4() : x(0), y(0), z(0), w(0) {}
	HVector4(float32 x, float32 y, float32 z, float32 w) :
		x(x), y(y), z(z), w(w) {}

	HVector4(const HVector3& v, float32 f);
	HVector4(const HLinearColor& c);

public:
	HVector4 operator*(float32 k) const
	{
		return HVector4(x * k, y * k, z * k, w * k);
	}

	HVector4 operator/(float32 k) const
	{
		float32 inv_k = 1.0f / k;
		return HVector4(x * inv_k, y * inv_k, z * inv_k, w * inv_k);
	}

	HVector4& operator*=(float32 k)
	{
		x *= k; y *= k; z *= k; w *= k;
		return *this;
	}
	HVector4& operator/=(float32 k)
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
	static float32 Length(const HVector4& v) 
	{
		DirectX::XMVECTOR length = DirectX::XMVector4Length(GetSIMD(v));
		return DirectX::XMVectorGetX(length);
	}

	static HVector4 Normalize(const HVector4& v)
	{
		DirectX::XMVECTOR sim = DirectX::XMVector4Normalize(GetSIMD(v));
		return ConvertPVector4(sim);
	}

	static HVector4 Max(const HVector4& v1, const HVector4& v2)
	{
		return HVector4(HMath::Max(v1.x, v2.x), HMath::Max(v1.y, v2.y), HMath::Max(v1.z, v2.z), HMath::Max(v1.w, v2.w));
	}

	static HVector4 Min(const HVector4& v1, const HVector4& v2)
	{
		return HVector4(HMath::Min(v1.x, v2.x), HMath::Min(v1.y, v2.y), HMath::Min(v1.z, v2.z), HMath::Min(v1.w, v2.w));
	}

public:
	static DirectX::XMVECTOR GetSIMD(const HVector4& v)
	{
		DirectX::XMFLOAT4 value(v.x, v.y, v.z, v.w);
		return DirectX::XMLoadFloat4(&value);
	}

	static HVector4 ConvertPVector4(DirectX::XMVECTOR sim_vec)
	{
		DirectX::XMFLOAT4 value;
		DirectX::XMStoreFloat4(&value, sim_vec);
		return HVector4(value.x, value.y, value.z, value.w);
	}

	static void ConvertPVector4(DirectX::XMVECTOR sim_vec, HVector4& in_v)
	{
		DirectX::XMFLOAT4 value;
		DirectX::XMStoreFloat4(&value, sim_vec);
		in_v.x = value.x; in_v.y = value.y; in_v.z = value.z; in_v.w = value.w;
	}
};

class HVector3
{
	friend HQuaternion;
	friend HMatrix;

	using SimVec = DirectX::XMVECTOR;
public:
	float32 x, y, z;

public: // 생성자 및 소멸자
	HVector3() : x(0), y(0), z(0) {}
	HVector3(float32 init_value) : x(init_value), y(init_value), z(init_value) {}
	HVector3(float32 x, float32 y, float32 z) : x(x), y(y), z(z) {}
	HVector3(const HVector4& v) : x(v.x), y(v.y), z(v.z) {}
	HVector3(const HVector2& v, float32 z) : x(v.x), y(v.y), z(z) {}
	HVector3(const HLinearColor& color);
	HVector3(const HVector3& v) = default;
	HVector3(HVector3&& v) = default;

public: // 연산자
	HVector3& operator=(const HVector3& v) = default;
	HVector3& operator=(HVector3&& v) = default;


	HVector3 operator+(const HVector3& v) const
	{
		return HVector3(x + v.x, y + v.y, z + v.z);
	}

	HVector3 operator-(const HVector3& v) const
	{
		return HVector3(x - v.x, y - v.y, z - v.z);
	}

	HVector3& operator+=(const HVector3& v)
	{
		x += v.x; y += v.y; z += v.z;
		return *this;
	}

	HVector3& operator-=(const HVector3& v)
	{
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}

	HVector3 operator*(float32 k) const
	{
		return HVector3(x * k, y * k, z * k);
	}

	HVector3 operator/(float32 k) const
	{
		float inv_k = 1.0f / k;
		return HVector3(x * inv_k, y * inv_k, z * inv_k);
	}

	HVector3& operator*=(float32 k)
	{
		x *= k; y *= k; z *= k;
		return *this;
	}

	HVector3& operator/=(float32 k)
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
	bool operator==(const HVector3& v) const 
	{
		return (v.x == x && v.y == y && v.z == z);
	}

	bool operator!=(const HVector3& v) const 
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
	static float32 Length(const HVector3& v) 
	{
		DirectX::XMVECTOR length = DirectX::XMVector3Length(GetSIMD(v));
		return DirectX::XMVectorGetX(length);
	}

	static HVector3 Abs(const HVector3& v) 
	{
		return HVector3(HMath::Abs(v.x), HMath::Abs(v.y), HMath::Abs(v.z));
	}

	static float32 LengthSqrd(const HVector3& v) 
	{
		DirectX::XMVECTOR length = DirectX::XMVector3LengthSq(GetSIMD(v));
		return DirectX::XMVectorGetX(length);
	}

	static HVector3 Normalize(const HVector3& v) 
	{
		DirectX::XMVECTOR sim = DirectX::XMVector3Normalize(GetSIMD(v));
		return ConvertPVector3(sim);
	}

	static float Dot(const HVector3& v1, const HVector3& v2) 
	{
		DirectX::XMVECTOR sim = DirectX::XMVector3Dot(GetSIMD(v1), GetSIMD(v2));
		return DirectX::XMVectorGetX(sim);
	}

	static HVector3 Cross(const HVector3& v1, const HVector3& v2) 
	{
		DirectX::XMVECTOR sim = DirectX::XMVector3Cross(GetSIMD(v1), GetSIMD(v2));
		return ConvertPVector3(sim);
	}

	static float32 AnglesBetweenVectors(const HVector3& v1, const HVector3& v2)
	{
		DirectX::XMVECTOR sim = DirectX::XMVector3AngleBetweenVectors(GetSIMD(v1), GetSIMD(v2));
		return ConvertPVector3(sim).x;
	}

	static HVector3 Max(const HVector3& v1, const HVector3& v2)
	{
		return HVector3(HMath::Max(v1.x, v2.x), HMath::Max(v1.y, v2.y), HMath::Max(v1.z, v2.z));
	}

	static HVector3 Min(const HVector3& v1, const HVector3& v2)
	{
		return HVector3(HMath::Min(v1.x, v2.x), HMath::Min(v1.y, v2.y), HMath::Min(v1.z, v2.z));
	}

	static HVector3 Lerp(const HVector3& v1, const HVector3& v2, float32 a)
	{
		DirectX::XMVECTOR sim = DirectX::XMVectorLerp(GetSIMD(v1), GetSIMD(v2), a);
		return ConvertPVector3(sim);
	}

	static const HVector3& FowardVector()
	{
		static HVector3 v = HVector3(0.0f, 0.0f, 1.0f);
		return v;
	}

	static const HVector3& RightVector()
	{
		static HVector3 v = HVector3(1.0f, 0.0f, 0.0f);
		return v;
	}

	static const HVector3& UpVector()
	{
		static HVector3 v = HVector3(0.0f, 1.0f, 0.0f);
		return v;
	}

	static bool Equals(const HVector3& v1, const HVector3& v2, float32 error_range = ERROR_RANGE)
	{
		HVector3 delta_v = v1 - v2;
		delta_v = Abs(delta_v);

		if (delta_v.x < error_range && delta_v.y < error_range && delta_v.z < error_range)
			return true;

		return false;
	}

	static HVector3 UnProject(const HVector3& v, const HMatrix& proj, const HMatrix& view, const HMatrix& world, const HVector2& viewPortXY, const HVector2& viewportSize, float32 minZ = 0.0F, float32 maxZ = 1.0F);
private:
	static DirectX::XMVECTOR GetSIMD(const HVector3& v)
	{
		DirectX::XMFLOAT3 value(v.x, v.y, v.z);
		return DirectX::XMLoadFloat3(&value);
	}

	static HVector3 ConvertPVector3(DirectX::XMVECTOR sim_vec)
	{
		DirectX::XMFLOAT3 value;
		DirectX::XMStoreFloat3(&value, sim_vec);
		return HVector3(value.x, value.y, value.z);
	}

	static void ConvertPVector3(DirectX::XMVECTOR sim_vec, HVector3& in_v)
	{
		DirectX::XMFLOAT3 value;
		DirectX::XMStoreFloat3(&value, sim_vec);
		in_v.x = value.x; in_v.y = value.y; in_v.z = value.z;
	}
};

inline HVector2 operator* (float32 k, const HVector2& v);
inline HVector3 operator* (float32 k, const HVector3& v);
inline HVector4 operator* (float32 k, const HVector4& v);