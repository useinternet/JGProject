#include"Interface_ScenePointLight.h"
#include"Interface_Scene.h"
#include"Data/JGLight.h"
using namespace JGRC;
#include"Data/JGLight.h"
Interface_ScenePointLight::Interface_ScenePointLight(JGLight* l) : m_Data(l) {}

void Interface_ScenePointLight::SetLightColor(float r, float g, float b)
{
	m_Data->SetLightColor(r, g, b);
}
void Interface_ScenePointLight::SetLocation(float x, float y, float z)
{
	m_Data->SetLocation(x, y, z);
}
void Interface_ScenePointLight::SetFalloffStart(float x)
{
	m_Data->SetFalloffStart(x);
}
void Interface_ScenePointLight::SetFalloffEnd(float x)
{
	m_Data->SetFalloffEnd(x);
}


void Interface_ScenePointLight::OffsetLightColor(float r, float g, float b)
{
	m_Data->OffsetLightColor(r, g, b);
}
void Interface_ScenePointLight::OffsetLocation(float x, float y, float z)
{
	m_Data->OffsetLocation(x, y, z);
}
void Interface_ScenePointLight::OffsetFalloffStart(float x)
{
	m_Data->OffsetFalloffStart(x);
}

void Interface_ScenePointLight::OffsetFalloffEnd(float x)
{
	m_Data->OffsetFalloffEnd(x);
}