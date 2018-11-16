#include"SRSScene.h"
#include"DirectX/DirectX.h"
#include"DirectX/JGViewport.h"
#include"SRSMaterial.h"
#include"MaterialSystem/Mesh/Mesh.h"
#include"MaterialSystem/MaterialReader.h"
#include"MaterialSystem/Shader/ShaderAnalyze/ShaderAnalyzer.h"
#include"SRSRenderTarget.h"
using namespace JGRC;
using namespace std;


SRSScene::SRSScene()
{
	m_Mesh = make_unique<Mesh>();
	m_Material = make_unique<SRSMaterial>();
	m_viewMatrix = make_unique<jgMatrix4x4>();
	m_orthoMatrix = make_unique<jgMatrix4x4>();
	m_worldMatrix = make_unique<jgMatrix4x4>();
	m_wvpMatrix = make_unique<jgMatrix4x4>();

	//*m_orthoMatrix = DirectX::GetInstance()->GetViewport()->GetOrthoMatrix();
	m_viewMatrix->lookAtLH(jgVec3(0.0f, 0.0f, -5.0f), jgVec3(0.0f, 0.0f, 1.0f), jgVec3(0.0f, 1.0f, 0.0f));
	m_worldMatrix->identity();

	m_orthoMatrix->transpose();
	m_viewMatrix->transpose();
	m_worldMatrix->transpose();

	*m_wvpMatrix = (*m_orthoMatrix) * (*m_viewMatrix) * (*m_worldMatrix);
}
SRSScene::~SRSScene()
{
	ID3D11RenderTargetView* nullRTV = nullptr;
	DirectX::GetInstance()->GetContext()->OMSetRenderTargets(1, &nullRTV, nullptr);
	ID3D11ShaderResourceView* null = nullptr;
	DirectX::GetInstance()->GetContext()->PSSetShaderResources(0, 1, &null);

}
void SRSScene::CreateScene(const int width, const int height)
{
	real left = (real)((width / 2) * -1);
	real right = left + (real)width;
	real top = (real)(height / 2);
	real bottom = top - (real)height;

	vector<real> Vertex;
	// // 삼각형 1
	// 왼쪽 위
	Vertex.push_back(left);    Vertex.push_back(top);      Vertex.push_back(0.0f);
	Vertex.push_back(0.0f);    Vertex.push_back(0.0f);
	// 오른쪽 아래
	Vertex.push_back(right);   Vertex.push_back(bottom);   Vertex.push_back(0.0f);
	Vertex.push_back(1.0f);    Vertex.push_back(1.0f);
	// 왼쪽 아래
	Vertex.push_back(left);    Vertex.push_back(bottom);   Vertex.push_back(0.0f);
	Vertex.push_back(0.0f);    Vertex.push_back(1.0f);

	// // 삼각형 2
	// 왼쪽 위
	Vertex.push_back(left);    Vertex.push_back(top);      Vertex.push_back(0.0f);
	Vertex.push_back(0.0f);    Vertex.push_back(0.0f);
	// 오른쪽 위
	Vertex.push_back(right);   Vertex.push_back(top);      Vertex.push_back(0.0f);
	Vertex.push_back(1.0f);    Vertex.push_back(0.0f);
	// 오른쪽 아래
	Vertex.push_back(right);   Vertex.push_back(bottom);   Vertex.push_back(0.0f);
	Vertex.push_back(1.0f);    Vertex.push_back(1.0f);

	m_Mesh->CustomModel(Vertex,6);

	ShaderAnalyzer ayz;
	ayz.Analyze(Game::path / "HLSL/SRS_PostProcessing_vs.hlsl", EShaderType::Vertex);
	ayz.Analyze(Game::path / "HLSL/SRS_PostProcessing_ps.hlsl", EShaderType::Pixel);
	ayz.OutputShaderData(Game::path / "SRS_PostProcessing");

	MaterialReader MtReader;
	
	if (!MtReader.SRS_Read(Game::path / "SRS_PostProcessing.material",m_Material.get()))
	{
		JGLOG(log_Critical, "JGRC::SRScene", "Falied Create SRSScene / Failed Read Material");
		return;
	}


	////////// 실험
	m_Material->SetMesh(m_Mesh.get());
	m_Material->SetParam("wvpMatrix", m_wvpMatrix.get());
	jgVec4 AD(0.1f, 0.1f, 0.1f,1.0f);
	jgVec4 AU(0.2f, 0.2f, 0.2f, 1.0f);
	jgVec4 Color(0.8f, 0.8f, 0.8f, 1.0f);
	m_Material->SetParam("DL_AmbientDown", &AD);
	m_Material->SetParam("DL_AmbientUp", &AU);
	m_Material->SetParam("DL_LightColor", &Color);
	jgVec4 CameraPos(0.0f, 0.0f, -5.0f, 1.0f);
	m_Material->SetParam("DL_CameraPos", &CameraPos);

}
void SRSScene::Render(SRSRenderTarget* SRST)
{
	static jgVec4 Dir(-0.0f, -1.0f, 0.0,1.0f);

	m_Material->SetParam("DL_LightDir", &Dir);
	
	DirectX::GetInstance()->SetDirectState(EStateType::DepthState, (uint)EDepthStateType::ZBufferOff);
	m_Material->Render(
		SRST->GetShaderResourceView(ERTType::Position),
		SRST->GetShaderResourceView(ERTType::Albedo), 
		SRST->GetShaderResourceView(ERTType::Normal),
		SRST->GetShaderResourceView(ERTType::Depth));
	ID3D11ShaderResourceView* null[4] = { nullptr, nullptr, nullptr, nullptr };
	DirectX::GetInstance()->GetContext()->PSSetShaderResources(0, 4, null);
	DirectX::GetInstance()->SetDirectState(EStateType::DepthState, (uint)EDepthStateType::ZBufferOn);
}
void SRSScene::SetOrthoMatrix(const jgMatrix4x4& m)
{
	*m_orthoMatrix = m;
	m_orthoMatrix->transpose();

	*m_wvpMatrix = (*m_orthoMatrix) * (*m_viewMatrix) * (*m_worldMatrix);
	m_bMatrixChange = true;
}
void SRSScene::SetViewMatrix(const jgMatrix4x4& m)
{
	*m_viewMatrix = m;
	m_viewMatrix->transpose();

	*m_wvpMatrix = (*m_orthoMatrix) * (*m_viewMatrix) * (*m_worldMatrix);
	m_bMatrixChange = true;
}