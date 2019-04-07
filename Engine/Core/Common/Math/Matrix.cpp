#include<PCH.h>
#include"Matrix.h"
#include"Vector.h"

using namespace std;
using namespace Common;

void JMatrix4x4::LookAtLH(const JVector3& pos, const JVector3& target, const JVector3& up)
{
	m_SimMat = DirectX::XMMatrixLookAtLH(pos.GetSIMD(), target.GetSIMD(), up.GetSIMD());
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
