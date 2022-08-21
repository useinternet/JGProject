#pragma once
#include "Math.h"
#include <DirectXMath.h>

class PVector3;
class PMatrix;
class PQuaternion
{
	friend PMatrix;

public:
	float32 x, y, z, w;

public:
	PQuaternion(const PVector3& v);
	PQuaternion(float32 pitch, float32 yaw, float32 roll);
	PQuaternion(float32 x = 0.0f, float32 y = 0.0f, float32 z = 0.0f, float32 w = 0.0f) 
		: x(x), y(y), z(z), w(w) {}

	PQuaternion(const PQuaternion& copy) = default;

	PQuaternion& operator=(const PQuaternion& q)
	{
		x = q.x;  y = q.y;  z = q.z; w = q.w;
		return *this;
	}

	PQuaternion operator*(const PQuaternion& q) const
	{
		PQuaternion result;
		result.SetSIMD(DirectX::XMQuaternionMultiply(GetSIMD(), q.GetSIMD()));
		return result;
	}

	PQuaternion& operator*=(const PQuaternion& q)
	{
		SetSIMD(DirectX::XMQuaternionMultiply(GetSIMD(), q.GetSIMD()));
		return *this;
	}

	bool operator==(const PQuaternion& q) const
	{
		return DirectX::XMQuaternionEqual(GetSIMD(), q.GetSIMD());
	}

	bool operator!=(const PQuaternion& q) const
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

	static PQuaternion ToQuaternion(const PVector3& euler);
	static PQuaternion ToQuaternion(float32 pitch, float32 yaw, float32 roll);

	static PQuaternion Identity()
	{
		PQuaternion q;
		q.SetSIMD(DirectX::XMQuaternionIdentity());
		return q;
	}

	static PQuaternion Inverse(const PQuaternion& q)
	{
		PQuaternion result;
		result.SetSIMD(DirectX::XMQuaternionInverse(q.GetSIMD()));
		return result;
	}

	static PQuaternion Normalize(const PQuaternion& q)
	{
		PQuaternion result;
		result.SetSIMD(DirectX::XMQuaternionNormalize(q.GetSIMD()));
		return result;
	}

	static PQuaternion Slerp(const PQuaternion& q1, const PQuaternion& q2, float32 t)
	{
		PQuaternion result;
		result.SetSIMD(DirectX::XMQuaternionSlerp(q1.GetSIMD(), q2.GetSIMD(), t));
		return result;
	}

	static PVector3 ToEuler(const PQuaternion& q);

	static PQuaternion RotationRollPitchYawFromVector(const PVector3& angles);

	static void ToAxisAngle(PVector3& out_axis, float32& out_angle, const PQuaternion& q);

	static PQuaternion RotatationAxis(const PVector3& axis, float32 angle);

	static PQuaternion FromTwoVectors(const PVector3& u, const PVector3& v);

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
