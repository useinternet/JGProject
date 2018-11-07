#include"JGSRSystem.h"
#include"SRSRenderTarget.h"
#include"DirectX/DirectX.h"
using namespace std;
using namespace JGRC;

JGSRSystem::JGSRSystem()
{
	m_Dx = DirectX::GetInstance();
	m_SRSRT = make_unique<SRSRenderTarget>();
	for (uint i = 0; i < 4; ++i)
	{
		m_ClearColor.push_back(0);
	}

}
JGSRSystem::~JGSRSystem()
{

}
void JGSRSystem::BindingRenderFunc(const function<void()>& func)
{
	m_RenderFunc = func;
}
bool JGSRSystem::InitSRSystem(const int width, const int height)
{
	bool result = m_SRSRT->CreateSRSRenderTarget(width, height);
	if (!result)
	{
		return false;
	}
	return true;
}
void JGSRSystem::Render()
{
	m_SRSRT->BindingRenderTarget();
	m_SRSRT->ClearRenderTarget(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);

	m_RenderFunc();

	m_Dx->SetDefautRenderTarget();
}
void JGSRSystem::SetClearColor(const real r, const real g, const real b, const real a)
{
	m_ClearColor[0] = r;
	m_ClearColor[1] = g;
	m_ClearColor[2] = b;
	m_ClearColor[3] = a;
}
SRSRenderTarget* JGSRSystem::GetRenderTarget() const
{
	return m_SRSRT.get();
}