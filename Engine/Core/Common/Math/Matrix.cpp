#include<PCH.h>
#include"Matrix.h"
#include"Vector.h"

using namespace std;
namespace Common
{
	void JMatrix4x4::LookAtLH(const JVector3& pos, const JVector3& target, const JVector3& up)
	{
		m_SimMat = DirectX::XMMatrixLookAtLH(pos.GetSIMD(), target.GetSIMD(), up.GetSIMD());
	}
	void JMatrix4x4::Inverse()
	{
		m_SimMat = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(m_SimMat), m_SimMat);
	}
	void JMatrix4x4::Translation(const JVector3& v)
	{
		auto v3f = v.Get();
		m_SimMat = DirectX::XMMatrixTranslation(v3f.x, v3f.y, v3f.z);
	}
	void JMatrix4x4::Rotation(const JVector3& v)
	{
		auto v3f = v.Get();
		m_SimMat = DirectX::XMMatrixRotationRollPitchYaw(v3f.x, v3f.y, v3f.z);
	}
	void JMatrix4x4::RotationAxis(const JVector3& axis, float angle)
	{
		m_SimMat = DirectX::XMMatrixRotationAxis(axis.GetSIMD(), angle);
	}
	void JMatrix4x4::Scaling(const JVector3& v)
	{
		auto v3f = v.Get();
		m_SimMat = DirectX::XMMatrixScaling(v3f.x, v3f.y, v3f.z);
	}

	JVector3 JMatrix4x4::TransformPoint(const JVector3& v) const
	{
		return JVector3(DirectX::XMVector2TransformCoord(v.GetSIMD(), m_SimMat));
	}
	JVector3 JMatrix4x4::TransformVector(const JVector3& v) const
	{
		return JVector3(DirectX::XMVector3TransformNormal(v.GetSIMD(), m_SimMat));
	}




	JMatrix4x4 Identity()
	{
		JMatrix4x4 m;
		m.Identity();
		return m;
	}
	JMatrix4x4 Translation(float x, float y, float z)
	{
		JMatrix4x4 m;
		m.Translation(x, y, z);
		return m;
	}
	JMatrix4x4 Rotation(float pitch, float yaw, float roll)
	{
		JMatrix4x4 m;
		m.Rotation(pitch, yaw, roll);
		return m;
	}
	JMatrix4x4 RotationX(float x)
	{
		JMatrix4x4 m;
		m.RotationX(x);
		return m;
	}
	JMatrix4x4 RotationY(float y)
	{
		JMatrix4x4 m;
		m.RotationY(y);
		return m;
	}
	JMatrix4x4 RotationZ(float z)
	{
		JMatrix4x4 m;
		m.RotationZ(z);
		return m;
	}
	JMatrix4x4 Scaling(float x, float y, float z)
	{
		JMatrix4x4 m;
		m.Scaling(x, y, z);
		return m;
	}
	JMatrix4x4 PerspectiveFovLH(float fov, float aspectRatio, float nearZ, float farZ)
	{
		JMatrix4x4 m;
		m.PerspectiveFovLH(fov, aspectRatio, nearZ, farZ);
		return m;
	}
	JMatrix4x4 OrthographicLH(float width, float height, float nearZ, float farZ)
	{
		JMatrix4x4 m;
		m.OrthographicLH(width, height, nearZ, farZ);
		return m;
	}
	JMatrix4x4 Transpose(const JMatrix4x4& matrix)
	{
		JMatrix4x4 m = matrix;
		m.Transpose();
		return m;
	}
	JMatrix4x4 Translation(const JVector3& v)
	{
		JMatrix4x4 m;
		m.Translation(v);
		return m;
	}
	JMatrix4x4 Rotation(const JVector3& v)
	{
		JMatrix4x4 m;
		m.Rotation(v);
		return m;
	}
	JMatrix4x4 RotationAxis(const JVector3& axis, float angle)
	{
		JMatrix4x4 m;
		m.RotationAxis(axis, angle);
		return m;
	}
	JMatrix4x4 Scaling(const JVector3& v)
	{
		JMatrix4x4 m;
		m.Scaling(v);
		return m;
	}
	JMatrix4x4 LookAtLH(const JVector3& pos, const JVector3& target, const JVector3& up)
	{
		JMatrix4x4 m;
		m.LookAtLH(pos, target, up);
		return m;
	}
	JMatrix4x4 Inverse(const JMatrix4x4& matrix)
	{
		JMatrix4x4 m = matrix;
		m.Inverse();
		return m;
	}
	JVector3 TransformPoint(const JMatrix4x4& m, const JVector3& v)
	{
		return m.TransformPoint(v);
	}
	JVector3 TransformVector(const JMatrix4x4& m, const JVector3& v)
	{
		return m.TransformVector(v);
	}
}

