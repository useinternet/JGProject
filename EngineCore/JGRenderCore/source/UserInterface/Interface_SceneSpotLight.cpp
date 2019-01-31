#include"Interface_SceneSpotLight.h"
#include"Interface_Scene.h"
#include"Data/JGLight.h"
using namespace JGRC;


Interface_SceneSpotLight::Interface_SceneSpotLight(JGLight* l) : m_Data(l) {}


void Interface_SceneSpotLight::SetLightColor(float r, float g, float b)
{
	m_Data->SetLightColor(r, g, b);
}
void Interface_SceneSpotLight::SetDirection(float x, float y, float z)
{
	m_Data->SetDirection(x, y, z);
}
void Interface_SceneSpotLight::SetLocation(float x, float y, float z)
{
	m_Data->SetLocation(x, y, z);
}
void Interface_SceneSpotLight::SetFalloffStart(float x)
{
	m_Data->SetFalloffStart(x);
}
void Interface_SceneSpotLight::SetFalloffEnd(float x)
{
	m_Data->SetFalloffEnd(x);
}
void Interface_SceneSpotLight::SetOuterAngle(float angle)
{
	m_Data->SetOuterAngle(angle);
}
void Interface_SceneSpotLight::SetInnerAngle(float angle)
{
	m_Data->SetInnerAngle(angle);
}
void Interface_SceneSpotLight::SetRotation(float pitch, float yaw, float roll)
{
	m_Data->SetRotation(pitch, yaw, roll);
}

void Interface_SceneSpotLight::OffsetLightColor(float r, float g, float b)
{
	m_Data->OffsetLightColor(r, g, b);
}
void Interface_SceneSpotLight::OffsetDirection(float x, float y, float z)
{
	m_Data->OffsetDirection(x, y, z);
}
void Interface_SceneSpotLight::OffsetLocation(float x, float y, float z)
{
	m_Data->OffsetLocation(x, y, z);
}
void Interface_SceneSpotLight::OffsetFalloffStart(float x)
{
	m_Data->OffsetFalloffStart(x);
}
void Interface_SceneSpotLight::OffsetFalloffEnd(float x)
{
	m_Data->OffsetFalloffEnd(x);
}
void Interface_SceneSpotLight::OffsetOuterAngle(float angle)
{
	m_Data->OffsetOuterAngle(angle);
}
void Interface_SceneSpotLight::OffsetInnerAngle(float angle)
{
	m_Data->OffsetInnerAngle(angle);
}
void Interface_SceneSpotLight::OffsetRotation(float pitch, float yaw, float roll)
{
	m_Data->OffsetRotation(pitch, yaw, roll);
}