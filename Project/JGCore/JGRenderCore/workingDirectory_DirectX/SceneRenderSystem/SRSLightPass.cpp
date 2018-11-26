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
	m_LightManager = make_unique<LightPassLightManager>();
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
	for (uint i = 0; i < SRSRT->GetBufferCount(); ++i)
	{
		m_Shader->Get(EShaderType::Pixel)->AddTexture(SRSRT->GetShaderResourceView(i));
	}
	m_Shader->Get(EShaderType::Pixel)->AddTexture(nullptr);
	m_LightManager->InitManager(m_Shader);
}
void SRSLightPass::Render()
{
	if (m_RenderTarget == nullptr)
	{
		JGLOG(log_Error, "JGRC::SRSLightPass", "SRSRenderTarget is nullptr");
		return;
	}
	for (uint i = 0; i < m_RenderTarget->GetBufferCount(); ++i)
	{
		m_Shader->Get(EShaderType::Pixel)->SetTexture(i, m_RenderTarget->GetShaderResourceView(i));
	}
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
	m_LightManager->ParamSetting();

	m_Scene->Render();
	m_Shader->Render(m_Scene->GetIndexCount());
	DirectX::GetInstance()->SetDirectState(EStateType::DepthState, (uint)EDepthStateType::ZBufferOn);
	// ¼ÎÀÌ´õ ·»´õ¸µ
	ID3D11ShaderResourceView* null[5] = { nullptr, nullptr, nullptr, nullptr ,nullptr};
	DirectX::GetInstance()->GetContext()->PSSetShaderResources(0, 5, null);
}
DirectionLight* SRSLightPass::GetDirectionLight()
{
	return m_LightManager->GetDirectionLight();
}
DirectionLight* SRSLightPass::AddDirectionLight()
{
	return m_LightManager->AddDirectionLight();
}
PointLight* SRSLightPass::AddPointLight()
{
	return m_LightManager->AddPointLight();
}
SpotLight*  SRSLightPass::AddSpotLight()
{
	return m_LightManager->AddSpotLight();
}
void   SRSLightPass::DeletePointLight(PointLight* light)
{
	m_LightManager->DeletePointLight(light);
}
void   SRSLightPass::DeleteSpotLight(SpotLight* light)
{
	m_LightManager->DeleteSpotLight(light);
}
void SRSLightPass::BindingCamera(Camera* cam)
{
	m_Camera = cam;
}
bool SRSLightPass::OutputHlsl()
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
	Cbf->AddVar("wvpMatrix", 0, 15);
	Cbf->AddVar("CameraDir", 16, 19);
	// ÇÈ¼¿ ¼ÎÀÌ´õ
	CBuffer* pCbf = PixelEdit->CreateCBuffer();
	pCbf->SetName("LightBuffer");
	unsigned int start = 0;
	for (unsigned int i = 0; i < m_LightManager->GetMaxDirectionLightCount(); ++i)
	{
		pCbf->AddVar("DirLight", start, start + DirectionLight::ParamCount - 1);
		start += DirectionLight::ParamCount;
	}
	for (unsigned int i = 0; i < m_LightManager->GetMaxPointLightCount(); ++i)
	{
		pCbf->AddVar("PntLight" + to_string((int)i), start, start + PointLight::ParamCount - 1);
		start += PointLight::ParamCount;
	}
	for (unsigned int i = 0; i < m_LightManager->GetMaxSpotLightCount(); ++i)
	{
		pCbf->AddVar("SptLight" + to_string((int)i), start, start + SpotLight::ParamCount - 1);
		start += SpotLight::ParamCount;
	}
	pCbf->AddVar("DirectionLightCount", start, start);
	pCbf->AddVar("PointLightCount", start + 1, start + 1);
	pCbf->AddVar("SpotLightCount", start + 2, start + 2);
	pCbf->AddVar("TempCount2", start + 3, start + 3);

	Texture2D* texture = PixelEdit->CreateTexture2D();
	texture->Add("T_Pos_Depth");
	texture->Add("T_Normal_SpecPow");
	texture->Add("T_Albedo");
	texture->Add("T_Specular");

	SamplerState* sampler = PixelEdit->CreateSamplerState();
	sampler->AddDefaultClampMode();


	ShaderWriter writer;
	writer.AddEditor(VertexEdit); writer.AddEditor(PixelEdit);
	if (!writer.Write(Game::path / "Engine/Shader/Shader/LightPass"))
	{
		JGLOG(log_Error, "JGRC::SRSLightPass", "Failed ShaderWrite in LightPass");
		return false;
	}

	m_Dx->DeleteObject(VertexEdit);
	m_Dx->DeleteObject(PixelEdit);

	return true;
}
void SRSLightPass::InitHlsl(const DxWinConfig& config)
{
	OutputHlsl();

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
}