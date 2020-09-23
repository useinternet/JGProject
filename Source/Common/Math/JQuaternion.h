#pragma once
#include "Math.h"
#include <DirectXMath.h>

class JVector3;
class JMatrix;
class JQuaternion
{
	friend JMatrix;
public:
	float x, y, z, w;
public:
	JQuaternion(const JVector3& v);
	JQuaternion(float pitch, float yaw, float roll);
	JQuaternion(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 0.0f) :
		x(x), y(y), z(z), w(w) {}
	JQuaternion(const JQuaternion& copy) = default;




	JQuaternion& operator=(const JQuaternion& q)
	{
		x = q.x;  y = q.y;  z = q.z; w = q.w;
		return *this;
	}

	JQuaternion operator*(const JQuaternion& q)
	{
		JQuaternion result;
		result.SetSIMD(DirectX::XMQuaternionMultiply(GetSIMD(), q.GetSIMD()));
		return result;
	}
	JQuaternion& operator*=(const JQuaternion& q)
	{
		SetSIMD(DirectX::XMQuaternionMultiply(GetSIMD(), q.GetSIMD()));
		return *this;
	}
	bool operator==(const JQuaternion& q)
	{
		return DirectX::XMQuaternionEqual(GetSIMD(), q.GetSIMD());
	}
	bool operator!=(const JQuaternion& q)
	{
		return !DirectX::XMQuaternionEqual(GetSIMD(), q.GetSIMD());
	}

	static JQuaternion ToQuaternion(const JVector3& euler);
	static JQuaternion ToQuaternion(float pitch, float yaw, float roll);
	//static JVector3 ToEuler(const JQuaternion& q);

	inline static JQuaternion Identity()
	{
		JQuaternion q;
		q.SetSIMD(DirectX::XMQuaternionIdentity());
		return q;
	}
	inline static JQuaternion Normalize(const JQuaternion& q)
	{
		JQuaternion result;
		result.SetSIMD(DirectX::XMQuaternionNormalize(q.GetSIMD()));
		return result;
	}
	inline static JQuaternion Slerp(const JQuaternion& q1, const JQuaternion& q2, float t)
	{
		JQuaternion result;
		result.SetSIMD(DirectX::XMQuaternionSlerp(q1.GetSIMD(), q2.GetSIMD(), t));
		return result;
	}

	inline static JQuaternion RotatationAxis(const JVector3& axis, float angle);
private:
	inline DirectX::XMVECTOR GetSIMD() const
	{
		DirectX::XMFLOAT4 value(x, y, z, w);
		return DirectX::XMLoadFloat4(&value);
	}
	inline void SetSIMD(DirectX::XMVECTOR q)
	{
		DirectX::XMFLOAT4 value;
		DirectX::XMStoreFloat4(&value, q);

		x = value.x;
		y = value.y;
		z = value.z;
		w = value.w;
	}

};