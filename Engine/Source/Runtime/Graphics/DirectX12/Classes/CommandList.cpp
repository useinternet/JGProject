#include "PCH/PCH.h"
#include "CommandList.h"
#include "DirectX12/DirectX12API.h"
#include "DirectX12/Classes/DynamicDescriptionAllocator.h"
#include "DirectX12/Classes/DescriptionAllocator.h"
#include "DirectX12/Classes/RootSignature.h"
#include "DirectX12/Classes/PipelineState.h"

PCommandList::PCommandList(D3D12_COMMAND_LIST_TYPE d3dType) : _commandListType(d3dType)
{
	_dx12Allocator   = HDirectX12Helper::CreateD3DCommandAllocator(HDirectXAPI::GetDevice(), _commandListType);
	_dx12CommandList = HDirectX12Helper::CreateD3DCommandList(HDirectXAPI::GetDevice(), _dx12Allocator.Get(), _commandListType);

	_resourceStateTracker = Allocate<PResourceStateTracker>();
	_dynamicDescriptionAllocator = Allocate<PDynamicDescriptionAllocator>();
	_uploadAllocator = Allocate<PUploadAllocator>();
}

void PCommandList::BackupResource(HDX12Object* d3dObj)
{
	_tempObjectList.push_back(d3dObj);
}

void PCommandList::Reset()
{
	_resourceStateTracker->Reset();
	_uploadAllocator->Reset();
	_dynamicDescriptionAllocator->Reset(true);
	_tempObjectList.clear();

	_dx12Allocator->Reset();
	_dx12CommandList->Reset(_dx12Allocator.Get(), nullptr);

	_bindedDescriptorHeap  = nullptr;
	_bindedPipelineState   = nullptr;
	_bindedGraphicsRootSig = nullptr;
	_bindedComputeRootSig  = nullptr;
}

void PCommandList::Close()
{
	_resourceStateTracker->FlushResourceBarrier(Get());
	_dx12CommandList->Close();
}

bool PCommandList::Close(PCommandList* commandList)
{
	Close();
	bool bExistBarrier = false;
	if (commandList != nullptr) {
		bExistBarrier = _resourceStateTracker->FlushPendingResourceBarrier(commandList->Get());
	}

	_resourceStateTracker->CommitResourceState();
	return bExistBarrier;
}

void PCommandList::TransitionBarrier(HDX12Resource* d3dResource, D3D12_RESOURCE_STATES state, uint32 subResource, bool bFlush)
{
	if (d3dResource == nullptr)
	{
		return;
	}

	_resourceStateTracker->TransitionBarrier(d3dResource, state, subResource);

	if (bFlush)
	{
		FlushResourceBarrier();
	}

	BackupResource(d3dResource);
}

void PCommandList::UAVBarrier(HDX12Resource* d3dResource, bool bFlush)
{
	_resourceStateTracker->UAVBarrier(d3dResource);
	if (bFlush)
	{
		FlushResourceBarrier();
	}
	BackupResource(d3dResource);
}

void PCommandList::AliasBarrier(HDX12Resource* beforeResource, HDX12Resource* afterResource, bool bFlush)
{
	_resourceStateTracker->AliasBarrier(beforeResource, afterResource);
	if (bFlush)
	{
		FlushResourceBarrier();
	}
	BackupResource(beforeResource);
	BackupResource(afterResource);
}

D3D12_GPU_DESCRIPTOR_HANDLE PCommandList::UploadDirect(D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	if (_dynamicDescriptionAllocator == nullptr || handle.ptr == 0)
	{
		return { 0 };
	}
	return _dynamicDescriptionAllocator->UploadDirect(handle, _dx12CommandList, &_bindedDescriptorHeap);
}

void PCommandList::FlushResourceBarrier()
{
	_resourceStateTracker->FlushResourceBarrier(Get());
}

void PCommandList::CopyResource(HDX12Resource* dest, HDX12Resource* src)
{
	if (dest == nullptr || src == nullptr)
	{
		return;
	}
	TransitionBarrier(dest, D3D12_RESOURCE_STATE_COPY_DEST);
	TransitionBarrier(src, D3D12_RESOURCE_STATE_COPY_SOURCE);
	FlushResourceBarrier();

	_dx12CommandList->CopyResource(dest, src);

	BackupResource(dest);
	BackupResource(src);
}

void PCommandList::CopyTextrueFromMemory(HDX12Resource* resource, const void* pixels, int32 width, int32 height, int32 channels, uint64 arraySize)
{
	if (resource == nullptr)
	{
		return;
	}

	HDX12Device* dx12Device = HDirectXAPI::GetDevice();

	uint64 uploadSize = GetRequiredIntermediateSize(resource, 0, 1);

	ComPtr<ID3D12Resource> uploadBuffer;

	CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC   resourceDesc   = CD3DX12_RESOURCE_DESC::Buffer(uploadSize * arraySize);

	HRESULT hResult = dx12Device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(uploadBuffer.GetAddressOf()));

	uploadBuffer->SetName(L"UploadBuffer");

	HList<D3D12_SUBRESOURCE_DATA> subResources;

	for (int32 i = 0; i < arraySize; ++i)
	{
		D3D12_SUBRESOURCE_DATA subResourceData = {};
		subResourceData.pData = (void*)((uint64)pixels + (uint64)(uploadSize * i));
		subResourceData.RowPitch = width * channels;
		subResourceData.SlicePitch = subResourceData.RowPitch * height;
		subResources.push_back(subResourceData);
	}

	TransitionBarrier(resource, D3D12_RESOURCE_STATE_COPY_DEST);
	FlushResourceBarrier();

	UpdateSubresources(_dx12CommandList.Get(), resource, uploadBuffer.Get(), 0, 0, (uint32)subResources.size(), subResources.data());

	BackupResource(resource);
	BackupResource(uploadBuffer.Get());
}

void PCommandList::CopyBuffer(HDX12Resource* buffer, const void* data, uint64 elementSize, uint64 elementCount)
{
	if (buffer == nullptr)
	{
		return;
	}

	HDX12Device* dx12Device = HDirectXAPI::GetDevice();
	uint64 btSize = elementSize * elementCount;

	ComPtr<ID3D12Resource> uploadBuffer;

	CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC   resourceDesc   = CD3DX12_RESOURCE_DESC::Buffer(btSize);

	HRESULT hResult = dx12Device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(uploadBuffer.GetAddressOf()));

	uploadBuffer->SetName(L"UploadBuffer");

	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData      = data;
	subResourceData.RowPitch   = (uint32_t)btSize;
	subResourceData.SlicePitch = subResourceData.RowPitch;

	TransitionBarrier(buffer, D3D12_RESOURCE_STATE_COPY_DEST);
	FlushResourceBarrier();
	UpdateSubresources(_dx12CommandList.Get(), buffer, uploadBuffer.Get(), 0, 0, 1, &subResourceData);

	BackupResource(buffer);
	BackupResource(uploadBuffer.Get());
}

void PCommandList::CopyBufferRegion(HDX12Resource* dest, uint64 destOffset, HDX12Resource* src, uint64 srcOffset, uint64 byteSize)
{
	TransitionBarrier(dest, D3D12_RESOURCE_STATE_COPY_DEST);
	TransitionBarrier(src, D3D12_RESOURCE_STATE_COPY_SOURCE);
	FlushResourceBarrier();

	_dx12CommandList->CopyBufferRegion(dest, destOffset, src, srcOffset, byteSize);

	BackupResource(dest);
	BackupResource(src);
}

void PCommandList::CopyTextureRegion(HDX12Resource* dest, HDX12Resource* src, const HRect& srcBox, D3D12_RESOURCE_STATES inDestState, D3D12_RESOURCE_STATES inSrcState)
{
	TransitionBarrier(dest, D3D12_RESOURCE_STATE_COPY_DEST);
	TransitionBarrier(src, D3D12_RESOURCE_STATE_COPY_SOURCE);
	FlushResourceBarrier();

	CD3DX12_TEXTURE_COPY_LOCATION copyDest(dest, 0);
	CD3DX12_TEXTURE_COPY_LOCATION copySrc(src, 0);

	CD3DX12_BOX regionBox = CD3DX12_BOX(0, 0, (int32)srcBox.Width(), (int32)srcBox.Height());

	_dx12CommandList->CopyTextureRegion(&copyDest, 0, 0, 0, &copySrc, &regionBox);

	TransitionBarrier(dest, inDestState);
	TransitionBarrier(src, inSrcState);
	FlushResourceBarrier();

	BackupResource(dest);
	BackupResource(src);
}

PGraphicsCommandList::PGraphicsCommandList(D3D12_COMMAND_LIST_TYPE d3dType) : PCommandList(d3dType)
{
	//_computeDynamicDescriptorAllocator = CreateSharedPtr<DynamicDescriptorAllocator>();
}

void PGraphicsCommandList::Reset()
{
	PCommandList::Reset();
	//_computeDynamicDescriptorAllocator->Reset(true);
	_vertexViews.clear();
}

void PGraphicsCommandList::SetViewport(const HViewport& viewport)
{
	CD3DX12_VIEWPORT dx12Viewport = CD3DX12_VIEWPORT(viewport.TopLeftX, viewport.TopLeftY, viewport.Width, viewport.Height, viewport.MinDepth, viewport.MaxDepth);
	_dx12CommandList->RSSetViewports(1, &dx12Viewport);
}

void PGraphicsCommandList::SetViewports(const HList<HViewport>& viewports)
{
	HList<D3D12_VIEWPORT> d3dViewports(viewports.size());
	for (int32 i = 0; i < (int32)d3dViewports.size(); ++i)
	{
		d3dViewports[i] = CD3DX12_VIEWPORT(
			viewports[i].TopLeftX, viewports[i].TopLeftY,
			viewports[i].Width, viewports[i].Height,
			viewports[i].MinDepth, viewports[i].MaxDepth);
	}

	_dx12CommandList->RSSetViewports((uint32)d3dViewports.size(), d3dViewports.data());
}

void PGraphicsCommandList::SetScissorRect(const HScissorRect& rect)
{
	CD3DX12_RECT dx12Rect = CD3DX12_RECT(rect.Left, rect.Top, rect.Right, rect.Bottom);
	_dx12CommandList->RSSetScissorRects(1, &dx12Rect);
}

void PGraphicsCommandList::SetScissorRects(const HList<HScissorRect>& rects)
{
	HList<D3D12_RECT> d3dRects(rects.size());
	for (int32 i = 0; i < (int32)d3dRects.size(); ++i)
	{
		d3dRects[i] = CD3DX12_RECT(rects[i].Left, rects[i].Top, rects[i].Right, rects[i].Bottom);
	}

	_dx12CommandList->RSSetScissorRects((uint32)d3dRects.size(), d3dRects.data());
}

void PGraphicsCommandList::ClearRenderTargetTexture(HDX12Resource* resource, D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, const HColor& clearColor)
{
	if (resource == nullptr || rtvHandle.ptr == 0) return;

	TransitionBarrier(resource, D3D12_RESOURCE_STATE_RENDER_TARGET);
	FlushResourceBarrier();

	float32 color[4] = { (float32)clearColor.R, (float32)clearColor.G, (float32)clearColor.B, (float32)clearColor.A };
	_dx12CommandList->ClearRenderTargetView(rtvHandle, color, 0, nullptr);
}

void PGraphicsCommandList::ClearDepthTexture(HDX12Resource* resource, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, float32 clearDepth, uint8 clearStencil, D3D12_CLEAR_FLAGS clearFlags)
{
	if (resource == nullptr || dsvHandle.ptr == 0) return;

	TransitionBarrier(resource, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	FlushResourceBarrier();

	_dx12CommandList->ClearDepthStencilView(dsvHandle, clearFlags, clearDepth, clearStencil, 0, nullptr);
}

void PGraphicsCommandList::ClearUAVUint(D3D12_CPU_DESCRIPTOR_HANDLE handle, HDX12Resource* resource)
{
	D3D12_GPU_DESCRIPTOR_HANDLE gpu = _dynamicDescriptionAllocator->UploadDirect(handle, _dx12CommandList, &_bindedDescriptorHeap);
	const uint32 ClearColor[4] = {};

	_dx12CommandList->ClearUnorderedAccessViewUint(gpu, handle, resource, ClearColor, 0, nullptr);
}

void PGraphicsCommandList::ClearUAVFloat(D3D12_CPU_DESCRIPTOR_HANDLE handle, HDX12Resource* resource)
{
	D3D12_GPU_DESCRIPTOR_HANDLE gpu = _dynamicDescriptionAllocator->UploadDirect(handle, _dx12CommandList, &_bindedDescriptorHeap);
	const float32 ClearColor[4] = {};

	_dx12CommandList->ClearUnorderedAccessViewFloat(gpu, handle, resource, ClearColor, 0, nullptr);
}
void PGraphicsCommandList::SetRenderTarget(
	HDX12Resource** rtTextures, D3D12_CPU_DESCRIPTOR_HANDLE* rtvHandles, uint64 rtTextureCount,
	HDX12Resource* depthTexture, D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle)
{
	for (uint64 i = 0; i < rtTextureCount; ++i)
	{
		if (rtTextures[i] == nullptr) continue;
		TransitionBarrier(rtTextures[i], D3D12_RESOURCE_STATE_RENDER_TARGET);
	}
	if (depthTexture != nullptr)
	{
		TransitionBarrier(depthTexture, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	}

	FlushResourceBarrier();

	_dx12CommandList->OMSetRenderTargets((uint32)rtTextureCount, rtvHandles, false, dsvHandle);
}

void PGraphicsCommandList::BindRootSignature(PSharedPtr<PRootSignature> rootSig)
{
	if (_bindedGraphicsRootSig.Get() == rootSig->Get())
	{
		return;
	}
	_bindedGraphicsRootSig = rootSig->Get();
	_dx12CommandList->SetGraphicsRootSignature(_bindedGraphicsRootSig.Get());
	_dynamicDescriptionAllocator->CommitRootSignature(*rootSig);
}

void PGraphicsCommandList::BindPipelineState(PSharedPtr<PGraphicsPipelineState> pso)
{
	if (_bindedPipelineState.Get() == pso->Get())
	{
		return;
	}
	_bindedPipelineState = pso->Get();
	_dx12CommandList->SetPipelineState(_bindedPipelineState.Get());
}

void PGraphicsCommandList::BindTextures(uint32 rootParam, HList<D3D12_CPU_DESCRIPTOR_HANDLE> handles)
{
	int32 initType = _dynamicDescriptionAllocator->GetDescriptorInitAsType(rootParam);
	switch (initType)
	{
	case PRootSignature::__DescriptorTable__:
	{
		D3D12_DESCRIPTOR_RANGE_TYPE tableType = _dynamicDescriptionAllocator->GetDescriptorTableType(rootParam);
		switch (tableType)
		{
		case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
		case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
			_dynamicDescriptionAllocator->CommitDescriptorTable(rootParam, handles);
			break;
		default:
			JG_ASSERT("trying bind CBV or Sampler in BindTextures");
			break;
		}
	}
	break;
	case PRootSignature::__ShaderResourceView__:
		break;
	case PRootSignature::__UnorderedAccessView__:
		break;
	default:
		JG_ASSERT("BindTextures not support ConstantBufferView / Constant");
		break;
	}
}

void PGraphicsCommandList::BindConstantBuffer(uint32 rootParam, PUploadAllocator::HAllocation alloc)
{
	BindConstantBuffer(rootParam, alloc.GPU, alloc.OwnerPage->Get());
}

void PGraphicsCommandList::BindConstantBuffer(uint32 rootParam, const void* data, uint64 dataSize)
{
	PUploadAllocator::HAllocation alloc = _uploadAllocator->Allocate(dataSize, 256);
	memcpy(alloc.CPU, data, dataSize);

	BindConstantBuffer(rootParam, alloc.GPU, alloc.OwnerPage->Get());
}

void PGraphicsCommandList::BindConstantBuffer(uint32 rootParam, D3D12_GPU_VIRTUAL_ADDRESS gpu, ID3D12Resource* backUpResource)
{
	int32 initType = _dynamicDescriptionAllocator->GetDescriptorInitAsType(rootParam);
	if (initType == PRootSignature::__ConstantBufferView__)
	{
		_dx12CommandList->SetGraphicsRootConstantBufferView(rootParam, gpu);
	}
	else
	{
		JG_ASSERT("BindConstantBuffer not support ShaderResourceView / UnorderedAccessView / Constant / DescriptorTable");
	}

	if (backUpResource != nullptr)
	{
		BackupResource(backUpResource);
	}
}

void PGraphicsCommandList::BindStructuredBuffer(uint32 rootParam, PUploadAllocator::HAllocation alloc)
{
	BindStructuredBuffer(rootParam, alloc.GPU, alloc.OwnerPage->Get());
}

void PGraphicsCommandList::BindStructuredBuffer(uint32 rootParam, const void* data, uint64 elementCount, uint64 elementSize)
{
	uint64 dataSize = elementCount * elementSize;
	PUploadAllocator::HAllocation alloc = _uploadAllocator->Allocate(dataSize, elementSize);
	memcpy(alloc.CPU, data, dataSize);

	BindStructuredBuffer(rootParam, alloc.GPU, alloc.OwnerPage->Get());
}

void PGraphicsCommandList::BindStructuredBuffer(uint32 rootParam, D3D12_GPU_VIRTUAL_ADDRESS gpu, ID3D12Resource* backUpResource)
{
	int32 initType = _dynamicDescriptionAllocator->GetDescriptorInitAsType(rootParam);
	switch (initType)
	{
	case PRootSignature::__ShaderResourceView__:
		_dx12CommandList->SetGraphicsRootShaderResourceView(rootParam, gpu);
		break;
	case PRootSignature::__UnorderedAccessView__:
		_dx12CommandList->SetGraphicsRootUnorderedAccessView(rootParam, gpu);
		break;
	default:
		assert("BindStructuredBuffer not support ConstantBufferView / Constant / DescriptorTable");
		break;
	}

	if (backUpResource != nullptr)
	{
		BackupResource(backUpResource);
	}
}

void PGraphicsCommandList::BindConstants(uint32 rootParam, uint32 btSize, const void* data, uint32 offset)
{
	int32 initType = _dynamicDescriptionAllocator->GetDescriptorInitAsType(rootParam);
	switch (initType)
	{
	case PRootSignature::__Constant__:
		_dx12CommandList->SetGraphicsRoot32BitConstants(rootParam, btSize / 4, data, offset / 4);
		break;
	default:
		assert("BindConstants not support CBV / SRV / UAV /DescriptorTable");
		break;
	}
}

void PGraphicsCommandList::BindVertexBuffer(const D3D12_VERTEX_BUFFER_VIEW& view, bool bFlush)
{
	_vertexViews.push_back(view);
	if (bFlush == true)
	{
		FlushVertexBuffer();
	}
}

void PGraphicsCommandList::FlushVertexBuffer()
{
	_dx12CommandList->IASetVertexBuffers(0, (uint32)_vertexViews.size(), _vertexViews.data());
	_vertexViews.clear();
}

void PGraphicsCommandList::BindIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& view)
{
	_dx12CommandList->IASetIndexBuffer(&view);
}

void PGraphicsCommandList::SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology)
{
	_dx12CommandList->IASetPrimitiveTopology(topology);
}

void PGraphicsCommandList::DrawIndexed(uint32 indexCount, uint32 instancedCount, uint32 startIndexLocation, uint32 startVertexLocation, uint32 startInstanceLocation)
{
	_bindedDescriptorHeap = _dynamicDescriptionAllocator->PushDescriptorTable(_dx12CommandList, _bindedDescriptorHeap, true);
	_dx12CommandList->DrawIndexedInstanced(indexCount, instancedCount, startIndexLocation, startVertexLocation, startInstanceLocation);
}
void PGraphicsCommandList::Draw(uint32 vertexPerInstance, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation)
{
	_bindedDescriptorHeap = _dynamicDescriptionAllocator->PushDescriptorTable(_dx12CommandList, _bindedDescriptorHeap, true);
	_dx12CommandList->DrawInstanced(vertexPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
}

//PSharedPtr<ComputeCommandList> GraphicsCommandList::QueryInterfaceAsComputeCommandList()
//{
//	SharedPtr<ComputeCommandList> result = CreateSharedPtr<ComputeCommandList>();
//
//	// 공유할건 공유하고 따로 만들건 따로 만들기
//	result->mD3DType = mD3DType;
//	result->mD3DCommandList = mD3DCommandList;
//	result->mD3DAllocator = mD3DAllocator;
//
//	result->mTempObjectList = mTempObjectList;
//	result->mResourceStateTracker = mResourceStateTracker;
//	result->mUploadAllocator = mUploadAllocator;
//	result->mDynamicDescriptorAllocator = mComputeDynamicDescriptorAllocator;
//
//	result->mBindedDescriptorHeap = mBindedDescriptorHeap;
//	result->mBindedComputeRootSig = mBindedComputeRootSig;
//	result->mBindedPipelineState = mBindedPipelineState;
//
//	return result;
//}

void PComputeCommandList::ClearUAVUint(D3D12_CPU_DESCRIPTOR_HANDLE handle, HDX12Resource* resource)
{
	D3D12_GPU_DESCRIPTOR_HANDLE gpu = _dynamicDescriptionAllocator->UploadDirect(handle, _dx12CommandList, &_bindedDescriptorHeap);
	const uint32 ClearColor[4] = {};
	_dx12CommandList->ClearUnorderedAccessViewUint(gpu, handle, resource, ClearColor, 0, nullptr);
}

void PComputeCommandList::ClearUAVFloat(D3D12_CPU_DESCRIPTOR_HANDLE handle, HDX12Resource* resource)
{
	D3D12_GPU_DESCRIPTOR_HANDLE gpu = _dynamicDescriptionAllocator->UploadDirect(handle, _dx12CommandList, &_bindedDescriptorHeap);
	const float32 ClearColor[4] = {};
	_dx12CommandList->ClearUnorderedAccessViewFloat(gpu, handle, resource, ClearColor, 0, nullptr);
}

void PComputeCommandList::BindRootSignature(PSharedPtr<PRootSignature> rootSig)
{
	if (_bindedComputeRootSig.Get() == rootSig->Get())
	{
		return;
	}
	_bindedComputeRootSig = rootSig->Get();
	_dx12CommandList->SetComputeRootSignature(_bindedComputeRootSig.Get());
	_dynamicDescriptionAllocator->CommitRootSignature(*rootSig);
}
void PComputeCommandList::BindPipelineState(PSharedPtr<PComputePipelineState> pso)
{
	if (_bindedPipelineState.Get() == pso->Get())
	{
		return;
	}
	_bindedPipelineState = pso->Get();
	_dx12CommandList->SetPipelineState(_bindedPipelineState.Get());
}

void PComputeCommandList::BindPipelineState(HDX12StateObject* pso)
{
	_dx12CommandList->SetPipelineState1(pso);
	BackupResource(pso);
}

void PComputeCommandList::BindTextures(uint32 rootParam, HList<D3D12_CPU_DESCRIPTOR_HANDLE> handles)
{
	int32 initType = _dynamicDescriptionAllocator->GetDescriptorInitAsType(rootParam);

	switch (initType)
	{
	case PRootSignature::__DescriptorTable__:
	{
		D3D12_DESCRIPTOR_RANGE_TYPE tableType = _dynamicDescriptionAllocator->GetDescriptorTableType(rootParam);
		switch (tableType)
		{
		case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
		case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
			_dynamicDescriptionAllocator->CommitDescriptorTable(rootParam, handles);
			break;
		default:
			JG_ASSERT("trying bind CBV or Sampler in BindTextures");
			break;
		}
	}
	break;
	default:
		JG_ASSERT("BindTextures not support ConstantBufferView / Constant");
		break;
	}
}
void PComputeCommandList::BindConstantBuffer(uint32 rootParam, PUploadAllocator::HAllocation alloc)
{
	BindConstantBuffer(rootParam, alloc.GPU, alloc.OwnerPage->Get());
}

void PComputeCommandList::BindConstantBuffer(uint32 rootParam, const void* data, uint64 dataSize)
{
	PUploadAllocator::HAllocation alloc = _uploadAllocator->Allocate(dataSize, 256);
	memcpy(alloc.CPU, data, dataSize);

	BindConstantBuffer(rootParam, alloc.GPU, alloc.OwnerPage->Get());
}
void PComputeCommandList::BindConstantBuffer(uint32 rootParam, D3D12_GPU_VIRTUAL_ADDRESS gpu, HDX12Resource* backUpResource)
{
	int32 initType = _dynamicDescriptionAllocator->GetDescriptorInitAsType(rootParam);
	if (initType == PRootSignature::__ConstantBufferView__)
	{
		_dx12CommandList->SetComputeRootConstantBufferView(rootParam, gpu);
	}
	else
	{
		JG_ASSERT("BindConstantBuffer not support ShaderResourceView / UnorderedAccessView / Constant / DescriptorTable");
	}

	if (backUpResource != nullptr)
	{
		BackupResource(backUpResource);
	}
}
void PComputeCommandList::BindStructuredBuffer(uint32 rootParam, PUploadAllocator::HAllocation alloc)
{
	BindStructuredBuffer(rootParam, alloc.GPU, alloc.OwnerPage->Get());
}
void PComputeCommandList::BindStructuredBuffer(uint32 rootParam, const void* data, uint64 elementCount, uint64 elementSize)
{
	uint64 dataSize = elementCount * elementSize;
	PUploadAllocator::HAllocation alloc = _uploadAllocator->Allocate(dataSize, elementSize);
	memcpy(alloc.CPU, data, dataSize);

	BindStructuredBuffer(rootParam, alloc.GPU, alloc.OwnerPage->Get());
}
void PComputeCommandList::BindStructuredBuffer(uint32 rootParam, D3D12_GPU_VIRTUAL_ADDRESS gpu, HDX12Resource* backUpResource)
{
	int32 initType = _dynamicDescriptionAllocator->GetDescriptorInitAsType(rootParam);
	switch (initType)
	{
	case PRootSignature::__ShaderResourceView__:
		_dx12CommandList->SetComputeRootShaderResourceView(rootParam, gpu);
		break;
	case PRootSignature::__UnorderedAccessView__:
		_dx12CommandList->SetComputeRootUnorderedAccessView(rootParam, gpu);
		break;
	default:
		assert("BindStructuredBuffer not support ConstantBufferView / Constant / DescriptorTable");
		break;
	}
	if (backUpResource != nullptr)
	{
		BackupResource(backUpResource);
	}
}
void PComputeCommandList::BindStructuredBuffer(uint32 rootParam, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	int32 initType = _dynamicDescriptionAllocator->GetDescriptorInitAsType(rootParam);
	switch (initType)
	{
	case PRootSignature::__DescriptorTable__:
	{
		D3D12_DESCRIPTOR_RANGE_TYPE tableType = _dynamicDescriptionAllocator->GetDescriptorTableType(rootParam);
		switch (tableType)
		{
		case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
		case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
			_dynamicDescriptionAllocator->CommitDescriptorTable(rootParam, { handle });
			break;
		default:
			JG_ASSERT("trying bind CBV or Sampler in BindStructuredBuffer");
			break;
		}
	}
	break;
	default:
		JG_ASSERT("BindStructuredBuffer not support ConstantBufferView / Constant");
		break;
	}
}
void PComputeCommandList::BindConstants(uint32 rootparam, uint32 btSize, const void* data, uint32 offset)
{
	int32 initType = _dynamicDescriptionAllocator->GetDescriptorInitAsType(rootparam);
	switch (initType)
	{
	case PRootSignature::__Constant__:
		_dx12CommandList->SetComputeRoot32BitConstants(rootparam, btSize / 4, data, offset / 4);
		break;
	default:
		JG_ASSERT("BindConstants not support CBV / SRV / UAV /DescriptorTable");
		break;
	}
}

void PComputeCommandList::Dispatch(uint32 groupX, uint32 groupY, uint32 groupZ)
{
	_bindedDescriptorHeap = _dynamicDescriptionAllocator->PushDescriptorTable(_dx12CommandList, _bindedDescriptorHeap, false);
	_dx12CommandList->Dispatch(groupX, groupY, groupZ);
}

void PComputeCommandList::DispatchRays(const D3D12_DISPATCH_RAYS_DESC& desc)
{
	_bindedDescriptorHeap = _dynamicDescriptionAllocator->PushDescriptorTable(_dx12CommandList, _bindedDescriptorHeap, false);
	_dx12CommandList->DispatchRays(&desc);

}