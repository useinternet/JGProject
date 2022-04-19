#include "JQuaternion.h"
#include "JQuaternion.h"
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
	JVector3 JQuaternion::ToEuler(const JQuaternion& q)
	{

		f32 sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
		f32 cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
		f32 roll = std::atan2(sinr_cosp, cosr_cosp);


		f32 sinp = 2 * (q.w * q.y - q.z * q.x);
		f32 pitch = 0.0f;
		if (std::abs(sinp) >= 1)
			pitch = std::copysign(JG_PI / 2, sinp); // use 90 degrees if out of range
		else
			pitch = std::asin(sinp);


		f32 siny_cosp = 2 * (q.w * q.z + q.x * q.y);
		f32 cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
		f32 yaw = std::atan2(siny_cosp, cosy_cosp);

		return JVector3(pitch, yaw, roll);
	}

	JQuaternion JQuaternion::RotationRollPitchYawFromVector(const JVector3& angles)
	{
		
		JQuaternion result;
		auto simQ = DirectX::XMQuaternionRotationRollPitchYawFromVector(JVector3::GetSIMD(angles));
		result.SetSIMD(simQ);
		return result;
	}

	void JQuaternion::ToAxisAngle(JVector3& out_axis, f32& out_angle, const JQuaternion& q)
	{
		auto sim_axis = JVector3::GetSIMD(out_axis);
		auto sim_q = q.GetSIMD();

		DirectX::XMQuaternionToAxisAngle(&sim_axis, &out_angle, sim_q);
		out_axis = JVector3::ConvertJVector3(sim_axis);
	}

	JQuaternion JQuaternion::RotatationAxis(const JVector3& axis, float angle)
	{
		JQuaternion q;
		DirectX::XMFLOAT3 v_axis = { axis.x, axis.y, axis.z };
		q.SetSIMD(DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&v_axis), angle));
		return q;
	}
}


