#include"JGViewportD.h"
#include"../EngineStatics/JMath/JGMatrix.h"
using namespace std;
JGViewportD::JGViewportD()
{
	m_Viewport = make_unique<D3D11_VIEWPORT>();
	m_projectionMatrix = make_unique<JGMatrix>();
	m_orthoMatrix = make_unique<JGMatrix>();
}
JGViewportD::~JGViewportD()
{

}
bool JGViewportD::InitViewport(const int Width, const int Height, const float FOV, const float FarZ, const float NearZ)
{
	m_Width  = (float)Width;
	m_Height = (float)Height;
	m_FOV    = FOV;
	m_FarZ   = FarZ;
	m_NearZ  = NearZ;


	// 뷰포트 생성
	CreateViewport();
	// 투영 행렬 생성
	CreateProjectionMatrix();
	// 정사영 행렬 생성
	CreateOrthoMatrix();

	return true;
}
D3D11_VIEWPORT * JGViewportD::Get()
{
	return m_Viewport.get();
}
JGMatrix& JGViewportD::GetProjectionMatrix()
{
	return *m_projectionMatrix;
}

JGMatrix& JGViewportD::GetOrthoMatrix()
{
	return *m_orthoMatrix;
}

const float JGViewportD::GetFOV()
{
	return m_FOV;
}
const float JGViewportD::GetWidth()
{
	return m_Width;
}
const float JGViewportD::GetHeight()
{
	return m_Height;
}
void JGViewportD::SetFOV(const float FOV)
{
	m_FOV = FOV;
	CreateProjectionMatrix();
}
void JGViewportD::CreateViewport(const float MinDepth, const float MaxDepth, const float TopLeftX, const float TopLeftY)
{
	m_Viewport->Width = m_Width;
	m_Viewport->Height = m_Height;
	m_Viewport->MinDepth = MinDepth;
	m_Viewport->MaxDepth = MaxDepth;
	m_Viewport->TopLeftX = TopLeftX;
	m_Viewport->TopLeftY = TopLeftY;
}
void JGViewportD::CreateProjectionMatrix()
{
	// 화면 비율을 구한다.
	float ScreenAspect = m_Width / m_Height;
	// 투영 행렬을 만든다.
	m_projectionMatrix->MakeProjectionMatrix(m_FOV, ScreenAspect, m_NearZ, m_FarZ);
}
void JGViewportD::CreateOrthoMatrix()
{
	m_orthoMatrix->MakeOrthoMatrix(m_Width, m_Height, m_NearZ, m_FarZ);
}
