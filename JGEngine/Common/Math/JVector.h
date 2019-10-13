#pragma once
//#include "CommonCore.h"
#include "Defined.h"
#include <DirectXMath.h>


//
class JMatrix;

class JVector2
{
public:
	float x, y;
public:
	JVector2() : x(0), y(0) {}
};

class JVector3
{
	friend JMatrix;
	using SimVec = DirectX::XMVECTOR;
public:
	float x, y, z;
public: // 생성자 및 소멸자
	JVector3() : x(0), y(0), z(0) {}
	JVector3(float init_value) : x(init_value), y(init_value), z(init_value) {}
	JVector3(float x, float y, float z) : x(x), y(y), z(z) {}
	JVector3(const JVector3& v) = default;
	JVector3(JVector3&& v) = default;
public: // 연산자
	inline JVector3& operator=(const JVector3& v) = default;
	inline JVector3& operator=(JVector3&& v) = default;


	inline JVector3 operator+(const JVector3& v) const {
		return JVector3(x + v.x, y + v.y, z + v.z);
	}
	inline JVector3 operator-(const JVector3& v) const {
		return JVector3(x - v.x, y - v.y, z - v.z);
	}
	inline JVector3& operator+=(const JVector3& v) {
		x += v.x; y += v.y; z += v.z;
		return *this;
	}
	inline JVector3& operator-=(const JVector3& v) {
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}
	inline JVector3 operator*(float k) const {
		return JVector3(x * k, y * k, z * k);
	}
	inline JVector3 operator/(float k) const {
		float inv_k = 1.0f / k;
		return JVector3(x * inv_k, y * inv_k, z * inv_k);
	}
	inline JVector3& operator*=(float k) {
		x *= k; y *= k; z *= k;
		return *this;
	}
	inline 	JVector3& operator/=(float k) {
		float inv_k = 1.0f / k;
		x *= inv_k; y *= inv_k; z *= inv_k;
		return *this;
	}
	inline float& operator[](int idx) {
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
public: // 기능들
	inline void  Set(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	inline std::string ToString()  const {
		return "[ " + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + " ]";
	}
public: // static 기능
	inline static float Length(const JVector3& v) {
		auto length = DirectX::XMVector3Length(GetSIMD(v));
		return DirectX::XMVectorGetX(length);
	}
	inline static JVector3 Abs(const JVector3& v) {
		return JVector3(std::abs(v.x), std::abs(v.y), std::abs(v.z));
	}
	inline static float LengthSqrd(const JVector3& v) {
		auto length = DirectX::XMVector3LengthSq(GetSIMD(v));
		return DirectX::XMVectorGetX(length);
	}
	inline static JVector3 Normalize(const JVector3& v) {
		auto sim = DirectX::XMVector3Normalize(GetSIMD(v));
		return ConvertJVector3(sim);
	}
	inline static float Dot(const JVector3& v1, const JVector3& v2) {
		auto sim =  DirectX::XMVector3Dot(GetSIMD(v1), GetSIMD(v2));
		return DirectX::XMVectorGetX(sim);
	}
	inline static JVector3 Cross(const JVector3& v1, const JVector3& v2) {
		auto sim = DirectX::XMVector3Cross(GetSIMD(v1), GetSIMD(v2));
		return ConvertJVector3(sim);
	}
	inline static bool Equals(const JVector3& v1, const JVector3& v2, float error_range = ERROR_RANGE)
	{
		JVector3 delta_v = v1 - v2;
		delta_v = Abs(delta_v);
		if (delta_v.x < error_range && delta_v.y < error_range && delta_v.z < error_range)
			return true;
		return false;
	}
private:
	inline static DirectX::XMVECTOR GetSIMD(const JVector3& v)
	{
		DirectX::XMFLOAT3 value(v.x, v.y, v.z);
		return DirectX::XMLoadFloat3(&value);
	}
	inline static JVector3 ConvertJVector3(DirectX::XMVECTOR sim_vec) {
		DirectX::XMFLOAT3 value;
		DirectX::XMStoreFloat3(&value, sim_vec);
		return JVector3(value.x, value.y, value.z);
	}
	inline static void ConvertJVector3(DirectX::XMVECTOR sim_vec, JVector3& in_v) {
		DirectX::XMFLOAT3 value;
		DirectX::XMStoreFloat3(&value, sim_vec);
		in_v.x = value.x; in_v.y = value.y; in_v.z = value.z;
	}
};




JVector3 operator* (float k, const JVector3& v);