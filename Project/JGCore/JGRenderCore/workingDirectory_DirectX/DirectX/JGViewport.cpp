#include"JGViewport.h"
using namespace JGRC;
using namespace std;
JGViewport::JGViewport()
{
	m_Viewport = make_unique<D3D11_VIEWPORT>();
	m_projectionMatrix = make_unique<jgMatrix4x4>();
	m_orthoMatrix = make_unique<jgMatrix4x4>();
}
JGViewport::~JGViewport()
{

}
bool JGViewport::InitViewport(const int Width, const int Height, const real FOV, const real FarZ, const real NearZ)
{
	m_Width  = (real)Width;
	m_Height = (real)Height;
	m_FOV    = FOV;
	m_FarZ   = FarZ;
	m_NearZ  = NearZ;


	// 뷰포트 생성
	CreateViewport();
	// 투영 행렬 생성
	CreateProjectionMatrix();
	// 정사영 행렬 생성
	CreateOrthoMatrix();
	JGLOG(log_Info, "JGRC::JGViewport", "Create ViewPort.");
	return true;
}
D3D11_VIEWPORT* JGViewport::Get()
{
	return m_Viewport.get();
}
jgMatrix4x4& JGViewport::GetProjectionMatrix()
{
	return *m_projectionMatrix;
}

jgMatrix4x4& JGViewport::GetOrthoMatrix()
{
	return *m_orthoMatrix;
}

const float JGViewport::GetFOV()
{
	return m_FOV;
}
const float JGViewport::GetWidth()
{
	return m_Width;
}
const float JGViewport::GetHeight()
{
	return m_Height;
}
void JGViewport::SetFOV(const real FOV)
{
	m_FOV = FOV;
	CreateProjectionMatrix();
}

void JGViewport::CreateViewport(const real MinDepth, const real MaxDepth, const real TopLeftX, const real TopLeftY)
{
	m_Viewport->Width = m_Width;
	m_Viewport->Height = m_Height;
	m_Viewport->MinDepth = MinDepth;
	m_Viewport->MaxDepth = MaxDepth;
	m_Viewport->TopLeftX = TopLeftX;
	m_Viewport->TopLeftY = TopLeftY;
}
void JGViewport::CreateProjectionMatrix()
{
	real ScreenAspect = m_Width / m_Height;
	m_projectionMatrix->perspectiveFovLH(m_FOV, ScreenAspect, m_NearZ, m_FarZ);
}
void JGViewport::CreateOrthoMatrix()
{
	m_orthoMatrix->orthoLH(m_Width, m_Height, m_NearZ, m_FarZ);
}
