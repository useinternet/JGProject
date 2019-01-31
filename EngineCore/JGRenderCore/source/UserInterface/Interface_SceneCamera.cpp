#include"Interface_SceneCamera.h"

using namespace JGRC;
void Interface_SceneCamera::SetLens(float fov, float aspect, float nearz, float farz)
{
	m_Data->SetLens(fov, aspect, nearz, farz);
}
void Interface_SceneCamera::SetPosition(float x, float y, float z)
{
	m_Data->SetPosition(x, y, z);
}
void Interface_SceneCamera::Strafe(float d)
{
	m_Data->Strafe(d);
}
void Interface_SceneCamera::Walk(float d)
{
	m_Data->Walk(d);
}
void Interface_SceneCamera::Up(float d)
{
	m_Data->Up(d);
}
// Rotate the camera.
void Interface_SceneCamera::Pitch(float angle)
{
	m_Data->Pitch(angle);
}
void Interface_SceneCamera::RotateY(float angle)
{
	m_Data->RotateY(angle);
}
void Interface_SceneCamera::Roll(float angle)
{
	m_Data->Roll(angle);
}