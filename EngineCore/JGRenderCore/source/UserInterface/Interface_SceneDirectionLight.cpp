#include"Interface_SceneDirectionLight.h"
#include"Interface_Scene.h"
#include"Data/JGLight.h"

using namespace JGRC;

Interface_SceneDirectionLight::Interface_SceneDirectionLight(JGLight* l) : m_Data(l) {}

void Interface_SceneDirectionLight::SetLightColor(float r, float g, float b)
{
	m_Data->SetLightColor(r, g, b);
}
void Interface_SceneDirectionLight::SetDirection(float x, float y, float z)
{
	m_Data->SetDirection(x, y, z);
}
void Interface_SceneDirectionLight::SetRotation(float pitch, float yaw, float roll)
{
	m_Data->SetRotation(pitch, yaw, roll);
}
void Interface_SceneDirectionLight::OffsetLightColor(float r, float g, float b)
{
	m_Data->OffsetLightColor(r, g, b);
}
void Interface_SceneDirectionLight::OffsetDirection(float x, float y, float z)
{
	m_Data->OffsetDirection(x, y, z);
}
void Interface_SceneDirectionLight::OffsetRotation(float pitch, float yaw, float roll)
{
	m_Data->OffsetRotation(pitch, yaw, roll);
}