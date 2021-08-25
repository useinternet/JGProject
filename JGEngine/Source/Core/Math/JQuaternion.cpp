#include "pch.h"
#include "JQuaternion.h"
#include "JMatrix.h"
#include "JVector.h"
using namespace std;


namespace JG
{
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
		return JQuaternion::Normalize(q);
	}
	JQuaternion JQuaternion::ToQuaternion(float pitch, float yaw, float roll)
	{
		return ToQuaternion({ pitch, yaw, roll });
	}

	
	JQuaternion JQuaternion::RotatationAxis(const JVector3& axis, float angle)
	{
		JQuaternion q;
		DirectX::XMFLOAT3 v_axis = { axis.x, axis.y, axis.z };
		q.SetSIMD(DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&v_axis), angle));
		return q;
	}

}


