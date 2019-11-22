#include "pch.h"
#include "JQuaternion.h"
#include "JMatrix.h"
#include "JVector.h"
using namespace std;

JQuaternion::JQuaternion(const JVector3& v)
{
	ToQuaternion(v);
}
JQuaternion::JQuaternion(float pitch, float yaw, float roll)
{
	ToQuaternion({ pitch, yaw, roll });
}
// 
JQuaternion JQuaternion::ToQuaternion(const JVector3& euler)
{
	JQuaternion q;
	q.SetSIMD(DirectX::XMQuaternionRotationRollPitchYaw(euler.x, euler.y, euler.z));
	return q;
}
JQuaternion JQuaternion::ToQuaternion(float pitch, float yaw, float roll)
{
	return ToQuaternion({ pitch, yaw, roll });
}

JVector3 JQuaternion::ToEuler(const JQuaternion& q)
{
	JVector3 euler;
	// roll -> Z
	float sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
	float cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
	euler.z = std::atan2(sinr_cosp, cosr_cosp);
	// pitch -> x
	float sinp = 2 * (q.w * q.y - q.z * q.x);
	if (std::abs(sinp) >= 1)
	{
		euler.x = std::copysign(JG_PI / 2, sinp);
	}
	else
	{
		euler.x = std::asin(sinp);
	}
	//yaw -> y
	float siny_cosp = 2 * (q.w * q.z + q.x * q.y);
	float cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
	euler.y = std::atan2(siny_cosp, cosy_cosp);

	return euler;
}


JQuaternion JQuaternion::RotatationAxis(const JVector3& axis, float angle)
{
	JQuaternion q;
	DirectX::XMFLOAT3 v_axis = { axis.x, axis.y, axis.z };
	q.SetSIMD(DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&v_axis), angle));
	return q;
}


