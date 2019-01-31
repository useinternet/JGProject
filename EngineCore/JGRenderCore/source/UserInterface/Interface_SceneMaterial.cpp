#include"Interface_SceneMaterial.h"
#include"Interface_Scene.h"
#include"Data/JGMaterial.h"
using namespace JGRC;
using namespace std;
Interface_SceneMaterial::Interface_SceneMaterial(JGMaterial* mat) : m_Data(mat) {}
void Interface_SceneMaterial::SetTexture(ETextureSlot slot, const wstring& TexturePath)
{
	m_Data->SetTexture(slot, TexturePath);
}
void Interface_SceneMaterial::SetDiffuseAlbedo(float r, float g, float b, float a)
{
	m_Data->SetDiffuseAlbedo(r, g, b, a);
}
void Interface_SceneMaterial::SetFresnelR0(float x, float y, float z)
{
	m_Data->SetFresnelR0(x, y, z);
}
void Interface_SceneMaterial::SetRoughness(float x)
{
	m_Data->SetRoughness(x);
}
void Interface_SceneMaterial::SetMetalic(float x)
{
	m_Data->SetMetalic(x);
}
