#include"SRSLightPass.h"
#include"DirectX/DirectX.h"
#include"MaterialSystem/Shader/JGShaderArray.h"
#include"MaterialSystem/Shader/ShaderWriter.h"
#include"MaterialSystem/Shader/ShaderReader.h"
using namespace JGRC;
using namespace std;


SRSLightPass::SRSLightPass()
{
	m_Dx = DirectX::GetInstance();
	m_Scene = make_unique<SRSScene>();
	m_RenderTarget = make_unique<SRSRenderTarget>();
}
SRSLightPass::~SRSLightPass()
{

}
void SRSLightPass::Init(HWND hWnd, const uint width, const uint height)
{
	m_hWnd = hWnd;
	m_Scene->CreateScene(width, height);
	if (!m_RenderTarget->CreateSRSRenderTarget(width, height))
	{
		JGLOG(log_Error, "JGRC::SRSLightPass", "Failed Create LightPass's RenderTarget");
		return;
	}

}
void SRSLightPass::Render(const jgVec3& CameraPos)
{

}

void SRSLightPass::InitHlsl()
{
	HlslEditor* VertexEdit = m_Dx->CreateObject<HlslEditor>();
	VertexEdit->RegisterHlslPath(EShaderType::Vertex, Game::path / "Engine/Shader/HLSL/LightPass_vs.hlsl");
	HlslEditor* PixelEdit = m_Dx->CreateObject<HlslEditor>();
	PixelEdit->RegisterHlslPath(EShaderType::Pixel, Game::path / "Engine/Shader/HLSL/LightPass_ps.hlsl");


	// ¹öÅØ½º ¼ÎÀÌ´õ
	InputLayout* vsIL = VertexEdit->CreateInputLayout();
	vsIL->AddDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	vsIL->AddDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0);


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
	m_Shader = reader.ReadShader(Game::path / "Engine/Shader/Shader/LightPass.shader");

	m_Dx->DeleteObject(VertexEdit);
	m_Dx->DeleteObject(PixelEdit);
}