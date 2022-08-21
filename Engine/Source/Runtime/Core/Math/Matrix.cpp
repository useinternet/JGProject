#include "Matrix.h"
#include "Vector.h"
#include "Quaternion.h"

PMatrix::PMatrix() :
	m_Data(
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f) {}

PMatrix::PMatrix(float32 init_value) :
	m_Data(
		init_value, init_value, init_value, init_value,
		init_value, init_value, init_value, init_value,
		init_value, init_value, init_value, init_value,
		init_value, init_value, init_value, init_value) {}

PMatrix::PMatrix(
	float32 w,
	const PVector3& v1,
	const PVector3& v2,
	const PVector3& v3,
	const PVector3& v4) :
	m_Data(
		v1.x, v1.y, v1.z, w,
		v2.x, v2.y, v2.z, w,
		v3.x, v3.y, v3.z, w,
		v4.x, v4.y, v4.z, w) {};

PMatrix::PMatrix(
	const PVector3& v1, float32 w1,
	const PVector3& v2, float32 w2,
	const PVector3& v3, float32 w3,
	const PVector3& v4, float32 w4) :
	m_Data(
		v1.x, v1.y, v1.z, w1,
		v2.x, v2.y, v2.z, w2,
		v3.x, v3.y, v3.z, w3,
		v4.x, v4.y, v4.z, w4) { }

PVector3 PMatrix::TransformPoint(const PVector3& v) const
{
	DirectX::XMVECTOR sim = PVector3::GetSIMD(v);
	sim = DirectX::XMVectorSetW(sim, 1.0f);

	DirectX::XMVECTOR sim_v = DirectX::XMVector3Transform(sim, GetSIMD());
	return PVector3::ConvertPVector3(sim_v);
}

PVector3 PMatrix::TransformVector(const PVector3& v) const
{
	DirectX::XMVECTOR sim_v = DirectX::XMVector3TransformNormal(PVector3::GetSIMD(v), GetSIMD());
	return PVector3::ConvertPVector3(sim_v);
}

PVector4 PMatrix::Transform(const PVector4& v) const
{
	DirectX::XMVECTOR sim = PVector4::GetSIMD(v);
	DirectX::XMVECTOR sim_v = DirectX::XMVector4Transform(sim, GetSIMD());

	return PVector4::ConvertPVector4(sim_v);
}

bool PMatrix::Decompose(PVector3* T, PQuaternion* R, PVector3* S) const
{
	PVector3 temp_t;
	PVector3 temp_s;
	PQuaternion temp_r;

	if (T) temp_t = *T;
	if (R) temp_r = *R;
	if (S) temp_s = *S;

	DirectX::XMVECTOR sim_t = PVector3::GetSIMD(temp_t);
	DirectX::XMVECTOR sim_r = temp_r.GetSIMD();
	DirectX::XMVECTOR sim_s = PVector3::GetSIMD(temp_s);

	bool result = DirectX::XMMatrixDecompose(&sim_s, &sim_r, &sim_t, GetSIMD());
	if (result == false)
	{
		return false;
	}

	if (T) *T = PVector3::ConvertPVector3(sim_t);
	if (R) R->SetSIMD(sim_r);
	if (S) *S = PVector3::ConvertPVector3(sim_s);

	return true;
}

PMatrix PMatrix::Translation(const PVector3& v)
{
	PMatrix m;
	m.SetSIMD(DirectX::XMMatrixTranslation(v.x, v.y, v.z));
	return m;
}

PMatrix PMatrix::Rotation(const PVector3& v)
{
	PMatrix m;
	m.SetSIMD(DirectX::XMMatrixRotationRollPitchYaw(v.x, v.y, v.z));
	return m;
}

PMatrix PMatrix::RotationYaw(float32 yaw)
{
	PMatrix m;
	m.SetSIMD(DirectX::XMMatrixRotationY(yaw));
	return m;
}

PMatrix PMatrix::RotationPitch(float32 pitch)
{
	PMatrix m;
	m.SetSIMD(DirectX::XMMatrixRotationX(pitch));
	return m;
}

PMatrix PMatrix::RotationRoll(float32 roll)
{
	PMatrix m;
	m.SetSIMD(DirectX::XMMatrixRotationZ(roll));
	return m;
}

PMatrix PMatrix::Rotation(const PQuaternion& q)
{
	PMatrix m;
	m.SetSIMD(DirectX::XMMatrixRotationQuaternion(q.GetSIMD()));
	return m;
}

PMatrix PMatrix::RotationAxis(const PVector3& axis, float32 angle)
{
	PMatrix m;
	m.SetSIMD(DirectX::XMMatrixRotationAxis(PVector3::GetSIMD(axis), angle));
	return m;
}

PMatrix PMatrix::Scaling(const PVector3& v)
{
	PMatrix m;
	m.SetSIMD(DirectX::XMMatrixScaling(v.x, v.y, v.z));
	return m;
}

PMatrix PMatrix::LookAtLH(const PVector3& pos, const PVector3& target, const PVector3& up)
{
	PMatrix m;
	m.SetSIMD(DirectX::XMMatrixLookAtLH(
		PVector3::GetSIMD(pos),
		PVector3::GetSIMD(target),
		PVector3::GetSIMD(up)));
	return m;
}

PMatrix PMatrix::LookToLH(const PVector3& pos, const PVector3& targetDir, const PVector3& up)
{
	PMatrix m;

	m.SetSIMD(DirectX::XMMatrixLookToLH(
		PVector3::GetSIMD(pos),
		PVector3::GetSIMD(targetDir),
		PVector3::GetSIMD(up)));
	return m;
}

PMatrix PMatrix::LookToLH(const PVector3& pos, const PVector3& up, const PVector3& right, const PVector3& look)
{
	PMatrix m;

	PVector3 Look = PVector3::Normalize(look);
	PVector3 Up = PVector3::Normalize(PVector3::Cross(Look, right));
	PVector3 Right = PVector3::Cross(Up, Look);

	PVector3 NegP = pos * -1;
	float32 x = PVector3::Dot(NegP, Right);
	float32 y = PVector3::Dot(NegP, Up);
	float32 z = PVector3::Dot(NegP, Look);

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

PVector3 PMatrix::ToEulerAngles(const PMatrix& m)
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

	return PVector3(x, y, z);
}

PMatrix PMatrix::AffineTransformation(const PVector3& T, const PQuaternion& Quat, const PVector3& S)
{
	PMatrix m;
	m.SetSIMD(DirectX::XMMatrixAffineTransformation(PVector3::GetSIMD(S), DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), Quat.GetSIMD(), PVector3::GetSIMD(T)));
	return m;
}