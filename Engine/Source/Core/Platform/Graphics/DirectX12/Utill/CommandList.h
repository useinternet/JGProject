#pragma once

#include "JGCore.h"
#include "Graphics/GraphicsDefine.h"
#include "DirectX12Helper.h"
#include "UploadAllocator.h"
#include <Platform/Graphics/DirectX12/DirectX12Resource.h>


namespace JG
{
	class ResourceStateTracker;
	class UploadAllocator;
	class DynamicDescriptorAllocator;
	class RootSignature;
	class DirectX12Texture;
	class GraphicsPipelineState;
	class ComputePipelineState;
	class CommandList
	{
	protected:
		ComPtr<ID3D12PipelineState> mBindedPipelineState = nullptr;
		ComPtr<ID3D12RootSignature> mBindedGraphicsRootSig = nullptr;
		ComPtr<ID3D12RootSignature> mBindedComputeRootSig = nullptr;
		ComPtr<ID3D12DescriptorHeap> mBindedDescriptorHeap = nullptr;

		D3D12_COMMAND_LIST_TYPE           mD3DType;
		ComPtr<ID3D12GraphicsCommandList> mD3DCommandList;
		ComPtr<ID3D12CommandAllocator>    mD3DAllocator;

		List<ComPtr<ID3D12Object>> mTempObjectList;
		UniquePtr<ResourceStateTracker> mResourceStateTracker;
		UniquePtr<DynamicDescriptorAllocator> mDynamicDescriptorAllocator;
		UniquePtr<UploadAllocator> mUploadAllocator;
		bool mIsClose = false;
	public:
		CommandList(D3D12_COMMAND_LIST_TYPE d3dType);
		virtual ~CommandList();
	public:
		ID3D12GraphicsCommandList* Get() const {
			return mD3DCommandList.Get();
		}
		void BackupResource(ID3D12Object* d3dObj);
		virtual void Reset();
		virtual void Close();
		virtual bool Close(CommandList* commandList);
		bool IsClosing() const;
		void TransitionBarrier(ID3D12Resource* d3dResource, D3D12_RESOURCE_STATES state, u32 subResource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, bool flush = false);
		void UAVBarrier(ID3D12Resource* d3dResource, bool flush = false);
		void AliasBarrier(ID3D12Resource* beforeResource, ID3D12Resource* afterResource, bool flush = false);
		void FlushResourceBarrier();

		void CopyResource(ID3D12Resource* dest, ID3D12Resource* src);
		void CopyTextrueFromMemory(ID3D12Resource* resource, const void* pixels, i32 width, i32 height, i32 channels, u64 arraySize);
		void CopyBuffer(ID3D12Resource* buffer, const void* data, u64 elementSize, u64 elementCount);
		void CopyBufferRegion(ID3D12Resource* dest, u64 destOffset,  ID3D12Resource* src, u64 srcOffset, u64 byteSize);
	};

	class GraphicsCommandList : public CommandList
	{
	private:
		List<D3D12_VERTEX_BUFFER_VIEW> mVertexViews;
	public:
		GraphicsCommandList(D3D12_COMMAND_LIST_TYPE d3dType) : CommandList(d3dType) {};
		virtual ~GraphicsCommandList() = default;
	public:
		virtual void Reset() override;
	public:
		void SetViewport(const Viewport& viewport);
		void SetViewports(const List<Viewport>& viewports);
		void SetScissorRect(const ScissorRect& rect);
		void SetScissorRects(const List<ScissorRect>& rects);
		void ClearRenderTargetTexture(ID3D12Resource* resource, D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, const Color& clearColor);
		void ClearDepthTexture(ID3D12Resource* resource, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, 
			f32 clearDepth = 1.0f, u8 clearStencil = 0, D3D12_CLEAR_FLAGS clearFlags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL);

		void SetRenderTarget(
			ID3D12Resource** rtTextures, D3D12_CPU_DESCRIPTOR_HANDLE* rtvHandles, u64 rtTextureCount,
			ID3D12Resource* depthTexture, D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle);
		void BindRootSignature(SharedPtr<RootSignature> rootSig);
		void BindPipelineState(SharedPtr<GraphicsPipelineState> pso);

		void BindTextures(u32 rootParam, List<D3D12_CPU_DESCRIPTOR_HANDLE> handles);
		void BindConstantBuffer(u32 rootParam, UploadAllocator::Allocation alloc);
		void BindConstantBuffer(u32 rootParam, const void* data, u64 dataSize);
		void BindConstantBuffer(u32 rootParam, D3D12_GPU_VIRTUAL_ADDRESS gpu, ID3D12Resource* backUpResource = nullptr);
		void BindStructuredBuffer(u32 rootParam, UploadAllocator::Allocation alloc);
		void BindStructuredBuffer(u32 rootParam, const void* data, u64 elementCount, u64 elementSize);
		void BindStructuredBuffer(u32 rootParam, D3D12_GPU_VIRTUAL_ADDRESS gpu, ID3D12Resource* backUpResource = nullptr);
		void BindConstants(u32 rootparam, u32 btSize, const void* data, u32 offset = 0);
		void BindVertexBuffer(const D3D12_VERTEX_BUFFER_VIEW& view, bool isFlush = true);
		void FlushVertexBuffer();

		void BindIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& view);
		void SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology);


		void DrawIndexed(u32 indexCount, u32 instancedCount = 1, u32 startIndexLocation = 0, u32 startVertexLocation = 0, u32 startInstanceLocation = 0);
	};

	class ComputeCommandList : public CommandList
	{
	public:
		ComputeCommandList(D3D12_COMMAND_LIST_TYPE d3dType) : CommandList(d3dType) {};
		virtual ~ComputeCommandList() = default;
	public:
		void BindRootSignature(SharedPtr<RootSignature> rootSig);
		void BindPipelineState(SharedPtr<ComputePipelineState> pso);
		void BindTextures(u32 rootParam, List<D3D12_CPU_DESCRIPTOR_HANDLE> handles);
		void BindConstantBuffer(u32 rootParam, UploadAllocator::Allocation alloc);
		void BindConstantBuffer(u32 rootParam, const void* data, u64 dataSize);
		void BindConstantBuffer(u32 rootParam, D3D12_GPU_VIRTUAL_ADDRESS gpu, ID3D12Resource* backUpResource = nullptr);
		void BindStructuredBuffer(u32 rootParam, UploadAllocator::Allocation alloc);
		void BindStructuredBuffer(u32 rootParam, const void* data, u64 elementCount, u64 elementSize);
		void BindStructuredBuffer(u32 rootParam, D3D12_GPU_VIRTUAL_ADDRESS gpu, ID3D12Resource* backUpResource = nullptr);
		void BindConstants(u32 rootparam, u32 btSize, const void* data, u32 offset = 0);
		void Dispatch(u32 groupX, u32 groupY, u32 groupZ);
	};

	class CopyCommandList : public CommandList
	{
	public:
		CopyCommandList(D3D12_COMMAND_LIST_TYPE d3dType) : CommandList(d3dType) {};
		virtual ~CopyCommandList() = default;
	};
	
}

