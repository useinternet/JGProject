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
Interface_SceneMaterialData Interface_SceneMaterial::AddData(const string& name)
{
	return Interface_SceneMaterialData(m_Data->AddData(name));
}
Interface_SceneMaterialData::Interface_SceneMaterialData(MatPersonalData* data)
{
	m_Data = data;
}
void Interface_SceneMaterialData::SetDiffuseAlbedo(float r, float g, float b, float a)
{
	m_Data->DiffuseAlbedo = { r, g, b, a };
}
void Interface_SceneMaterialData::SetFresnelR0(float x, float y, float z)
{
	m_Data->FresnelR0 = { x, y, z };
}
void Interface_SceneMaterialData::SetRoughness(float x)
{
	m_Data->Roughness = x; 
}
void Interface_SceneMaterialData::SetMetalic(float x)
{
	m_Data->Metallic = x;
}
const std::string& Interface_SceneMaterialData::GetName() const
{
	return m_Data->Name;
}
