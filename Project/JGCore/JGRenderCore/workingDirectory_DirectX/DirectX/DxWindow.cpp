#include"DxWindow.h"
#include"DirectX.h"
#include"JGSwapChain.h"
#include"JGRenderTarget.h"
#include"JGRenderState.h"
#include"JGViewport.h"
#include"SceneRenderSystem/SRSRenderTarget.h"
#include"Camera/Camera.h"
#include"SceneRenderSystem/LightPass.h"
using namespace JGRC;
using namespace std;
DxWindow::DxWindow()
{
	m_Dx = DirectX::GetInstance();
	m_SwapChain    = make_unique<JGSwapChain>();
	m_RenderTarget = make_unique<JGRenderTarget>();
	m_Viewport     = make_unique<JGViewport>();
	m_WindowCamera = make_unique<Camera>();
	m_SRSRT        = make_unique<SRSRenderTarget>();
	m_LightPass = make_unique<LightPass>();
}
DxWindow::~DxWindow()
{

}
bool DxWindow::Init(const DxWinConfig& config)
{
	bool result = true;
	m_hWnd = config.hWnd;
	string hwndNumber = to_string((int)config.hWnd);
	JGLOG(log_Info, "JGRC::DxWindow", hwndNumber + " : Create DxWindow Start")
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
	

	m_LightPass->InitPass(config);

	result = m_SRSRT->CreateSRSRenderTarget(config.ScreenWidth, config.ScreenHeight);
	if (!result)
	{
		JGLOG(log_Critical, "JGRC::DxWindow", "Failed SRSRenderTarget");
		return false;
	}

	JGLOG(log_Info, "JGRC::DxWindow", hwndNumber + " : Create DxWindow Complete")
	return true;
}
void DxWindow::Draw()
{
	float color[4] = { m_BackColor.x,m_BackColor.y,m_BackColor.z,m_BackColor.w };
	m_SRSRT->BindingRenderTarget();
	for (auto& evt : m_RenderPassEvent)
	{
		evt();
	}
	m_SRSRT->ReleaseRenderTarget();

	m_Dx->GetContext()->OMSetRenderTargets(1, m_RenderTarget->GetAddress(), m_RenderTarget->GetDepthStencilView());
	m_Dx->GetContext()->RSSetViewports(1, m_Viewport->Get());
	m_Dx->GetContext()->ClearRenderTargetView(m_RenderTarget->Get(), color);
	m_Dx->GetContext()->ClearDepthStencilView(m_RenderTarget->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	m_LightPass->Draw(m_SRSRT.get(), m_WindowCamera.get(), m_Viewport.get());
	m_SwapChain->Get()->Present(0, 0);
}
Camera* DxWindow::GetWindowCamera()
{
	return m_WindowCamera.get();
}
jgMatrix4x4& DxWindow::GetProjectionMatrix()
{
	return m_Viewport->GetProjectionMatrix();
}
jgMatrix4x4& DxWindow::GetOrhthoMatrix()
{
	return m_Viewport->GetOrthoMatrix();
}