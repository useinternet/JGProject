#include"DxWindow.h"
#include"DirectX.h"
#include"JGSwapChain.h"
#include"JGRenderTarget.h"
#include"JGRenderState.h"
#include"JGViewport.h"
using namespace JGRC;
using namespace std;


DxWindow::DxWindow()
{
	m_Dx = DirectX::GetInstance();
	m_SwapChain    = make_unique<JGSwapChain>();
	m_RenderTarget = make_unique<JGRenderTarget>();
	m_Viewport     = make_unique<JGViewport>();
}
DxWindow::~DxWindow()
{

}
bool DxWindow::Init(const JGInitConfig& config)
{
	bool result = true;
	string hwndNumber = to_string((int)config.hWnd);
	JGLOG(log_Info, "JGRC::DxWindow", hwndNumber + " : dx윈도우 생성 시작")
	result = m_SwapChain->CreateSwapChain(m_Dx->GetDevice(), config.hWnd, config.bFullScreen,
		config.ScreenWidth, config.ScreenHeight);
	if (!result)
	{
		JGLOG(log_Critical, "JGRC::DxWindow", "Failed SwapChain.. : " + hwndNumber);
		return false;
	}
	result = m_RenderTarget->CreateRenderTarget(m_Dx->GetDevice(), m_SwapChain->Get(),
		config.ScreenWidth, config.ScreenHeight);
	if (!result)
	{
		JGLOG(log_Critical, "JGRC::DxWindow", "Failed RenderTarget : " + hwndNumber);
		return false;
	}
	result = m_Viewport->InitViewport(config.ScreenWidth, config.ScreenHeight, config.Fov, config.FarZ, config.NearZ);

	JGLOG(log_Info, "JGRC::DxWindow", hwndNumber + " : dx윈도우 생성 완료")
	return true;
}
void DxWindow::Draw()
{
	for (auto& evt : m_SubEvent)
	{
		evt();
	}
	float color[4] = { m_BackColor.x,m_BackColor.y,m_BackColor.z,m_BackColor.w };
	m_Dx->GetContext()->ClearRenderTargetView(m_RenderTarget->Get(), color);
	m_Dx->GetContext()->ClearDepthStencilView(m_RenderTarget->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	for (auto& evt : m_MainEvent)
	{
		evt();
	}

	m_SwapChain->Get()->Present(0, 0);
}
void DxWindow::ResigisterRenderTarget()
{
	m_Dx->GetContext()->OMSetRenderTargets(1, m_RenderTarget->GetAddress(), m_RenderTarget->GetDepthStencilView());
	m_Dx->GetContext()->RSSetViewports(1, m_Viewport->Get());
}
jgMatrix4x4& DxWindow::GetProjectionMatrix()
{
	return m_Viewport->GetProjectionMatrix();
}
jgMatrix4x4& DxWindow::GetOrhthoMatrix()
{
	return m_Viewport->GetOrthoMatrix();
}