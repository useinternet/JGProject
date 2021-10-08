#include "pch.h"
#include "JMatrix.h"
#include "JVector.h"
#include "JQuaternion.h"

namespace JG
{
	JMatrix::JMatrix() :
		m_Data(
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f) {}
	JMatrix::JMatrix(float init_value) :
		m_Data(
			init_value, init_value, init_value, init_value,
			init_value, init_value, init_value, init_value,
			init_value, init_value, init_value, init_value,
			init_value, init_value, init_value, init_value) {}

	JMatrix::JMatrix(
		float w,
		const JVector3& v1,
		const JVector3& v2,
		const JVector3& v3,
		const JVector3& v4) :
		m_Data(
			v1.x, v1.y, v1.z, w,
			v2.x, v2.y, v2.z, w,
			v3.x, v3.y, v3.z, w,
			v4.x, v4.y, v4.z, w) {};


	JMatrix::JMatrix(
		const JVector3& v1, float w1,
		const JVector3& v2, float w2,
		const JVector3& v3, float w3,
		const JVector3& v4, float w4) :
		m_Data(
			v1.x, v1.y, v1.z, w1,
			v2.x, v2.y, v2.z, w2,
			v3.x, v3.y, v3.z, w3,
			v4.x, v4.y, v4.z, w4) { }

	JVector3 JMatrix::TransformPoint(const JVector3& v) const
	{
		auto sim = JVector3::GetSIMD(v);
		sim = DirectX::XMVectorSetW(sim, 1.0f);
		auto sim_v = DirectX::XMVector3Transform(sim, GetSIMD());
		return JVector3::ConvertJVector3(sim_v);
	}
	JVector3 JMatrix::TransformVector(const JVector3& v) const
	{
		auto sim_v = DirectX::XMVector3TransformNormal(JVector3::GetSIMD(v), GetSIMD());
		return JVector3::ConvertJVector3(sim_v);
	}
	JVector4 JMatrix::Transform(const JVector4& v) const
	{
		auto sim = JVector4::GetSIMD(v);
		auto sim_v = DirectX::XMVector4Transform(sim, GetSIMD());
		return JVector4::ConvertJVector4(sim_v);
	}
	JMatrix JMatrix::Translation(const JVector3& v)
	{
		JMatrix m;
		m.SetSIMD(DirectX::XMMatrixTranslation(v.x, v.y, v.z));
		return m;
	}
	JMatrix JMatrix::Rotation(const JVector3& v)
	{
		JMatrix m;
		m.SetSIMD(DirectX::XMMatrixRotationRollPitchYaw(v.x, v.y, v.z));
		return m;
	}

	JMatrix JMatrix::RotationYaw(float yaw)
	{
		JMatrix m;
		m.SetSIMD(DirectX::XMMatrixRotationY(yaw));
		return m;
	}

	JMatrix JMatrix::RotationPitch(float pitch)
	{
		JMatrix m;
		m.SetSIMD(DirectX::XMMatrixRotationX(pitch));
		return m;
	}

	JMatrix JMatrix::RotationRoll(float roll)
	{
		JMatrix m;
		m.SetSIMD(DirectX::XMMatrixRotationZ(roll));
		return m;
	}

	JMatrix JMatrix::Rotation(const JQuaternion& q)
	{
		JMatrix m;
		m.SetSIMD(DirectX::XMMatrixRotationQuaternion(q.GetSIMD()));
		return m;
	}
	JMatrix JMatrix::RotationAxis(const JVector3& axis, float angle)
	{
		JMatrix m;
		m.SetSIMD(DirectX::XMMatrixRotationAxis(JVector3::GetSIMD(axis), angle));
		return m;
	}
	JMatrix JMatrix::Scaling(const JVector3& v)
	{
		JMatrix m;
		m.SetSIMD(DirectX::XMMatrixScaling(v.x, v.y, v.z));
		return m;
	}
	JMatrix JMatrix::LookAtLH(const JVector3& pos, const JVector3& target, const JVector3& up)
	{
		JMatrix m;
		m.SetSIMD(DirectX::XMMatrixLookAtLH(
			JVector3::GetSIMD(pos),
			JVector3::GetSIMD(target),
			JVector3::GetSIMD(up)));
		return m;
	}

	JMatrix JMatrix::LookToLH(const JVector3& pos, const JVector3& targetDir, const JVector3& up)
	{
		JMatrix m;

		m.SetSIMD(DirectX::XMMatrixLookToLH(
			JVector3::GetSIMD(pos),
			JVector3::GetSIMD(targetDir),
			JVector3::GetSIMD(up)));
		return m;
	}

	JMatrix JMatrix::LookToLH(const JVector3& pos, const JVector3& up, const JVector3& right, const JVector3& look)
	{
		JMatrix m;

		JVector3 Look = JVector3::Normalize(look);
		JVector3 Up = JVector3::Normalize(JVector3::Cross(Look, right));
		JVector3 Right = JVector3::Cross(Up, Look);

		JVector3 NegP = pos * -1;
		float x = JVector3::Dot(NegP, Right);
		float y = JVector3::Dot(NegP, Up);
		float z = JVector3::Dot(NegP, Look);

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
	JVector3 JMatrix::ToEulerAngles(const JMatrix& m)
	{
		float sy = sqrt(m.Get_C(0, 0) * m.Get_C(0, 0) + m.Get_C(1, 0) * m.Get_C(1, 0));
		bool singular = sy < 1e-6;
		float x, y, z;
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

		return JVector3(x,y,z);
	}
}
