#include"JGMaterial.h"
#include"DxCore/DxCore.h"
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
	m_Data->Roughness = x;
	ClearNotify();
}
void JGMaterial::SetRefractive(float x)
{
	m_Data->Refractive = x;
	ClearNotify();
}
void JGMaterial::SetReflectivity(float x)
{
	m_Data->Reflectivity = x;
	ClearNotify();
}
void JGMaterial::SetMatTransform(const DirectX::XMFLOAT4X4& mat)
{
	m_Data->MatTransform = mat;
	ClearNotify();
}
void JGMaterial::SetPSO(ID3D12PipelineState* PSO)
{
	m_PSO = PSO;
}
vector<ShaderMacroPack> JGMaterial::CreateMacroPack()
{
	vector<ShaderMacroPack> v;
	if (m_Desc->Mode == EPSOMode::INSTANCE)
	{
		ShaderMacroPack Pack;
		Pack.Macro = SHADER_MACRO_DEFINE_SHADER_INSTANCE_OBJECT;
		Pack.Value = "1";
		v.push_back(Pack);
	}

	return move(v);
}
void JGMaterialCreater::BuildMaterial(CommonShaderRootSignature* CommonRootSig)
{
	// 셰이더 컴파일 및 머터리얼 PSO 등록
	for (auto& mat : m_Materials)
	{
		MaterialDesc* desc = mat.second->GetDesc();
		vector<ShaderMacroPack> macro = mat.second->CreateMacroPack();

		mat.second->SetPSO(m_Shaders[desc->ShaderPath]->CompileAndConstrutPSO(
			desc->Mode,
			CommonRootSig,
			macro));
	}
}
JGMaterial* JGMaterialCreater::CreateMaterial(const MaterialDesc& Desc)
{
	// 머터리얼이 이미 있다면 패쓰
	if (m_Materials.end() != m_Materials.find(Desc.Name))
		return nullptr;
	// 처음 등록 하는 셰이더라면 셰이더 배열에 저장
	if (m_Shaders.end() == m_Shaders.find(Desc.ShaderPath))
	{
		auto shader = make_unique<Shader>();
		shader->Init(
			Desc.ShaderPath,
			{ EShaderType::Vertex, EShaderType::Pixel });
		m_Shaders[Desc.ShaderPath] = move(shader);
	}
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
