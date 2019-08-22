#pragma once
#include "CommonCore.h"
#include <DirectXMath.h>


//
class JMatrix;
class JVector
{
	friend JMatrix;
	using SimVec = DirectX::XMVECTOR;
public:
	float x, y, z;
public: // 생성자 및 소멸자
	JVector() : x(0), y(0), z(0) {}
	JVector(float init_value) : x(init_value), y(init_value), z(init_value) {}
	JVector(float x, float y, float z) : x(x), y(y), z(z) {}
	JVector(const JVector& v) = default;
	JVector(JVector&& v) = default;
public: // 연산자
	inline JVector& operator=(const JVector& v) = default;
	inline JVector& operator=(JVector&& v) = default;


	inline JVector operator+(const JVector& v) const {
		return JVector(x + v.x, y + v.y, z + v.z);
	}
	inline JVector operator-(const JVector& v) const {
		return JVector(x - v.x, y - v.y, z - v.z);
	}
	inline JVector& operator+=(const JVector& v) {
		x += v.x; y += v.y; z += v.z;
		return *this;
	}
	inline JVector& operator-=(const JVector& v) {
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}
	inline JVector operator*(float k) const {
		return JVector(x * k, y * k, z * k);
	}
	inline JVector operator/(float k) const {
		float inv_k = 1.0f / k;
		return JVector(x * inv_k, y * inv_k, z * inv_k);
	}
	inline JVector& operator*=(float k) {
		x *= k; y *= k; z *= k;
		return *this;
	}
	inline 	JVector& operator/=(float k) {
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
	inline static float Length(const JVector& v) {
		auto length = DirectX::XMVector3Length(GetSIMD(v));
		return DirectX::XMVectorGetX(length);
	}
	inline static JVector Abs(const JVector& v) {
		return JVector(std::abs(v.x), std::abs(v.y), std::abs(v.z));
	}
	inline static float LengthSqrd(const JVector& v) {
		auto length = DirectX::XMVector3LengthSq(GetSIMD(v));
		return DirectX::XMVectorGetX(length);
	}
	inline static JVector Normalize(const JVector& v) {
		auto sim = DirectX::XMVector3Normalize(GetSIMD(v));
		return ConvertJVector(sim);
	}
	inline static float Dot(const JVector& v1, const JVector& v2) {
		auto sim =  DirectX::XMVector3Dot(GetSIMD(v1), GetSIMD(v2));
		return DirectX::XMVectorGetX(sim);
	}
	inline static JVector Cross(const JVector& v1, const JVector& v2) {
		auto sim = DirectX::XMVector3Cross(GetSIMD(v1), GetSIMD(v2));
		return ConvertJVector(sim);
	}
	inline static bool Equals(const JVector& v1, const JVector& v2, float error_range = ERROR_RANGE)
	{
		JVector delta_v = v1 - v2;
		delta_v = Abs(delta_v);
		if (delta_v.x < error_range && delta_v.y < error_range && delta_v.z < error_range)
			return true;
		return false;
	}
private:
	inline static DirectX::XMVECTOR GetSIMD(const JVector& v)
	{
		DirectX::XMFLOAT3 value(v.x, v.y, v.z);
		return DirectX::XMLoadFloat3(&value);
	}
	inline static JVector ConvertJVector(DirectX::XMVECTOR sim_vec) {
		DirectX::XMFLOAT3 value;
		DirectX::XMStoreFloat3(&value, sim_vec);
		return JVector(value.x, value.y, value.z);
	}
	inline static void ConvertJVector(DirectX::XMVECTOR sim_vec, JVector& in_v) {
		DirectX::XMFLOAT3 value;
		DirectX::XMStoreFloat3(&value, sim_vec);
		in_v.x = value.x; in_v.y = value.y; in_v.z = value.z;
	}
};




JVector operator* (float k, const JVector& v);