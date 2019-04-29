#include<PCH.h>
#include"Camera.h"
using namespace Dx12;
using namespace Common;
Camera::Camera()
{
	m_Target.Set(0.0f, 0.0f, 1.0f);
	m_Up.Set(0.0f, 1.0f, 0.0f);
	m_Position.Set(0.0f, 0.0f, 0.0f);
	UpdateMatrix();
}
Camera::Camera(float fov, float width, float height, float nearZ, float farZ)
	: Camera()
{
	SetLens(fov, width, height, nearZ, farZ);
}
void Camera::SetPosition(const JVector3& v)
{
	m_Position.Set(v);

	m_bUpdate = true;
}
void Camera::SetUp(const JVector3& v)
{
	m_Up.Set(v);
	m_bUpdate = true;
}
void Camera::SetTarget(const JVector3& v)
{
	m_Target.Set(v);
	m_bUpdate = true;
}
void Camera::RightMove(float speed)
{
	m_Position += (GetRightVector() * speed);

	m_bUpdate = true;
}
void Camera::FowardMove(float speed)
{
	m_Position += (GetLookVector() * speed);

	m_bUpdate = true;
}
void Camera::UpMove(float speed)
{
	m_Position.SetY(m_Position.Y() + speed);

	m_bUpdate = true;
}
void Camera::RotationPitch(float pitch)
{
	m_CameraRotator.OffsetPitch(pitch);
	m_bUpdate = true;
}
void Camera::RotationYaw(float yaw)
{
	m_CameraRotator.OffsetYaw(yaw);
	m_bUpdate = true;
}
void Camera::RotationRoll(float roll)
{
	m_CameraRotator.OffsetRoll(roll);
	m_bUpdate = true;
}
void Camera::SetLens(float fov, float width, float height, float nearZ, float farZ)
{
	m_Near = nearZ;
	m_Far = farZ;
	m_ProjMatrix.PerspectiveFovLH(fov, width / height, nearZ, farZ);
	m_bUpdate = true;
}
const JMatrix4x4& Camera::GetMatrix() const
{
	UpdateMatrix();
	return m_ViewProjMatrix;
}
const Common::JMatrix4x4& Camera::GetViewMatrix() const
{
	UpdateMatrix();
	return m_ViewMatrix;
}
const Common::JMatrix4x4& Camera::GetProjMatrix() const
{
	UpdateMatrix();
	return m_ProjMatrix;
}
JMatrix4x4 Camera::GetHlslMatrix() const
{
	UpdateMatrix();
	JMatrix4x4 result = m_ViewProjMatrix;
	result.Transpose();


	return result;
}
Common::JMatrix4x4 Camera::GetHlslViewMatrix() const
{
	UpdateMatrix();
	JMatrix4x4 result = m_ViewMatrix;
	result.Transpose();
	return result;
}
Common::JMatrix4x4 Camera::GetHlslProjMatrix() const
{
	UpdateMatrix();
	JMatrix4x4 result = m_ProjMatrix;
	result.Transpose();
	return result;
}
void Camera::UpdateMatrix() const
{
	if (!m_bUpdate)
	{
		return;
	}
	m_Up.Set(0.0f, 1.0f, 0.0f);
	m_Target.Set(0.0f, 0.0f, 1.0f);


	m_Target = m_CameraRotator.GetMatrix().TransformVector(m_Target);
	m_Up     = m_CameraRotator.GetMatrix().TransformVector(m_Up);

	m_ViewMatrix.LookAtLH(m_Position, m_Position + m_Target, m_Up);

	m_ViewProjMatrix = m_ViewMatrix * m_ProjMatrix;
	m_bUpdate = false;
}



JVector3 Camera::GetLookVector() const
{
	UpdateMatrix();

	JVector3 target = m_Target + m_Position;
	JVector3 LookVector = Normalize(target - m_Position);
	

	return LookVector;
}
JVector3 Camera::GetRightVector() const
{
	UpdateMatrix();
	return Cross(m_Up, GetLookVector());
}
const Common::JVector3& Camera::GetPosition() const
{
	UpdateMatrix();
	return m_Position;
}

const Common::JVector3& Camera::GetUp() const
{
	UpdateMatrix();
	return m_Up;
}
const Common::JVector3& Camera::GetTarget() const
{
	UpdateMatrix();
	return m_Target;
}