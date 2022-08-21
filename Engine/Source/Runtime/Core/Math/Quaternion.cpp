#include "Quaternion.h"
#include "Matrix.h"
#include "Vector.h"
using namespace std;

PQuaternion::PQuaternion(const PVector3& v)
{
	ToQuaternion(v);
}
PQuaternion::PQuaternion(float32  pitch, float32  yaw, float32  roll)
{
	ToQuaternion({ pitch, yaw, roll });
}
// 
PQuaternion PQuaternion::ToQuaternion(const PVector3& euler)
{
	PQuaternion q;
	q.SetSIMD(DirectX::XMQuaternionRotationRollPitchYaw(euler.x, euler.y, euler.z));
	return PQuaternion::Normalize(q);
}

PQuaternion PQuaternion::ToQuaternion(float32  pitch, float32  yaw, float32  roll)
{
	return ToQuaternion({ pitch, yaw, roll });
}
PVector3 PQuaternion::ToEuler(const PQuaternion& q)
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

	return PVector3(pitch, yaw, roll);
}

PQuaternion PQuaternion::RotationRollPitchYawFromVector(const PVector3& angles)
{

	PQuaternion result;
	auto simQ = DirectX::XMQuaternionRotationRollPitchYawFromVector(PVector3::GetSIMD(angles));
	result.SetSIMD(simQ);
	return result;
}

void PQuaternion::ToAxisAngle(PVector3& out_axis, float32& out_angle, const PQuaternion& q)
{
	auto sim_axis = PVector3::GetSIMD(out_axis);
	auto sim_q = q.GetSIMD();

	DirectX::XMQuaternionToAxisAngle(&sim_axis, &out_angle, sim_q);
	out_axis = PVector3::ConvertPVector3(sim_axis);
}

PQuaternion PQuaternion::RotatationAxis(const PVector3& axis, float32  angle)
{
	PQuaternion q;
	DirectX::XMFLOAT3 v_axis = { axis.x, axis.y, axis.z };
	q.SetSIMD(DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&v_axis), angle));
	return q;
}
PQuaternion PQuaternion::FromTwoVectors(const PVector3& u, const PVector3& v)
{
	float32  norm_u_norm_v = sqrt(PVector3::Dot(u, u) * PVector3::Dot(v, v));
	float32  real_part = norm_u_norm_v + PVector3::Dot(u, v);
	PVector3 w;

	if (real_part < 1.e-6f * norm_u_norm_v)
	{
		real_part = 0.0f;
		w = PMath::Abs(u.x) > PMath::Abs(u.z) ? PVector3(-u.y, u.x, 0.f)
			: PVector3(0.f, -u.z, u.y);
	}
	else
	{
		w = PVector3::Cross(u, v);
	}

	return PQuaternion::Normalize(PQuaternion(w.x, w.y, w.z, real_part));
}


