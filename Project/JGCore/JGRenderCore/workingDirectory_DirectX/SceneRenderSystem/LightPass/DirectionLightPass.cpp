#include"DirectionLightPass.h"
#include"DirectX/DirectX.h"
#include"MaterialSystem/Shader/HlslEditor.h"
#include"MaterialSystem/Shader/JGShaderArray.h"
#include"MaterialSystem/Shader/ShaderWriter.h"
#include"MaterialSystem/Shader/ShaderReader.h"
#include"SceneRenderSystem/SRSRenderTarget.h"
#include"Camera/Camera.h"
#include"DirectX/JGViewport.h"
using namespace JGRC;
using namespace std;

DirectionLightPass::DirectionLightPass()
{
	m_Dx = DirectX::GetInstance();
}
DirectionLightPass::~DirectionLightPass()
{

}
bool DirectionLightPass::Init(const DxWinConfig& config, const bool isUpdate)
{
	HlslEditor* VertexEd = m_Dx->CreateObject<HlslEditor>();
	VertexEd->RegisterHlslPath(EShaderType::Vertex, m_vsHlslPath);

	HlslEditor* PixelEd = m_Dx->CreateObject<HlslEditor>();
	PixelEd->RegisterHlslPath(EShaderType::Pixel, m_psHlslPath);

	Texture2D* t = PixelEd->CreateTexture2D();
	t->Add("DepthTexture");
	t->Add("ColorSpecIntTexture");
	t->Add("NormalTexture");
	t->Add("SpecPowTexture");

	CBuffer* cbfp = PixelEd->CreateCBuffer();
	cbfp->SetName("UnpackBuffer");
	cbfp->AddVar("PerspectiveValues", 0, 3);
	cbfp->AddVar("ViewInv", 4, 19);

	ShaderWriter writer;
	writer.AddEditor(VertexEd);
	writer.AddEditor(PixelEd);
	writer.Write(m_PassShaderPath);

	ShaderReader reader(config.hWnd);
	m_Shader = reader.ReadShader(m_PassShaderPath + ".shader");

	m_Shader->Get(EShaderType::Pixel)->AddTexture(nullptr);
	m_Shader->Get(EShaderType::Pixel)->AddTexture(nullptr);
	m_Shader->Get(EShaderType::Pixel)->AddTexture(nullptr);
	m_Shader->Get(EShaderType::Pixel)->AddTexture(nullptr);

	return true;
}
void DirectionLightPass::Render(SRSRenderTarget* SRSRT, Camera* cam, JGViewport* viewport)
{
	jgMatrix4x4 proj = viewport->GetProjectionMatrix();
	jgVec4 vec(1.0f / proj(1, 1), 1.0f / proj(2, 2), proj(4, 3), -proj(3, 3));
	jgMatrix4x4 ViewInv = cam->GetViewMatrix();
	ViewInv.inverse();
	ViewInv.transpose();

	m_Shader->Get(EShaderType::Pixel)->SetParam("PerspectiveValues", &vec, 4);
	m_Shader->Get(EShaderType::Pixel)->SetParam("ViewInv", &ViewInv, 16);
	m_Shader->Get(EShaderType::Pixel)->SetTexture(0, SRSRT->GetDepthTexture());
	m_Shader->Get(EShaderType::Pixel)->SetTexture(1, SRSRT->GetColorSpecInsTexture());
	m_Shader->Get(EShaderType::Pixel)->SetTexture(2, SRSRT->GetNormalTexture());
	m_Shader->Get(EShaderType::Pixel)->SetTexture(3, SRSRT->GetSpecPowerTexture());


	m_Dx->GetContext()->IASetInputLayout(nullptr);
	m_Dx->GetContext()->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	m_Dx->GetContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	m_Shader->Render(4, EShaderRenderType::Draw);

	ID3D11ShaderResourceView *arrRV[4] = { nullptr,nullptr,nullptr,nullptr };
	m_Dx->GetContext()->PSSetShaderResources(0, 4, arrRV);
}