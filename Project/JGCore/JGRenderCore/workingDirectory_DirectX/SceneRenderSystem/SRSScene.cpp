#include"SRSScene.h"
#include"DirectX/DirectX.h"
#include"DirectX/JGViewport.h"
#include"MaterialSystem/Mesh/Mesh.h"
#include"SRSRenderTarget.h"
using namespace JGRC;
using namespace std;


SRSScene::SRSScene()
{
	m_Mesh        = make_unique<Mesh>();
	m_viewMatrix  = make_unique<jgMatrix4x4>();
	m_orthoMatrix = make_unique<jgMatrix4x4>();
	m_worldMatrix = make_unique<jgMatrix4x4>();
	m_wvpMatrix   = make_unique<jgMatrix4x4>();

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
void SRSScene::CreateScene(const uint width, const uint height)
{
	real left = (real)((width / 2) * -1);
	real right = left + (real)width;
	real top = (real)(height / 2);
	real bottom = top - (real)height;


	jgVec3 pos; 	jgVec2 tex;

	vector<real> Vertex;
	// // »ï°¢Çü 1
	// ¿ÞÂÊ À§
	Vertex.push_back(left);    Vertex.push_back(top);      Vertex.push_back(0.0f);
	Vertex.push_back(0.0f);    Vertex.push_back(0.0f);
	// ¿À¸¥ÂÊ ¾Æ·¡
	Vertex.push_back(right);   Vertex.push_back(bottom);   Vertex.push_back(0.0f);
	Vertex.push_back(1.0f);    Vertex.push_back(1.0f);
	// ¿ÞÂÊ ¾Æ·¡
	Vertex.push_back(left);    Vertex.push_back(bottom);   Vertex.push_back(0.0f);
	Vertex.push_back(0.0f);    Vertex.push_back(1.0f);

	// // »ï°¢Çü 2
	// ¿ÞÂÊ À§
	Vertex.push_back(left);    Vertex.push_back(top);      Vertex.push_back(0.0f);
	Vertex.push_back(0.0f);    Vertex.push_back(0.0f);
	// ¿À¸¥ÂÊ À§
	Vertex.push_back(right);   Vertex.push_back(top);      Vertex.push_back(0.0f);
	Vertex.push_back(1.0f);    Vertex.push_back(0.0f);
	// ¿À¸¥ÂÊ ¾Æ·¡
	Vertex.push_back(right);   Vertex.push_back(bottom);   Vertex.push_back(0.0f);
	Vertex.push_back(1.0f);    Vertex.push_back(1.0f);

	m_Mesh->CustomModel(Vertex,6);
}
void SRSScene::Render(SRSRenderTarget* SRST)
{
	DirectX::GetInstance()->SetDirectState(EStateType::DepthState, (uint)EDepthStateType::ZBufferOff);
	// ·»´õ¸µ
	m_Mesh->Render();
	// ¼ÎÀÌ´õ ·»´õ¸µ
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