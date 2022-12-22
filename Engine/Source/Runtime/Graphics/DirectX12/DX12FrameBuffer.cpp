#include "PCH/PCH.h"
#include "DX12FrameBuffer.h"
#include "DX12Texture.h"
#include "DirectX12API.h"
#include "JGGraphicsHelper.h"
#include "Classes/CommandQueue.h"
#include "Classes/CommandList.h"

PDX12FrameBuffer::~PDX12FrameBuffer()
{
}

bool PDX12FrameBuffer::Initialize(const HFrameBufferInfo& info)
{
	_frameBufferInfo = info;

	_dx12SwapChain = HDirectX12Helper::CreateDXGISwapChain(
		(HWND)_frameBufferInfo.Handle,
		HDirectXAPI::GetFactory(),
		HDirectXAPI::GetCommandQueue()->Get(),
		HDirectX12Helper::ConvertDXGIFormat(_frameBufferInfo.Format),
		_frameBufferInfo.Width, _frameBufferInfo.Height,
		_frameBufferInfo.FrameBufferCount);

	if (_dx12SwapChain == nullptr)
	{
		JG_LOG(Graphics, ELogLevel::Error, "Failed Create SwapChain in DirectX12RenderContext");
		return false;
	}

	_dx12BackBuffers.resize(_frameBufferInfo.FrameBufferCount);
	_rtvs.resize(_frameBufferInfo.FrameBufferCount);

	for (uint64 i = 0; i < _frameBufferInfo.FrameBufferCount; ++i)
	{
		_rtvs[i] = HDirectXAPI::RTVAllocate();

		_dx12SwapChain->GetBuffer((uint32)i, IID_PPV_ARGS(_dx12BackBuffers[i].GetAddressOf()));

		HDirectXAPI::GetDevice()->CreateRenderTargetView(_dx12BackBuffers[i].Get(), nullptr, _rtvs[i].CPU());

		_dx12BackBuffers[i]->SetName((TEXT("FrameBufferResource_") + std::to_wstring(i)).c_str());
	}

	return true;
}

void PDX12FrameBuffer::Update()
{
	uint32 index = _dx12SwapChain->GetCurrentBackBufferIndex();

	HDX12ComPtr<HDX12Resource>& backBuffer = _dx12BackBuffers[index];

	PSharedPtr<PCommandQueue> commandQueue = HDirectXAPI::GetCommandQueue();
	PSharedPtr<PCommandList>  commandList  = commandQueue->RequestCommandList(ECommandListType::Base, JG_UINT64_MAX);

	CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList->Get()->ResourceBarrier(1, &resourceBarrier);

	float32 clearColor[4] = { _frameBufferInfo.ClearColor.R,_frameBufferInfo.ClearColor.G, _frameBufferInfo.ClearColor.B, _frameBufferInfo.ClearColor.A};
	commandList->Get()->ClearRenderTargetView(_rtvs[index].CPU(), clearColor, 0, nullptr);

	D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_RENDER_TARGET;
	if (_submittedTexture.IsValid() == true)
	{
		CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_DEST);

		commandList->Get()->ResourceBarrier(1, &resourceBarrier);

		commandList->TransitionBarrier(_submittedTexture->Get(), D3D12_RESOURCE_STATE_COPY_SOURCE);
		commandList->FlushResourceBarrier();

		commandList->Get()->CopyResource(backBuffer.Get(), _submittedTexture->Get());
		state = D3D12_RESOURCE_STATE_COPY_DEST;
	}

	OnUpdate.BroadCast(commandList->Get(), backBuffer.Get(), _rtvs[index].CPU(), state);

	resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.Get(), state, D3D12_RESOURCE_STATE_PRESENT);
	commandList->Get()->ResourceBarrier(1, &resourceBarrier);
}

bool PDX12FrameBuffer::Present()
{
	OnPresent.BroadCast();
	HRESULT hResult = _dx12SwapChain->Present(0, 0);
	return SUCCEEDED(hResult);
}

void PDX12FrameBuffer::SubmitTexture(PSharedPtr<ITexture> inTexture)
{
	_submittedTexture = Cast<PDX12Texture>(inTexture);
}

void PDX12FrameBuffer::Resize(uint32 width, uint32 height)
{
	if (_frameBufferInfo.Width == width && _frameBufferInfo.Height == height) 
		return;

	HDirectXAPI::GetCommandQueue()->Flush();

	for (uint64 i = 0; i < _frameBufferInfo.FrameBufferCount; ++i)
	{
		_dx12BackBuffers[i].Reset();
	}

	DXGI_SWAP_CHAIN_DESC1 desc;
	_dx12SwapChain->GetDesc1(&desc);
	desc.Width = std::max<uint32>(1, width);
	desc.Height = std::max<uint32>(1, height);

	_frameBufferInfo.Width = desc.Width;
	_frameBufferInfo.Height = desc.Height;
	_dx12SwapChain->ResizeBuffers((uint32)_frameBufferInfo.FrameBufferCount, desc.Width, desc.Height, desc.Format, desc.Flags);

	for (uint64 i = 0; i < _frameBufferInfo.FrameBufferCount; ++i)
	{
		_dx12SwapChain->GetBuffer((uint32)i, IID_PPV_ARGS(_dx12BackBuffers[i].GetAddressOf()));
		HDirectXAPI::GetDevice()->CreateRenderTargetView(_dx12BackBuffers[i].Get(), nullptr, _rtvs[i].CPU());
	}
}

const HFrameBufferInfo& PDX12FrameBuffer::GetInfo() const
{
	return _frameBufferInfo;
}
