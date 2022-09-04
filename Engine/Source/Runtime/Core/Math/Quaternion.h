#pragma once
#include "Math.h"
#include <DirectXMath.h>

class HVector3;
class HMatrix;
class HQuaternion
{
	friend HMatrix;

public:
	float32 x, y, z, w;

public:
	HQuaternion(const HVector3& v);
	HQuaternion(float32 pitch, float32 yaw, float32 roll);
	HQuaternion(float32 x = 0.0f, float32 y = 0.0f, float32 z = 0.0f, float32 w = 0.0f) 
		: x(x), y(y), z(z), w(w) {}

	HQuaternion(const HQuaternion& copy) = default;

	HQuaternion& operator=(const HQuaternion& q)
	{
		x = q.x;  y = q.y;  z = q.z; w = q.w;
		return *this;
	}

	HQuaternion operator*(const HQuaternion& q) const
	{
		HQuaternion result;
		result.SetSIMD(DirectX::XMQuaternionMultiply(GetSIMD(), q.GetSIMD()));
		return result;
	}

	HQuaternion& operator*=(const HQuaternion& q)
	{
		SetSIMD(DirectX::XMQuaternionMultiply(GetSIMD(), q.GetSIMD()));
		return *this;
	}

	bool operator==(const HQuaternion& q) const
	{
		return DirectX::XMQuaternionEqual(GetSIMD(), q.GetSIMD());
	}

	bool operator!=(const HQuaternion& q) const
	{
		return !DirectX::XMQuaternionEqual(GetSIMD(), q.GetSIMD());
	}

	float32& operator[](int32 idx) {
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
		assert(false && "Quaternion Index exceed..");
		return x;
	}

	float32 At(int32 idx) const {
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
		assert(false && "Quaternion Index exceed..");
		return x;
	}

	static HQuaternion ToQuaternion(const HVector3& euler);
	static HQuaternion ToQuaternion(float32 pitch, float32 yaw, float32 roll);

	static HQuaternion Identity()
	{
		HQuaternion q;
		q.SetSIMD(DirectX::XMQuaternionIdentity());
		return q;
	}

	static HQuaternion Inverse(const HQuaternion& q)
	{
		HQuaternion result;
		result.SetSIMD(DirectX::XMQuaternionInverse(q.GetSIMD()));
		return result;
	}

	static HQuaternion Normalize(const HQuaternion& q)
	{
		HQuaternion result;
		result.SetSIMD(DirectX::XMQuaternionNormalize(q.GetSIMD()));
		return result;
	}

	static HQuaternion Slerp(const HQuaternion& q1, const HQuaternion& q2, float32 t)
	{
		HQuaternion result;
		result.SetSIMD(DirectX::XMQuaternionSlerp(q1.GetSIMD(), q2.GetSIMD(), t));
		return result;
	}

	static HVector3 ToEuler(const HQuaternion& q);

	static HQuaternion RotationRollPitchYawFromVector(const HVector3& angles);

	static void ToAxisAngle(HVector3& out_axis, float32& out_angle, const HQuaternion& q);

	static HQuaternion RotatationAxis(const HVector3& axis, float32 angle);

	static HQuaternion FromTwoVectors(const HVector3& u, const HVector3& v);

private:
	DirectX::XMVECTOR GetSIMD() const
	{
		DirectX::XMFLOAT4 value(x, y, z, w);
		return DirectX::XMLoadFloat4(&value);
	}

	void SetSIMD(DirectX::XMVECTOR q)
	{
		DirectX::XMFLOAT4 value;
		DirectX::XMStoreFloat4(&value, q);

		x = value.x;
		y = value.y;
		z = value.z;
		w = value.w;
	}
};
