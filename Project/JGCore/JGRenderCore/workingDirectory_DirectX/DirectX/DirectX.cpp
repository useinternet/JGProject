#include"DirectX.h"
#include"JGDevice.h"
#include"JGSwapChain.h"
#include"JGRenderTarget.h"
#include"JGRenderState.h"
#include"JGViewport.h"
#include"Common/JGInitConfig.h"

using namespace JGRC;
using namespace std;
DirectX* DirectX::Instance = nullptr;
DirectX::DirectX()
{
	m_Device = make_unique<JGDevice>();
	m_SwapChain = make_unique<JGSwapChain>();
	m_RenderTarget = make_unique<JGRenderTarget>();
	m_RenderState = make_unique<JGRenderState>();
	m_Viewport = make_unique<JGViewport>();
	for (int i = 0; i < 4; ++i)
	{
		m_BackColor[i] = 0.0f;
		m_BlendFactor[i] = 0.0f;
	}
}
DirectX::~DirectX() {}
DirectX* DirectX::GetInstance()
{
	if (Instance == nullptr)
	{
		Instance = new DirectX;
	}
	return Instance;
}
void  DirectX::Release()
{
	if (Instance)
	{
		delete Instance;
		Instance = nullptr;
	}
}
bool DirectX::Init(const JGInitConfig& config)
{
	JGLOG(log_Info, "JGRC::DirectX", "Creating DirectXApp..");
	bool result = true;
	result = m_Device->CreateDevice();
	if (!result)
	{
		JGLOG(log_Critical, "JGRC::DirectX", "Failed DirectXApp..");
		return false;
	}
	result = m_SwapChain->CreateSwapChain(m_Device->GetDevice(), config.hWnd, config.bFullScreen,
		config.ScreenWidth, config.ScreenHeight);
	if (!result)
	{
		JGLOG(log_Critical, "JGRC::DirectX", "Failed SwapChain..");
		return false;
	}
	result = m_RenderTarget->CreateRenderTarget(m_Device->GetDevice(), m_SwapChain->Get(),
		config.ScreenWidth, config.ScreenHeight);
	if (!result)
	{
		JGLOG(log_Critical, "JGRC::DirectX", "Failed RenderTarget");
		return false;
	}
	result = m_RenderState->RenderStateInit(m_Device->GetDevice());
	if (!result)
	{
		JGLOG(log_Critical, "JGRC::DirectX", "Failed RenderState");
		return false;
	}
	result = m_Viewport->InitViewport(config.ScreenWidth, config.ScreenHeight, config.Fov, config.FarZ, config.NearZ);
	JGLOG(log_Info, "JGRC::DirectX", "Create DirectXApp. Complete");
	return true;
}
void DirectX::Draw()
{
	m_Device->GetContext()->ClearRenderTargetView(m_RenderTarget->Get(), m_BackColor);
	m_Device->GetContext()->ClearDepthStencilView(m_RenderTarget->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	// 랭크 크기 오름차순으로 정렬
	sort(m_vDrawEvent.begin(), m_vDrawEvent.end(), 
		[](const DrawEvent& e1, const DrawEvent& e2) -> bool
	{
		return (e1.rank < e2.rank);
	});

	for (auto& iter : m_vDrawEvent)
	{
		iter.func();
	}
	m_SwapChain->Get()->Present(0, 0);
}
void DirectX::RegistorDrawEvent(const DrawEvent& drawEvent)
{
	m_vDrawEvent.push_back(drawEvent);
}
ID3D11Device* DirectX::GetDevice() const
{
	return m_Device->GetDevice();
}
ID3D11DeviceContext* DirectX::GetContext() const
{
	return m_Device->GetContext();
}
void DirectX::SetDefaultDirectState()
{
	ID3D11DeviceContext* context = GetContext();
	SetDefautRenderTarget();
	context->OMSetDepthStencilState(m_RenderState->GetDepthState(EDepthStateType::ZBufferOn),1);
	context->RSSetState(m_RenderState->GetRasterizerState(EFillModeType::Solid));
}
JGViewport* DirectX::GetViewport() const
{
	return m_Viewport.get();
}
void DirectX::SetBackColor(const real r, const real g, const real b, const real a)
{
	m_BackColor[0] = r; m_BackColor[1] = g; m_BackColor[2] = b; m_BackColor[3] = a;
}
void DirectX::SetDirectState(const EStateType StateType, const uint ConfigType)
{
	switch (StateType)
	{
	case EStateType::DepthState:
		m_Device->BindDepthStencilState(m_RenderState.get(),(EDepthStateType)ConfigType);
		break;
	case EStateType::BlendState:
		m_Device->BindBlendState(m_RenderState.get(), (EBlendStateType)ConfigType, m_BlendFactor, m_SampleMask);
		break;
	case EStateType::RasterState:
		m_Device->BindRasterizerState(m_RenderState.get(), (EFillModeType)ConfigType);
		break;
	}
}
void DirectX::SetBlendFactor(const real a1, const real a2, const real a3, const real a4)
{
	m_BlendFactor[0] = a1; m_BlendFactor[1] = a2; m_BlendFactor[2] = a3; m_BlendFactor[3] = a4;
}
void DirectX::SetSampleMask(const uint mask)
{
	m_SampleMask = mask;
}
void DirectX::SetDefautRenderTarget()
{
	ID3D11DeviceContext* context = GetContext();
	context->OMSetRenderTargets(1, m_RenderTarget->GetAddress(), m_RenderTarget->GetDepthStencilView());
	context->RSSetViewports(1, m_Viewport->Get());
}