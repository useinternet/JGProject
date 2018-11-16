#include"SRSLightPass.h"
#include"DirectX/DirectX.h"
#include"MaterialSystem/Shader/JGShaderArray.h"
#include"MaterialSystem/Shader/ShaderWriter.h"
#include"MaterialSystem/Shader/ShaderReader.h"
#include"MaterialSystem/Mesh/Mesh.h"
using namespace JGRC;
using namespace std;


SRSLightPass::SRSLightPass()
{
	m_Dx = DirectX::GetInstance();
	m_Scene = make_unique<SRSScene>();
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
	m_Shader->Get(EShaderType::Pixel)->AddTexture(SRSRT->GetShaderResourceView(ERTType::Albedo_pad));
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
	m_Shader->Get(EShaderType::Pixel)->SetTexture(2, m_RenderTarget->GetShaderResourceView(ERTType::Albedo_pad));
	m_Shader->Get(EShaderType::Pixel)->SetTexture(3, m_RenderTarget->GetShaderResourceView(ERTType::SpecColor_pad));
	DirectX::GetInstance()->SetDirectState(EStateType::DepthState, (uint)EDepthStateType::ZBufferOff);
	m_Scene->Render();
	m_Shader->Render(m_Scene->GetIndexCount());
	DirectX::GetInstance()->SetDirectState(EStateType::DepthState, (uint)EDepthStateType::ZBufferOn);
	// ¼ÎÀÌ´õ ·»´õ¸µ
	ID3D11ShaderResourceView* null[4] = { nullptr, nullptr, nullptr, nullptr };
	DirectX::GetInstance()->GetContext()->PSSetShaderResources(0, 4, null);
}
void SRSLightPass::InitHlsl(const DxWinConfig& config)
{
	HlslEditor* VertexEdit = m_Dx->CreateObject<HlslEditor>();
	VertexEdit->RegisterHlslPath(EShaderType::Vertex, Game::path / "Engine/Shader/HLSL/LightPass_vs.hlsl");
	HlslEditor* PixelEdit = m_Dx->CreateObject<HlslEditor>();
	PixelEdit->RegisterHlslPath(EShaderType::Pixel, Game::path / "Engine/Shader/HLSL/LightPass_ps.hlsl");


	// ¹öÅØ½º ¼ÎÀÌ´õ
	InputLayout* vsIL = VertexEdit->CreateInputLayout();
	vsIL->AddDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	vsIL->AddDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0);
	CBuffer* Cbf = VertexEdit->CreateCBuffer();
	Cbf->SetName("MatrixBuffer");
	Cbf->AddVar("wvpMatrix",0,15);
	// ÇÈ¼¿ ¼ÎÀÌ´õ
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

	ShaderReader reader(m_hWnd);


	jgMatrix4x4 worldMatrix;
	worldMatrix.identity();
	jgMatrix4x4 viewMatrix;
	viewMatrix.lookAtLH(jgVec3(0.0f, 0.0f, -5.0f), jgVec3(0.0f, 0.0f, 1.0f), jgVec3(0.0f, 1.0f, 0.0f));
	jgMatrix4x4 projectionMatrix;

	projectionMatrix.orthoLH(config.ScreenWidth, config.ScreenHeight, config.NearZ, config.FarZ);
	worldMatrix.transpose();
	viewMatrix.transpose();
	projectionMatrix.transpose();


	jgMatrix4x4 wvpMatrix = m_Scene->GetwvpMatrix();
	m_Shader = reader.ReadShader(Game::path / "Engine/Shader/Shader/LightPass.shader");
	m_Shader->Get(EShaderType::Vertex)->SetParam("wvpMatrix", &wvpMatrix, 16);


	m_Dx->DeleteObject(VertexEdit);
	m_Dx->DeleteObject(PixelEdit);
}