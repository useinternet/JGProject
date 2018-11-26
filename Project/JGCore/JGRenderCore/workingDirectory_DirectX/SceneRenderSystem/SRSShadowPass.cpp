#include"SRSShadowPass.h"
#include"DirectX/DirectX.h"
#include"LightPassLightManager.h"
#include"MaterialSystem/Shader/JGShaderArray.h"
#include"MaterialSystem/Shader/ShaderWriter.h"
#include"MaterialSystem/Shader/ShaderReader.h"
#include"MaterialSystem/Mesh/Mesh.h"
#include"Camera/Camera.h"
#include"DirectX/JGViewport.h"
using namespace JGRC;
using namespace std;
class Light;
SRSShadowPass::SRSShadowPass()
{
	m_Dx = DirectX::GetInstance();
	m_RenderTarget = make_unique<SRSRenderTarget>();
	m_Scene        = make_unique<SRSScene>();
}
SRSShadowPass::~SRSShadowPass()
{

}
void SRSShadowPass::ShadowMapSetting(const real width, const real height, const real nearZ, const real farZ)
{
	Light::ShadowWidth = width;
	Light::ShadowHeight = height;
	Light::FarZ  = farZ;
	Light::NearZ = nearZ;
}
void SRSShadowPass::ShadowMapResolutionSetting(const real width, const real height)
{
	Light::ShadowWidth = width;
	Light::ShadowHeight = height;
}
void SRSShadowPass::ShadowMapDepthSetting(const real nearZ, const real farZ)
{
	Light::FarZ = farZ;
	Light::NearZ = nearZ;
}
void SRSShadowPass::Init(const DxWinConfig& config, SRSRenderTarget* MainRT, LightPassLightManager* LM)
{
	m_pMainRenderTarget = MainRT;
	m_LightPassLM = LM;
	ShadowMapSetting(10, 10, 1.0f, 50.0f);
	bool result = true;
	result = m_RenderTarget->CreateSRSRenderTarget(1024, 1024, 1);
	if (!result)
	{
		JGLOG(log_Error, "JGRC::SRSShadowPass", "Failed Create SRSRenderTarget in ShadowPass");
		return;
	}
	m_Scene->CreateScene(1024, 1024,1.0f, 50.0f);
	OutputHlsl();
	ReadHlsl(config);

	m_DepthShader->Get(EShaderType::Pixel)->AddTexture(nullptr);
	m_DepthShader->Get(EShaderType::Pixel)->AddTexture(nullptr);
}
void SRSShadowPass::Render(Camera* cam, JGViewport* viewport)
{
	if (m_RenderTarget == nullptr)
	{
		JGLOG(log_Error, "JGRC::SRSShadowPass", "SRSRenderTarget is nullptr");
		return;
	}
	m_RenderTarget->BindingRenderTarget(0);
	m_RenderTarget->ClearRenderTarget(0,0.0f,0.0f,0.0f,0.0f);


	

	m_DepthShader->Get(EShaderType::Pixel)->SetTexture(0, m_pMainRenderTarget->GetShaderResourceView(0));
	m_DepthShader->Get(EShaderType::Pixel)->SetTexture(1, m_pMainRenderTarget->GetShaderResourceView(4));


	jgMatrix4x4 InvProj = viewport->GetProjectionMatrix();
	InvProj.transpose();
	InvProj.inverse();

	

	//m_DepthShader->Get(EShaderType::Pixel)->SetParam("LightViewProjMatrix", &ViewProj, 16);
	//m_DepthShader->Get(EShaderType::Pixel)->SetParam("InvCameraMatrix", &InvView, 16);
	//m_DepthShader->Get(EShaderType::Pixel)->SetParam("InvProjMatrix", &InvProj, 16);
	DirectX::GetInstance()->SetDirectState(EStateType::DepthState, (uint)EDepthStateType::ZBufferOff);
	m_Scene->Render();
	m_DepthShader->Render(m_Scene->GetIndexCount());
	DirectX::GetInstance()->SetDirectState(EStateType::DepthState, (uint)EDepthStateType::ZBufferOn);
}
ID3D11ShaderResourceView* SRSShadowPass::GetShadowMap()
{
	return m_RenderTarget->GetShaderResourceView(0);
}
void SRSShadowPass::OutputHlsl()
{
	// Vertex
	HlslEditor* VertexEditor = m_Dx->CreateObject<HlslEditor>();
	VertexEditor->RegisterHlslPath(EShaderType::Vertex, Game::path / "Engine/Shader/HLSL/LightShadowPass_vs.hlsl");
	CBuffer* vCbf = VertexEditor->CreateCBuffer();
	vCbf->SetName("PassBuffer");
	vCbf->AddVar("wvpMatrix", 0, 15);
	InputLayout* IL = VertexEditor->CreateInputLayout();
	IL->AddDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	IL->AddDesc("TEXCOORD",0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0);


	// Pixel
	HlslEditor* PixelEditor = m_Dx->CreateObject<HlslEditor>();
	PixelEditor->RegisterHlslPath(EShaderType::Pixel, Game::path / "Engine/Shader/HLSL/LightShadowPass_ps.hlsl");
	CBuffer* pCbf = PixelEditor->CreateCBuffer();
	pCbf->SetName("LightMatrixBuffer");
	pCbf->AddVar("LightViewProjMatrix", 0, 15);
	pCbf->AddVar("InvCameraMatrix", 16, 31);
	pCbf->AddVar("InvProjMatrix", 32, 47);
	Texture2D* t123 = PixelEditor->CreateTexture2D();
	t123->Add("T_Pos_Depth");
	t123->Add("T_ShadowTexture");
	SamplerState* ss22 = PixelEditor->CreateSamplerState();
	ss22->AddDefaultClampMode();
	D3D11_SAMPLER_DESC Desc = {
		D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_WRAP,
		0.0f,
		1u,
		D3D11_COMPARISON_ALWAYS,
	   { 0.0f, 0.0f, 0.0f, 0.0f },
	    0.0f,
		D3D11_FLOAT32_MAX
	};
	ss22->AddCustomMode(Desc);
	ShaderWriter writer;
	writer.AddEditor(VertexEditor);
	writer.AddEditor(PixelEditor);
	if (!writer.Write(Game::path / "Engine/Shader/Shader/ShadowDepthMap"))
	{
		JGLOG(log_Error,"JGRC::SRSShadowPass","Failed Write ShadowDepthMap.shader ")
		return;
	}

}
void SRSShadowPass::ReadHlsl(const DxWinConfig& config)
{
	ShaderReader reader(config.hWnd);
	jgMatrix4x4 matrix = m_Scene->GetwvpMatrix();
	m_DepthShader = reader.ReadShader(Game::path / "Engine/Shader/Shader/ShadowDepthMap.shader");
	m_DepthShader->Get(EShaderType::Vertex)->SetParam("wvpMatrix", &matrix, 16);
}