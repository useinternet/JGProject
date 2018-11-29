#include"Camera.h"
using namespace JGRC;
using namespace std;

Camera::Camera()
{
	m_Eye.set(0.0f, 0.0f, -5.0f);
	m_Up.set(0.0f, 1.0f, 0.0f);
	m_LookAt.set(0.0f, 0.0f, 1.0f);
	m_viewMatrix = make_unique<jgMatrix4x4>();
	CreateViewMatrix();
}
void Camera::SetEye(const real x, const real y, const real z)
{
	m_Eye.set(x, y, z);
	CreateViewMatrix();
}

void Camera::CreateViewMatrix()
{
	m_viewMatrix->lookAtLH(m_Eye, m_LookAt, m_Up);
}
const jgMatrix4x4& Camera::GetViewMatrix() const
{
	return (*m_viewMatrix);
}
