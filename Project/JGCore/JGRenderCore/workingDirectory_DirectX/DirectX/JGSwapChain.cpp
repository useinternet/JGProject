#include"JGSwapChain.h"
using namespace Microsoft::WRL;
using namespace JGRC;

JGSwapChain::JGSwapChain() {}
JGSwapChain::~JGSwapChain() {}

bool JGSwapChain::CreateSwapChain(ID3D11Device* Device, HWND hWnd, const bool bFullScreen,
	const int ScreenWidth, const int ScreenHeight)
{
	JGLOG(log_Info, "JGRC::JGSwapChain", "Creating SwapChain..");
	HRESULT result;
	ComPtr<IDXGIDevice1>  DXGIDevice;
	ComPtr<IDXGIAdapter>  DXGIAdapter;
	ComPtr<IDXGIFactory1> DXGIFactory;


	result = Device->QueryInterface(__uuidof(IDXGIDevice1), (void**)DXGIDevice.GetAddressOf());
	if (FAILED(result))
	{
		JGLOG(log_Critical, "JGRC::JGSwapChain", "Failed QueryInterface of Device..");
		return false;
	}
	result = DXGIDevice->GetAdapter(DXGIAdapter.GetAddressOf());
	if (FAILED(result))
	{
		JGLOG(log_Critical, "JGRC::JGSwapChain", "Failed Adapter of Device..");
		return false;
	}
	result = DXGIAdapter->GetParent(__uuidof(IDXGIFactory1), (void**)DXGIFactory.GetAddressOf());
	if (FAILED(result))
	{
		JGLOG(log_Critical, "JGRC::JGSwapChain", "Failed Factory of Device..");
		return false;
	}
	
	// 스왑체인 목록 작성
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	ZeroMemory(&SwapChainDesc, sizeof(SwapChainDesc));
	//
	SwapChainDesc.BufferCount = 1;
	//
	SwapChainDesc.BufferDesc.Width  = ScreenWidth;
	SwapChainDesc.BufferDesc.Height = ScreenHeight;
	//
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	//
	SwapChainDesc.BufferDesc.RefreshRate.Numerator   = 0;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	//
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.BufferDesc.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED;
	//
	SwapChainDesc.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.OutputWindow       = hWnd;
	SwapChainDesc.SampleDesc.Count   = 1;
	SwapChainDesc.SampleDesc.Quality = 0;
	//
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	SwapChainDesc.Flags      = 0;
	//
	if (bFullScreen)
	{
		SwapChainDesc.Windowed = false;
	}
	else
	{
		SwapChainDesc.Windowed = true;
	}
	result = DXGIFactory->CreateSwapChain(Device, &SwapChainDesc, m_SwapChain.GetAddressOf());
	if (FAILED(result))
	{
		JGLOG(log_Critical, "JGRC::JGSwapChain", "Failed CreateSwapChain");
		// 예외 로그 출력
	}
	JGLOG(log_Info, "JGRC::JGSwapChain", "Create CreateSwapChain Complete");
	return true;
}

IDXGISwapChain * JGSwapChain::Get()
{
	if (m_SwapChain.Get())
	{
		return m_SwapChain.Get();
	}
	JGLOG(log_Error, "JGRC::JGSwapChain", "SwapChain is nullptr");
	return nullptr;
}
