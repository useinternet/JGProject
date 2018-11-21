#include"SRSLightPass.h"
#include"DirectX/DirectX.h"
#include"MaterialSystem/Shader/JGShaderArray.h"
#include"MaterialSystem/Shader/ShaderWriter.h"
#include"MaterialSystem/Shader/ShaderReader.h"
#include"MaterialSystem/Mesh/Mesh.h"

//

using namespace JGRC;
using namespace std;


SRSLightPass::SRSLightPass()
{
	m_Dx = DirectX::GetInstance();
	m_Scene = make_unique<SRSScene>();
	m_DirectionLight = make_unique<DirectionLight>();
	for (uint i = 0; i < m_MaxPointLightCount; ++i)
	{
		PointLight l;
		m_PointLightArray.push_back(l);
	}
	for (uint i = 0; i < m_MaxSpotLightCount; ++i)
	{
		SpotLight l;
		m_SpotLightArray.push_back(l);
	}
}
SRSLightPass::~SRSLightPass()
{

}
void SRSLightPass::Init(const DxWinConfig& config, SRSRenderTarget* SRSRT)
{
	if (SRSRT == nullptr)
	{
		JGLOG(log_Error, "JGRC::SRSLightPass", "SRSRenderTarget is nullptr");
		return;
	}
	m_RenderTarget = SRSRT;
	m_hWnd = config.hWnd;
	m_Scene->CreateScene(config);
	InitHlsl(config);
	m_Shader->Get(EShaderType::Pixel)->AddTexture(SRSRT->GetShaderResourceView(ERTType::Pos_Depth));
	m_Shader->Get(EShaderType::Pixel)->AddTexture(SRSRT->GetShaderResourceView(ERTType::Normal_SpecPw));
	m_Shader->Get(EShaderType::Pixel)->AddTexture(SRSRT->GetShaderResourceView(ERTType::Albedo_SpecIts));
	m_Shader->Get(EShaderType::Pixel)->AddTexture(SRSRT->GetShaderResourceView(ERTType::SpecColor_pad));
}
void SRSLightPass::Render()
{
	if (m_RenderTarget == nullptr)
	{
		JGLOG(log_Error, "JGRC::SRSLightPass", "SRSRenderTarget is nullptr");
		return;
	}
	m_Shader->Get(EShaderType::Pixel)->SetTexture(0, m_RenderTarget->GetShaderResourceView(ERTType::Pos_Depth));
	m_Shader->Get(EShaderType::Pixel)->SetTexture(1, m_RenderTarget->GetShaderResourceView(ERTType::Normal_SpecPw));
	m_Shader->Get(EShaderType::Pixel)->SetTexture(2, m_RenderTarget->GetShaderResourceView(ERTType::Albedo_SpecIts));
	m_Shader->Get(EShaderType::Pixel)->SetTexture(3, m_RenderTarget->GetShaderResourceView(ERTType::SpecColor_pad));
	DirectX::GetInstance()->SetDirectState(EStateType::DepthState, (uint)EDepthStateType::ZBufferOff);

	if (m_Camera)
	{
		m_Shader->Get(EShaderType::Vertex)->SetParam("CameraDir", m_Camera->GetEyeData(), 3);
	}
	else
	{
		jgVec3 temp(1.0f, 1.0f, 1.0f);
		m_Shader->Get(EShaderType::Vertex)->SetParam("CameraDir", &temp, 3);
		JGLOG(log_Warning, "JGRC::SRSLightPass", "Camera is nullptr in SRSLightPass");
	}

	// ÀÓ½Ã
	m_Shader->Get(EShaderType::Pixel)->SetParam("DirLight", m_DirectionLight.get(), 16);
	m_Shader->Get(EShaderType::Pixel)->SetParam("PntLight", &m_PointLightArray[0], 8 * m_MaxPointLightCount);
	m_Shader->Get(EShaderType::Pixel)->SetParam("SptLight", &m_SpotLightArray[0], 12 * m_MaxSpotLightCount);
	m_Shader->Get(EShaderType::Pixel)->SetParam("DirectionLightCount", &m_DirectionLightCount, 1);
	m_Shader->Get(EShaderType::Pixel)->SetParam("PointLightCount", &m_PointLightCount, 1);
	m_Shader->Get(EShaderType::Pixel)->SetParam("SpotLightCount", &m_SpotLightCount, 1);

	m_Scene->Render();
	m_Shader->Render(m_Scene->GetIndexCount());
	DirectX::GetInstance()->SetDirectState(EStateType::DepthState, (uint)EDepthStateType::ZBufferOn);
	// ¼ÎÀÌ´õ ·»´õ¸µ
	ID3D11ShaderResourceView* null[4] = { nullptr, nullptr, nullptr, nullptr };
	DirectX::GetInstance()->GetContext()->PSSetShaderResources(0, 4, null);
}
DirectionLight* SRSLightPass::AddDirectionLight()
{
	if (m_DirectionLightCount > 1)
	{
		JGLOG(log_Error, "JGRC::SRSLightPass", "DirectionLight is over 1 count")
		return nullptr;
	}
	m_DirectionLightCount++;
	return m_DirectionLight.get();
}
PointLight* SRSLightPass::AddPointLight()
{
	if (m_PointLightCount >= m_MaxPointLightCount)
	{
		JGLOG(log_Error,"JGRC::SRSLightPass","Current PointLightCount is over MaxPointLightCount")
		return  nullptr;
	}
	return &m_PointLightArray[m_PointLightCount++];
}
SpotLight*  SRSLightPass::AddSpotLight()
{
	if (m_SpotLightCount >= m_MaxSpotLightCount)
	{
		JGLOG(log_Error, "JGRC::SRSLightPass", "Current SpotLightCount is over MaxSpotLightCount")
			return  nullptr;
	}
	return &m_SpotLightArray[m_SpotLightCount++];
}
void   SRSLightPass::DeletePointLight(PointLight* light)
{
	bool find = false;
	for (int i = 0; i < m_PointLightCount; ++i)
	{
		if (&m_PointLightArray[i] == light)
		{
			find = true;
		}
		if (find && i < m_MaxPointLightCount - 1)
		{
			m_PointLightArray[i] = m_PointLightArray[i + 1];
		}
	}
	m_PointLightCount--;
}
void   SRSLightPass::DeleteSpotLight(SpotLight* light)
{
	bool find = false;
	for (int i = 0; i < m_SpotLightCount; ++i)
	{
		if (&m_SpotLightArray[i] == light)
		{
			find = true;
		}
		if (find && i < m_MaxSpotLightCount - 1)
		{
			m_SpotLightArray[i] = m_SpotLightArray[i + 1];
		}
	}
	m_SpotLightCount--;
}
void SRSLightPass::BindingCamera(Camera* cam)
{
	m_Camera = cam;
}
void SRSLightPass::InitHlsl(const DxWinConfig& config)
{
	// ¼ÎÀÌ´õ ¾²±â
	HlslEditor* VertexEdit = m_Dx->CreateObject<HlslEditor>();
	VertexEdit->RegisterHlslPath(EShaderType::Vertex, Game::path / "Engine/Shader/HLSL/LightPass_vs.hlsl");
	HlslEditor* PixelEdit = m_Dx->CreateObject<HlslEditor>();
	PixelEdit->RegisterHlslPath(EShaderType::Pixel, Game::path / "Engine/Shader/HLSL/LightPass_ps.hlsl");


	// ¹öÅØ½º ¼ÎÀÌ´õ
	InputLayout* vsIL = VertexEdit->CreateInputLayout();
	vsIL->AddDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	vsIL->AddDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0);
	CBuffer* Cbf = VertexEdit->CreateCBuffer();
	Cbf->SetName("PassBuffer");
	Cbf->AddVar("wvpMatrix",0,15);
	Cbf->AddVar("CameraDir", 16, 19);
	// ÇÈ¼¿ ¼ÎÀÌ´õ
	CBuffer* pCbf = PixelEdit->CreateCBuffer();
	pCbf->SetName("LightBuffer");
	unsigned int PntLightSize = 8  * (unsigned int)m_MaxPointLightCount;
	unsigned int SptLightSize = 12 * (unsigned int)m_MaxSpotLightCount;
	// ÀÓ½Ã
	pCbf->AddVar("DirLight", 0, 15);
	pCbf->AddVar("PntLight", 16, 16 + PntLightSize - 1); 
	pCbf->AddVar("SptLight", 16 + PntLightSize, 16 + PntLightSize + SptLightSize - 1);

	unsigned int CountStart = 16 + PntLightSize + SptLightSize;

	pCbf->AddVar("DirectionLightCount", CountStart, CountStart);
	pCbf->AddVar("PointLightCount", CountStart + 1, CountStart + 1);
	pCbf->AddVar("SpotLightCount", CountStart + 2, CountStart + 2);
	pCbf->AddVar("TempCount2", CountStart + 3, CountStart + 3);

	Texture2D* texture = PixelEdit->CreateTexture2D();
	texture->Add("T_Pos_Depth");
	texture->Add("T_Normal_SpecPow");
	texture->Add("T_Albedo");
	texture->Add("T_Specular");

	SamplerState* sampler = PixelEdit->CreateSamplerState();
	sampler->AddDefaultClampMode();


	ShaderWriter writer;
	writer.AddEditor(VertexEdit); writer.AddEditor(PixelEdit);
	writer.Write(Game::path / "Engine/Shader/Shader/LightPass");


	// ¼ÎÀÌ´õ ÀÐ±â
	ShaderReader reader(m_hWnd);


	jgMatrix4x4 worldMatrix;
	worldMatrix.identity();
	jgMatrix4x4 viewMatrix;
	viewMatrix.lookAtLH(jgVec3(0.0f, 0.0f, -5.0f), jgVec3(0.0f, 0.0f, 1.0f), jgVec3(0.0f, 1.0f, 0.0f));
	jgMatrix4x4 projectionMatrix;

	projectionMatrix.orthoLH((real)config.ScreenWidth, (real)config.ScreenHeight, config.NearZ, config.FarZ);
	worldMatrix.transpose();
	viewMatrix.transpose();
	projectionMatrix.transpose();


	jgMatrix4x4 wvpMatrix = m_Scene->GetwvpMatrix();
	m_Shader = reader.ReadShader(Game::path / "Engine/Shader/Shader/LightPass.shader");
	m_Shader->Get(EShaderType::Vertex)->SetParam("wvpMatrix", &wvpMatrix, 16);


	m_Dx->DeleteObject(VertexEdit);
	m_Dx->DeleteObject(PixelEdit);


	// ¶óÀÌÆ® ¼ÂÆÃ
	
	m_Shader->Get(EShaderType::Pixel)->SetParam("TempCount2", &m_TempCount2, 1);
}