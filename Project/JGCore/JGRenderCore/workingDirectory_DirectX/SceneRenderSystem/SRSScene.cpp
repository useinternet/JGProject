#include"SRSScene.h"
#include"DirectX/DirectX.h"
#include"DirectX/JGViewport.h"
#include"MaterialSystem/Mesh/Mesh.h"
#include"Camera/Camera.h"
using namespace JGRC;
using namespace std;


SRSScene::SRSScene()
{
	m_Mesh        = make_unique<Mesh>();
	m_worldMatrix = make_unique<jgMatrix4x4>();
	m_orthoMatrix = make_unique<jgMatrix4x4>();
	m_wvpMatrix   = make_unique<jgMatrix4x4>();
	m_Camera = make_unique<Camera>();
	m_Camera->SetEye(0.0f, 0.0f, -5.0f);
}
SRSScene::~SRSScene()
{

}
void SRSScene::CreateScene(const DxWinConfig& config)
{
	CreateScene(config.ScreenWidth, config.ScreenHeight, config.NearZ, config.FarZ);
}
void SRSScene::CreateScene(const int width, const int height, const real nearZ, const real farZ)
{
	m_worldMatrix->identity();
	m_orthoMatrix->orthoLH((real)width, (real)height, nearZ, farZ);
	*m_wvpMatrix = (*m_worldMatrix) * (m_Camera->GetViewMatrix()) * (*m_orthoMatrix);
	m_wvpMatrix->transpose();




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
	std::vector<UINT> stride;
	stride.push_back(20);
	std::vector<UINT> offset;
	offset.push_back(0);
	m_Mesh->CustomModel(Vertex, 6, stride, offset);
	m_Mesh->CreateBuffer(nullptr);
}
void SRSScene::Render()
{
	// 렌더링
	m_Mesh->Render();
}
uint SRSScene::GetIndexCount()
{
	return m_Mesh->getIndexCount();
}
Camera* SRSScene::GetCamera()
{
	return m_Camera.get();
}
const jgMatrix4x4& SRSScene::GetwvpMatrix()
{
	return *m_wvpMatrix;
}