#include "PCH/PCH.h"
#include "Quaternion.h"
#include "Matrix.h"
#include "Vector.h"
using namespace std;

HQuaternion::HQuaternion(const HVector3& v)
{
	ToQuaternion(v);
}
HQuaternion::HQuaternion(float32  pitch, float32  yaw, float32  roll)
{
	ToQuaternion({ pitch, yaw, roll });
}
// 
HQuaternion HQuaternion::ToQuaternion(const HVector3& euler)
{
	HQuaternion q;
	q.SetSIMD(DirectX::XMQuaternionRotationRollPitchYaw(euler.x, euler.y, euler.z));
	return HQuaternion::Normalize(q);
}

HQuaternion HQuaternion::ToQuaternion(float32  pitch, float32  yaw, float32  roll)
{
	return ToQuaternion({ pitch, yaw, roll });
}
HVector3 HQuaternion::ToEuler(const HQuaternion& q)
{

	float32 sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
	float32 cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
	float32 roll = std::atan2(sinr_cosp, cosr_cosp);


	float32 sinp = 2 * (q.w * q.y - q.z * q.x);
	float32 pitch = 0.0f;
	if (std::abs(sinp) >= 1)
		pitch = std::copysign(JG_PI / 2, sinp); // use 90 degrees if out of range
	else
		pitch = std::asin(sinp);


	float32 siny_cosp = 2 * (q.w * q.z + q.x * q.y);
	float32 cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
	float32 yaw = std::atan2(siny_cosp, cosy_cosp);

	return HVector3(pitch, yaw, roll);
}

HQuaternion HQuaternion::RotationRollPitchYawFromVector(const HVector3& angles)
{

	HQuaternion result;
	auto simQ = DirectX::XMQuaternionRotationRollPitchYawFromVector(HVector3::GetSIMD(angles));
	result.SetSIMD(simQ);
	return result;
}

void HQuaternion::ToAxisAngle(HVector3& out_axis, float32& out_angle, const HQuaternion& q)
{
	auto sim_axis = HVector3::GetSIMD(out_axis);
	auto sim_q = q.GetSIMD();

	DirectX::XMQuaternionToAxisAngle(&sim_axis, &out_angle, sim_q);
	out_axis = HVector3::ConvertPVector3(sim_axis);
}

HQuaternion HQuaternion::RotatationAxis(const HVector3& axis, float32  angle)
{
	HQuaternion q;
	DirectX::XMFLOAT3 v_axis = { axis.x, axis.y, axis.z };
	q.SetSIMD(DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&v_axis), angle));
	return q;
}
HQuaternion HQuaternion::FromTwoVectors(const HVector3& u, const HVector3& v)
{
	float32  norm_u_norm_v = sqrt(HVector3::Dot(u, u) * HVector3::Dot(v, v));
	float32  real_part = norm_u_norm_v + HVector3::Dot(u, v);
	HVector3 w;

	if (real_part < 1.e-6f * norm_u_norm_v)
	{
		real_part = 0.0f;
		w = HMath::Abs(u.x) > HMath::Abs(u.z) ? HVector3(-u.y, u.x, 0.f)
			: HVector3(0.f, -u.z, u.y);
	}
	else
	{
		w = HVector3::Cross(u, v);
	}

	return HQuaternion::Normalize(HQuaternion(w.x, w.y, w.z, real_part));
}


