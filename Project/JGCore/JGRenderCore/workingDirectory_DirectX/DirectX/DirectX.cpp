#include"DirectX.h"
#include"BufferSystem/JGBuffer.h"
#include"JGDevice.h"
#include"JGRenderState.h"


using namespace JGRC;
using namespace std;
DirectX* DirectX::Instance = nullptr;
DirectX::DirectX()
{
	m_Device = make_unique<JGDevice>();
	m_RenderState = make_unique<JGRenderState>();
	for (int i = 0; i < 4; ++i)
	{
		m_BlendFactor[i] = 0.0f;
	}
}
DirectX::~DirectX()
{

}
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
void DirectX::Draw()
{
	for (auto& window : m_mDxWindow)
	{
		window.second->Draw();
	}
}
bool DirectX::Init()
{
	JGLOG(log_Info, "JGRC::DirectX", "Creating DirectXApp..");
	bool result = true;
	result = m_Device->CreateDevice();
	if (!result)
	{
		JGLOG(log_Critical, "JGRC::DirectX", "Failed DirectXApp..");
		return false;
	}
	result = m_RenderState->RenderStateInit(m_Device->GetDevice());
	if (!result)
	{
		JGLOG(log_Critical, "JGRC::DirectX", "Failed RenderState");
		return false;
	}
	return true;
}
DxWindow* DirectX::AddDxWindow(const DxWinConfig& config)
{
	DxWindow* window = CreateObject<DxWindow>();
	
	bool result = window->Init(config);
	if (!result)
	{
		DeleteObject(window);
		return nullptr;
	}
	m_mDxWindow.insert(DxWindowPair(config.hWnd, window));
	return window;
}
void DirectX::DeleteDxWindow(HWND hWnd)
{
	auto check = m_mDxWindow.find(hWnd);
	if (check != m_mDxWindow.end())
	{
		JGLOG(log_Info, "JGRC::DirectX", to_string((int)hWnd) + "'s Delete DxWindow");
		DeleteObject(m_mDxWindow[hWnd]);
	}
}
DxWindow* DirectX::GetWindow(HWND hWnd)
{
	auto check = m_mDxWindow.find(hWnd);
	if (check == m_mDxWindow.end())
	{
		JGLOG(log_Error, "JGRC::DirectX", to_string((int)hWnd) + "'s Handle is not exist");
		return nullptr;
	}
	return m_mDxWindow[hWnd];
}
void DirectX::DeleteObject(JGRCObject* obj)
{
	if (m_usObjects.empty())
	{
		return;
	}
	m_usObjects.erase(obj);
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
	context->OMSetDepthStencilState(m_RenderState->GetDepthState(EDepthStateType::ZBufferOn),1);
	context->RSSetState(m_RenderState->GetRasterizerState(EFillModeType::Solid));
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