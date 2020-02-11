#include "pch.h"
#include "DxScreen.h"
#include "DxUtill.h"
#include "RenderDevice.h"
#include "CommandQueue.h"
#include "CommandList.h"


using namespace std;


namespace RE
{

	void DxScreen::Init(const DxScreenDesc& desc)
	{
		m_Desc = desc;
		
		m_SwapChain = CreateDXGISwapChain(
			desc.hWnd, 
			GetRenderDevice()->GetDxgiFactory(),
			GetRenderDevice()->GetDirectCmdQueue()->GetD3DCommandQueue(),
			desc.format,
			desc.width,
			desc.height,
			desc.bufferCount);

		m_BackBuffer.resize(desc.bufferCount);
		UpdateBackBuffer();
		m_FinalTexture = make_shared<Texture>();
	}
	void DxScreen::SetFinalTexture(const Texture& texture)
	{
		m_FinalTexture = make_shared<Texture>(texture);
	}
	void DxScreen::Update(CommandList* cmdList)
	{
		auto index = m_SwapChain->GetCurrentBackBufferIndex();
		auto& back_buffer = m_BackBuffer[index];
		

		cmdList->ClearRenderTarget(*back_buffer, JColor(0.0f, 0.0f, 0.0f, 0.0f));


		if (m_FinalTexture->IsVaild())
		{
			cmdList->CopyResource(*back_buffer, *m_FinalTexture);
		}


		cmdList->SetRenderTarget(*back_buffer);
		cmdList->FlushResourceBarrier();


		// gui
		cmdList->TransitionBarrier(*back_buffer, D3D12_RESOURCE_STATE_PRESENT);
		cmdList->DeleteTemporaryResource(*back_buffer);
	}
	void DxScreen::Present()
	{
		HRESULT hr = m_SwapChain->Present(0, 0);
	}

	void DxScreen::ReSize(uint32_t width, uint32_t height)
	{
		m_Desc.width = width;
		m_Desc.height = height;
		GetRenderDevice()->Flush();


		for (auto& resource : m_BackBuffer)
		{
			resource->Reset();
		}

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		m_SwapChain->GetDesc(&swapChainDesc);


		HRESULT hr = m_SwapChain->ResizeBuffers(
			swapChainDesc.BufferCount,
			width, height,
			swapChainDesc.BufferDesc.Format,
			swapChainDesc.Flags);


		UpdateBackBuffer();
	}

	void DxScreen::UpdateBackBuffer()
	{
		for (uint32_t i = 0; i < m_Desc.bufferCount; ++i)
		{
			m_BackBuffer[i] = make_shared<Texture>();
			ComPtr<ID3D12Resource> d3d_resource;
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(d3d_resource.GetAddressOf()));
			m_BackBuffer[i]->SetName(GetName() + "_BackBufferResource" + to_string(i));
			m_BackBuffer[i]->SetD3DResource(d3d_resource, D3D12_RESOURCE_STATE_COMMON);
		}
	}
}