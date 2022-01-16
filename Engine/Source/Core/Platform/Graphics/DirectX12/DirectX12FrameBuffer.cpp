#include "pch.h"
#include "DirectX12FrameBuffer.h"
#include "DirectX12API.h"
#include "Utill/CommandQueue.h"
#include "Utill/CommandList.h"
#include "Imgui/imgui.h"

namespace JG
{

	bool DirectX12FrameBuffer::Init(const FrameBufferInfo& info)
	{
		u64 buffCount = DirectX12API::GetInstance()->GetBufferCount();
		
		mInfo = info;
		mSwapChain = CreateDXGISwapChain(
			(HWND)mInfo.Handle,
			DirectX12API::GetDXGIFactory(),
			DirectX12API::GetGraphicsCommandQueue()->Get(),
			ConvertDXGIFormat(mInfo.Format),
			mInfo.Width, mInfo.Height,
			(u32)buffCount);


		if (mSwapChain == nullptr)
		{
			JG_CORE_CRITICAL("Failed Create SwapChain in DirectX12RenderContext");
			return false;
		}
		mBackBuffers.resize(buffCount);
		mRTVs.resize(buffCount);
		for (u64 i = 0; i < buffCount; ++i)
		{
			mRTVs[i] = DirectX12API::RTVAllocate();
			mSwapChain->GetBuffer((u32)i, IID_PPV_ARGS(mBackBuffers[i].GetAddressOf()));
			DirectX12API::GetD3DDevice()->CreateRenderTargetView(mBackBuffers[i].Get(), nullptr, mRTVs[i].CPU());
			mBackBuffers[i]->SetName((TT("FrameBufferResource_") + std::to_wstring(i)).c_str());
		}
		return true;
	}

	void DirectX12FrameBuffer::Update()
	{
		u32 index = mSwapChain->GetCurrentBackBufferIndex();
		auto& backBuffer = mBackBuffers[index];


		auto commandQueue = DirectX12API::GetGraphicsCommandQueue();
		auto commandList = commandQueue->RequestCommandList();

		commandList->Get()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		// TODO
		// 설정 값으로
		f32 clearColor[4] = { mInfo.ClearColor.R,mInfo.ClearColor.G, mInfo.ClearColor.B, mInfo.ClearColor.A };
		commandList->Get()->ClearRenderTargetView(mRTVs[index].CPU(), clearColor, 0, nullptr);




		D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_RENDER_TARGET;
		if (mSubmittedTexture.expired() == false)
		{
			auto submittedTexture = static_cast<DirectX12Texture*>(mSubmittedTexture.lock().get());

			if (submittedTexture != nullptr && submittedTexture->IsValid())
			{
				commandList->Get()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
							backBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_DEST));

				commandList->TransitionBarrier(submittedTexture->Get(), D3D12_RESOURCE_STATE_COPY_SOURCE);
				commandList->FlushResourceBarrier();


				commandList->Get()->CopyResource(backBuffer.Get(), submittedTexture->Get());
				state = D3D12_RESOURCE_STATE_COPY_DEST;
			}
		}

		for (auto& callBack : mUpdateCallBackList)
		{
			state = callBack(commandList->Get(), backBuffer.Get(), mRTVs[index].CPU(), state);
		}

		commandList->Get()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffer.Get(), state, D3D12_RESOURCE_STATE_PRESENT));
	}

	bool DirectX12FrameBuffer::Present()
	{
		for (auto& callBack : mPresentCallBackList)
		{
			callBack();
		}
		HRESULT hResult = mSwapChain->Present(0, 0);
		return SUCCEEDED(hResult);
	}
	void DirectX12FrameBuffer::SubmitTexture(SharedPtr<ITexture> texture)
	{
		mSubmittedTexture = texture;
	}

	void DirectX12FrameBuffer::Resize(u32 width, u32 height)
	{
		u64 buffCount = DirectX12API::GetInstance()->GetBufferCount();

		if (mInfo.Width == width && mInfo.Height == height) return;
		DirectX12API::GetGraphicsCommandQueue()->Flush();

		for (u64 i = 0; i < buffCount; ++i)
		{
			mBackBuffers[i].Reset();
		}

		DXGI_SWAP_CHAIN_DESC1 desc;
		mSwapChain->GetDesc1(&desc);
		desc.Width  = std::max<u32>(1, width);
		desc.Height = std::max<u32>(1, height);

		mInfo.Width  = desc.Width;
		mInfo.Height = desc.Height;
		mSwapChain->ResizeBuffers((u32)buffCount, desc.Width, desc.Height, desc.Format, desc.Flags);

		for (u64 i = 0; i < buffCount; ++i)
		{
			mSwapChain->GetBuffer((u32)i, IID_PPV_ARGS(mBackBuffers[i].GetAddressOf()));
			DirectX12API::GetD3DDevice()->CreateRenderTargetView(mBackBuffers[i].Get(), nullptr, mRTVs[i].CPU());
		}

	}

	const FrameBufferInfo& DirectX12FrameBuffer::GetInfo() const
	{
		return mInfo;
	}

	void DirectX12FrameBuffer::AddUpdateCallBack(const DirectX12FrameBufferUpdateCallBack& callBack)
	{
		mUpdateCallBackList.push_back(callBack);
	}

	void DirectX12FrameBuffer::AddPresentCallBack(const DirectX12FrameBufferPresentCallBack& callBack)
	{
		mPresentCallBackList.push_back(callBack);
	}

	void DirectX12FrameBuffer::Reset()
	{
		mBackBuffers.clear();
		mRTVs.clear();
		mSwapChain.Reset();
		mSubmittedTexture.reset(); 
	}




}