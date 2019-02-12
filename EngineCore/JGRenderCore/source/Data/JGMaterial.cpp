#include"JGMaterial.h"
#include"DxCore/DxDevice.h"
#include"EngineFrameResource.h"
#include"CommonData.h"
using namespace JGRC;
using namespace std;
using namespace DirectX;
JGMaterial::JGMaterial(UINT Index, const MaterialDesc& desc)
{
	m_Data   = make_unique<MaterialData>();
	m_Desc   = make_unique<MaterialDesc>();
	*m_Desc = desc;
	m_MatCBIndex = Index;
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
			if(t->Type == ETextureType::Default)
				idx = CommonData::_ResourceManager()->GetTextureIndex(Data.second);
			else
				idx = CommonData::_ResourceManager()->GetCubeTextureShaderIndex(Data.second);
			if (idx < 0)
				continue;
			m_Data->TextureIndex[(int)Data.first] = idx;
		}
		// 머터리얼 데이터 GPU로 보내기 위해 업로드버퍼에 복사
		XMMATRIX matTransform = XMLoadFloat4x4(&m_Data->MatTransform);
		MaterialData Data = *m_Data;
		XMStoreFloat4x4(&Data.MatTransform, XMMatrixTranspose(matTransform));
		UpdatePerFrame();
		MaterialCB->CopyData(m_MatCBIndex, Data);
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
void JGMaterial::SetTexture(ETextureSlot slot,const std::wstring& TexturePath)
{
	m_TexturePaths[slot] = TexturePath;
	m_TextureDataQue.push(pair<ETextureSlot, wstring>(slot, TexturePath));
	ClearNotify();
}
void JGMaterial::SetMaterialData(const MaterialData& Data)
{
	*m_Data = Data;
	ClearNotify();
}
void JGMaterial::SetDiffuseAlbedo(float r, float g, float b, float a)
{
	m_Data->DiffuseAlbedo = { r,g,b,a };
	ClearNotify();
}
void JGMaterial::SetFresnelR0(float x, float y, float z)
{
	m_Data->FresnelR0 = { x,y,z };
	ClearNotify();
}
void JGMaterial::SetRoughness(float x)
{
	if (x < 0.0f) x = 0.0f;
	if (x > 1.0f) x = 1.0f;
	m_Data->Roughness = x;
	ClearNotify();
}
void JGMaterial::SetMetalic(float x)
{
	if (x < 0.0f) x = 0.0f;
	if (x > 1.0f) x = 1.0f;
	m_Data->Metallic = x;
	ClearNotify();
}
void JGMaterial::SetMatTransform(const DirectX::XMFLOAT4X4& mat)
{
	m_Data->MatTransform = mat;
	ClearNotify();
}
JGMaterial* JGMaterialCreater::CreateMaterial(const MaterialDesc& Desc)
{
	// 머터리얼이 이미 있다면 패쓰
	if (m_Materials.end() != m_Materials.find(Desc.Name))
		return nullptr;

	// 머터리얼 저장
	auto Mat = make_unique<JGMaterial>(++m_MatCBIndex, Desc);
	JGMaterial* result = Mat.get();
	m_Materials[Desc.Name] = move(Mat);

	return result;
}
JGMaterial* JGMaterialCreater::GetMaterial(const string& name)
{
	if (m_Materials.end() == m_Materials.find(name))
		return nullptr;
	return m_Materials[name].get();
}
