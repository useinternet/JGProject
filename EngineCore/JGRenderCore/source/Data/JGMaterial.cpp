#include"JGMaterial.h"
#include"DxCore/DxDevice.h"
#include"EngineFrameResource.h"
#include"ResourceManagement/DataManager.h"
#include"CommonData.h"
using namespace JGRC;
using namespace std;
using namespace DirectX;
JGMaterial::JGMaterial(const MaterialDesc& desc)
{
	m_Desc   = make_unique<MaterialDesc>();
	*m_Desc = desc;
}
void JGMaterial::Update(class FrameResource* CurrentFrameResource)
{
	auto MaterialCB = CurrentFrameResource->MaterialCB.get();
	if (IsCanUpdate())
	{
		// 텍스쳐 업데이트(지연된 업데이트)

		while (!m_TextureDataQue.empty())
		{
			auto Data = m_TextureDataQue.front();
			m_TextureDataQue.pop();
			UINT idx;
			TexturePack* t = CommonData::_ResourceManager()->GetTexturePack(Data.second);
			if (t->Type == ETextureType::Default)
				idx = CommonData::_ResourceManager()->GetTextureIndex(Data.second);
			else
				idx = CommonData::_ResourceManager()->GetCubeTextureShaderIndex(Data.second);
			if (idx < 0)
				continue;
			m_ShareData.TextureIndex[(int)Data.first] = idx;
		}
		UpdatePerFrame();
	}
	for (auto& data : m_PersonalData)
	{
		if (data.second->IsCanUpdate())
		{
			MaterialConstantData MatData;
			XMMATRIX matTransform = XMLoadFloat4x4(&data.second->MatTransform);
			XMStoreFloat4x4(&data.second->MatTransform, matTransform);


			MatData.DiffuseAlbedo = data.second->DiffuseAlbedo;
			MatData.FresnelR0 = data.second->FresnelR0;
			MatData.Metallic = data.second->Metallic;
			MatData.Roughness = data.second->Roughness;
			for (UINT i = 0; i < 8; ++i)
			{
				MatData.TextureIndex[i] = m_ShareData.TextureIndex[i];
			}
			MaterialCB->CopyData(data.second->CBIndex, MatData);
			data.second->UpdatePerFrame();
		}
	}
}
EShaderFlag JGMaterial::GetFlag()
{
	EShaderFlag result = Shader_Flag_None;
	if (m_TexturePaths.find(ETextureSlot::Diffuse) != m_TexturePaths.end())
	{
		result = result | Shader_Flag_Use_DiffuseMap;
	}
	if (m_TexturePaths.find(ETextureSlot::Normal) != m_TexturePaths.end())
	{
		result = result | Shader_Flag_Use_NormalMap;
	}
	return result;
}
MatPersonalData* JGMaterial::AddData(const string& name)
{
	if (m_PersonalData.find(name) != m_PersonalData.end())
		return nullptr;
	MatPersonalData* data = CommonData::_DataManager()->CreateMaterialData(name);
	m_PersonalData[name] = data;
	return data;
}
MatPersonalData* JGMaterial::GetData(const string& name)
{
	if (m_PersonalData.find(name) == m_PersonalData.end())
		return nullptr;
	return m_PersonalData[name];
}
void JGMaterial::SetTexture(ETextureSlot slot,const std::wstring& TexturePath)
{
	m_TexturePaths[slot] = TexturePath;
	m_TextureDataQue.push(pair<ETextureSlot, wstring>(slot, TexturePath));
	ClearNotify();
}
