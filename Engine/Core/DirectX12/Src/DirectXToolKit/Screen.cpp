#include<PCH.h>
#include"Screen.h"
#include"DirectXCore.h"
#include"CommandList.h"
#include"CommandQueue.h"
using namespace Dx12;
using namespace std;



void Screen::InitScreen(HWND hWnd, uint32_t width, uint32_t height, DXGI_FORMAT format)
{
	m_hWnd   = hWnd;
	m_Width  = width;
	m_Height = height;
	m_Format = format;

	for (uint32_t i = 0; i < BackBufferCount; ++i)
	{
		m_FrameValues[i] = 0;
		m_FenceValues[i] = 0;
	}

	CreateSwapChain();
	UpdateRenderTarget();
}
void Screen::Resize(uint32_t width, uint32_t height)
{
	if (m_Width != width || m_Height != height)
	{
		DxDevice::Flush();

		m_Width = std::max<uint32_t>(1, width);
		m_Height = std::max<uint32_t>(1, height);

		for (int i = 0; i < 3; ++i)
		{
			m_BackBuffer[i].Reset();
		}
		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		ThrowIfFailed(m_SwapChain->GetDesc(&swapChainDesc));
		ThrowIfFailed(m_SwapChain->ResizeBuffers(BackBufferCount, m_Width,
			m_Height, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags));
		UpdateRenderTarget();

	}
}
void Screen::Present(const Texture& texture, const DirectX::XMFLOAT4& clearColor)
{
	auto commandQue = DxDevice::GetCommandQueue();
	commandQue->Flush();


	auto commandList = commandQue->GetCommandList();
	int CurrBackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();
	auto& BackBuffer = m_BackBuffer[CurrBackBufferIndex];


	RenderTarget rendertarget;
	rendertarget.AttachTexture(RtvSlot::Slot_0, BackBuffer);
	rendertarget.SetRenderTargetClearColor(RtvSlot::Slot_0, clearColor);


	commandList->ClearRenderTarget(rendertarget);

	if (texture.IsValid())
	{
		commandList->CopyResource(BackBuffer, texture);
	}

	commandList->TransitionBarrier(BackBuffer, D3D12_RESOURCE_STATE_PRESENT);

	commandList->Close();
	commandQue->ExcuteCommandList(commandList);
	

	m_FenceValues[CurrBackBufferIndex] = commandQue->Signal();
	m_FrameValues[CurrBackBufferIndex] = DxDevice::GetFrameCount();
	commandQue->WaitForFenceValue(m_FenceValues[CurrBackBufferIndex]);
	DxDevice::ReleaseStaleDescriptors(m_FrameValues[CurrBackBufferIndex]);



	m_SwapChain->Present(0, 0);

	commandList->FlushTrackObject();
}

void Screen::UpdateRenderTarget()
{
	for (int i = 0; i < BackBufferCount; ++i)
	{
		ComPtr<ID3D12Resource> d3d_resource;
		m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&d3d_resource));

		m_BackBuffer[i].SetD3DResource(d3d_resource, D3D12_RESOURCE_STATE_PRESENT);
		m_BackBuffer[i].SetName("BackBufferResource" + to_string(i));
		m_BackBuffer[i].SetUsage(TextureUsage::RenderTarget);
	}
}
void Screen::CreateSwapChain()
{
	auto device = DxDevice::GetDevice();
	auto CmdQue = DxDevice::GetCommandQueue();
	ComPtr<IDXGIFactory4>   dxgiFactory4;


	UINT createFactoryFlags = 0;
#if defined(_DEVELOP_DEBUG)
	createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
	ThrowIfFailed(CreateDXGIFactory2(
		createFactoryFlags,
		IID_PPV_ARGS(&dxgiFactory4)));

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = m_Width;
	swapChainDesc.Height = m_Height;
	swapChainDesc.Format = m_Format;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc = { 1, 0 };
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = BackBufferCount;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	//
	swapChainDesc.Flags = 0;



	ComPtr<IDXGISwapChain1> swapChain1;
	ThrowIfFailed(dxgiFactory4->CreateSwapChainForHwnd(
		CmdQue->Get(),
		m_hWnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain1));

	ThrowIfFailed(dxgiFactory4->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER));
	ThrowIfFailed(swapChain1.As(&m_SwapChain));
}
float Screen::AspectRadio() const
{
	return (float)m_Width / (float)m_Height;
}

uint32_t Screen::GetWidth() const
{
	return m_Width;
}
uint32_t Screen::GetHeight() const
{
	return m_Height;
}
DXGI_FORMAT Screen::GetFormat() const
{
	return m_Format;
}