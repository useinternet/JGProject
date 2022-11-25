#include "PCH/PCH.h"
#include "Matrix.h"
#include "Vector.h"
#include "Quaternion.h"

HMatrix::HMatrix() :
	m_Data(
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f) {}

HMatrix::HMatrix(float32 init_value) :
	m_Data(
		init_value, init_value, init_value, init_value,
		init_value, init_value, init_value, init_value,
		init_value, init_value, init_value, init_value,
		init_value, init_value, init_value, init_value) {}

HMatrix::HMatrix(
	float32 w,
	const HVector3& v1,
	const HVector3& v2,
	const HVector3& v3,
	const HVector3& v4) :
	m_Data(
		v1.x, v1.y, v1.z, w,
		v2.x, v2.y, v2.z, w,
		v3.x, v3.y, v3.z, w,
		v4.x, v4.y, v4.z, w) {};

HMatrix::HMatrix(
	const HVector3& v1, float32 w1,
	const HVector3& v2, float32 w2,
	const HVector3& v3, float32 w3,
	const HVector3& v4, float32 w4) :
	m_Data(
		v1.x, v1.y, v1.z, w1,
		v2.x, v2.y, v2.z, w2,
		v3.x, v3.y, v3.z, w3,
		v4.x, v4.y, v4.z, w4) { }

HVector3 HMatrix::TransformPoint(const HVector3& v) const
{
	DirectX::XMVECTOR sim = HVector3::GetSIMD(v);
	sim = DirectX::XMVectorSetW(sim, 1.0f);

	DirectX::XMVECTOR sim_v = DirectX::XMVector3Transform(sim, GetSIMD());
	return HVector3::ConvertPVector3(sim_v);
}

HVector3 HMatrix::TransformVector(const HVector3& v) const
{
	DirectX::XMVECTOR sim_v = DirectX::XMVector3TransformNormal(HVector3::GetSIMD(v), GetSIMD());
	return HVector3::ConvertPVector3(sim_v);
}

HVector4 HMatrix::Transform(const HVector4& v) const
{
	DirectX::XMVECTOR sim = HVector4::GetSIMD(v);
	DirectX::XMVECTOR sim_v = DirectX::XMVector4Transform(sim, GetSIMD());

	return HVector4::ConvertPVector4(sim_v);
}

bool HMatrix::Decompose(HVector3* T, HQuaternion* R, HVector3* S) const
{
	HVector3 temp_t;
	HVector3 temp_s;
	HQuaternion temp_r;

	if (T) temp_t = *T;
	if (R) temp_r = *R;
	if (S) temp_s = *S;

	DirectX::XMVECTOR sim_t = HVector3::GetSIMD(temp_t);
	DirectX::XMVECTOR sim_r = temp_r.GetSIMD();
	DirectX::XMVECTOR sim_s = HVector3::GetSIMD(temp_s);

	bool result = DirectX::XMMatrixDecompose(&sim_s, &sim_r, &sim_t, GetSIMD());
	if (result == false)
	{
		return false;
	}

	if (T) *T = HVector3::ConvertPVector3(sim_t);
	if (R) R->SetSIMD(sim_r);
	if (S) *S = HVector3::ConvertPVector3(sim_s);

	return true;
}

HMatrix HMatrix::Translation(const HVector3& v)
{
	HMatrix m;
	m.SetSIMD(DirectX::XMMatrixTranslation(v.x, v.y, v.z));
	return m;
}

HMatrix HMatrix::Rotation(const HVector3& v)
{
	HMatrix m;
	m.SetSIMD(DirectX::XMMatrixRotationRollPitchYaw(v.x, v.y, v.z));
	return m;
}

HMatrix HMatrix::RotationYaw(float32 yaw)
{
	HMatrix m;
	m.SetSIMD(DirectX::XMMatrixRotationY(yaw));
	return m;
}

HMatrix HMatrix::RotationPitch(float32 pitch)
{
	HMatrix m;
	m.SetSIMD(DirectX::XMMatrixRotationX(pitch));
	return m;
}

HMatrix HMatrix::RotationRoll(float32 roll)
{
	HMatrix m;
	m.SetSIMD(DirectX::XMMatrixRotationZ(roll));
	return m;
}

HMatrix HMatrix::Rotation(const HQuaternion& q)
{
	HMatrix m;
	m.SetSIMD(DirectX::XMMatrixRotationQuaternion(q.GetSIMD()));
	return m;
}

HMatrix HMatrix::RotationAxis(const HVector3& axis, float32 angle)
{
	HMatrix m;
	m.SetSIMD(DirectX::XMMatrixRotationAxis(HVector3::GetSIMD(axis), angle));
	return m;
}

HMatrix HMatrix::Scaling(const HVector3& v)
{
	HMatrix m;
	m.SetSIMD(DirectX::XMMatrixScaling(v.x, v.y, v.z));
	return m;
}

HMatrix HMatrix::LookAtLH(const HVector3& pos, const HVector3& target, const HVector3& up)
{
	HMatrix m;
	m.SetSIMD(DirectX::XMMatrixLookAtLH(
		HVector3::GetSIMD(pos),
		HVector3::GetSIMD(target),
		HVector3::GetSIMD(up)));
	return m;
}

HMatrix HMatrix::LookToLH(const HVector3& pos, const HVector3& targetDir, const HVector3& up)
{
	HMatrix m;

	m.SetSIMD(DirectX::XMMatrixLookToLH(
		HVector3::GetSIMD(pos),
		HVector3::GetSIMD(targetDir),
		HVector3::GetSIMD(up)));
	return m;
}

HMatrix HMatrix::LookToLH(const HVector3& pos, const HVector3& up, const HVector3& right, const HVector3& look)
{
	HMatrix m;

	HVector3 Look = HVector3::Normalize(look);
	HVector3 Up = HVector3::Normalize(HVector3::Cross(Look, right));
	HVector3 Right = HVector3::Cross(Up, Look);

	HVector3 NegP = pos * -1;
	float32 x = HVector3::Dot(NegP, Right);
	float32 y = HVector3::Dot(NegP, Up);
	float32 z = HVector3::Dot(NegP, Look);

	m.Get(0, 0) = Right.x;
	m.Get(0, 1) = Right.y;
	m.Get(0, 2) = Right.z;
	m.Get(0, 3) = x;
	m.Get(1, 0) = Up.x;
	m.Get(1, 1) = Up.y;
	m.Get(1, 2) = Up.z;
	m.Get(1, 3) = y;
	m.Get(2, 0) = Look.x;
	m.Get(2, 1) = Look.y;
	m.Get(2, 2) = Look.z;
	m.Get(2, 3) = z;
	m.Get(3, 0) = 0.0f;
	m.Get(3, 1) = 0.0f;
	m.Get(3, 2) = 0.0f;
	m.Get(3, 3) = 1.0f;

	return m;
}

HVector3 HMatrix::ToEulerAngles(const HMatrix& m)
{
	float32 sy = sqrt(m.Get_C(0, 0) * m.Get_C(0, 0) + m.Get_C(1, 0) * m.Get_C(1, 0));

	bool singular = sy < 1e-6;
	float32 x, y, z;

	if (!singular)
	{
		x = atan2(m.Get_C(2, 1), m.Get_C(2, 2));
		y = atan2(-m.Get_C(2, 0), sy);
		z = atan2(m.Get_C(1, 0), m.Get_C(0, 0));
	}
	else
	{
		x = atan2(-m.Get_C(1, 2), m.Get_C(1, 1));
		y = atan2(-m.Get_C(2, 0), sy);
		z = 0;
	}

	return HVector3(x, y, z);
}

HMatrix HMatrix::AffineTransformation(const HVector3& T, const HQuaternion& Quat, const HVector3& S)
{
	HMatrix m;
	m.SetSIMD(DirectX::XMMatrixAffineTransformation(HVector3::GetSIMD(S), DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), Quat.GetSIMD(), HVector3::GetSIMD(T)));
	return m;
}