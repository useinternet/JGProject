#include "pch.h"
#include "CommandList.h"
#include "DxUtill.h"
#include "Resource.h"
#include "ResourceDataMap.h"
#include "UploadBuffer.h"
#include "Object/ReObject/RenderTarget.h"
#include "RootSignature.h"
#include "PipelineState.h"
using namespace std;
namespace RE
{
	CommandList::CommandList(D3D12_COMMAND_LIST_TYPE type) :
		DxObject("CommandList"),
		m_Type(type)
	{
		m_CurrRootSignatureState = RootSignatureState_None;
		m_D3D_Allocator = CreateD3DCommandAllocator(GetD3DDevice(),type);
		m_D3D_CommandList = CreateD3DCommandList(GetD3DDevice(), m_D3D_Allocator, type);


		m_ResourceDataMap = make_shared<ResourceDataMap>();
		m_UploadBuffer = make_shared<UploadBuffer>();
		m_DynamicDescriptorAllocator = make_shared<DynamicDescriptorAllocator>(GetD3DDevice());
	}     
	void CommandList::TransitionBarrier(
		const Resource& resource,
		D3D12_RESOURCE_STATES after_state,
		uint32_t subresource,
		bool flush)
	{
		m_ResourceDataMap->TransitionBarrier(resource, after_state, subresource);
		if (flush)
		{
			FlushResourceBarrier();
		}
	}
	void CommandList::UavBarrier(const Resource& resource, bool flush)
	{
		m_ResourceDataMap->UavBarrier(resource);
		if (flush)
		{
			FlushResourceBarrier();
		}
	}
	void CommandList::AliasBarrier(const Resource& beforeResource,const Resource& afterResource,bool flush)
	{
		m_ResourceDataMap->AliasBarrier(beforeResource, afterResource);
		if (flush)
		{
			FlushResourceBarrier();
		}
	}
	void CommandList::FlushResourceBarrier()
	{
		m_ResourceDataMap->FlushResourceBarrier(this);
	}
	void CommandList::DeleteTemporaryResource(const Resource& resource)
	{
		m_TemporaryResourceObject.erase(resource.GetD3DResource());
	}
	void CommandList::Reset()
	{
		HRESULT hResult = S_OK;
		hResult = m_D3D_Allocator->Reset();
		assert(SUCCEEDED(hResult) && "Failed ID3D12ComamndAllocator::Reset()");
		hResult = m_D3D_CommandList->Reset(m_D3D_Allocator.Get(), nullptr);
		assert(SUCCEEDED(hResult) && "Failed ID3D12CommandList::Close()");
		

		m_ResourceDataMap->Reset();
		m_UploadBuffer->Reset();
		m_DynamicDescriptorAllocator->Reset();
		m_TemporaryResourceObject.clear();
		m_TemporaryUploadResource.clear();

		m_D3D_Graphics_RootSignature.Reset();
		m_D3D_Compute_RootSignature.Reset();
		m_D3D_PipelineState.Reset();
		m_D3D_DescriptorHeap.Reset();

		m_CurrRootSignatureState = RootSignatureState_None;
		/*
		동적 디스크립쳐 힙 리셋
		*/
	}
	bool CommandList::Close(CommandList* pendingCmdList)
	{
		FlushResourceBarrier();

		m_D3D_CommandList->Close();

		uint32_t numPendingResourceBarrier = m_ResourceDataMap->FlushPendingResourceBarrier(pendingCmdList);
		m_ResourceDataMap->CommitResourceState();


		return numPendingResourceBarrier > 0;
	}
	void CommandList::Close()
	{
		FlushResourceBarrier();
		m_D3D_CommandList->Close();
	}
	void CommandList::CopyResource(const Resource& destResource, const Resource& sourceResource)
	{
		TransitionBarrier(destResource, D3D12_RESOURCE_STATE_COPY_DEST);
		TransitionBarrier(sourceResource, D3D12_RESOURCE_STATE_COPY_SOURCE);

		FlushResourceBarrier();


		m_D3D_CommandList->CopyResource(destResource.GetD3DResource(), sourceResource.GetD3DResource());

		ResourceTemporaryStorage(destResource);
		ResourceTemporaryStorage(sourceResource);
	}
	void CommandList::ResolveSubresource(Resource& dstRes, const Resource& srcRes, uint32_t dstSubresource, uint32_t srcSubresource)
	{
		TransitionBarrier(dstRes, D3D12_RESOURCE_STATE_RESOLVE_DEST, dstSubresource);
		TransitionBarrier(srcRes, D3D12_RESOURCE_STATE_RESOLVE_SOURCE, srcSubresource);
		FlushResourceBarrier();



		m_D3D_CommandList->ResolveSubresource(
			dstRes.GetD3DResource(), dstSubresource,
			srcRes.GetD3DResource(), srcSubresource,
			dstRes.GetDesc().Format);

		ResourceTemporaryStorage(dstRes);
		ResourceTemporaryStorage(srcRes);

	}
	void CommandList::CopyTextureSubresource(Texture& texture, uint32_t firstSubresource, uint32_t numSubresources, D3D12_SUBRESOURCE_DATA* subresourceData)
	{
		auto device = GetD3DDevice();

		if (texture.IsVaild())
		{
			// Resource must be in the copy-destination state.
			TransitionBarrier(texture, D3D12_RESOURCE_STATE_COPY_DEST);
			
			FlushResourceBarrier();

			UINT64 requiredSize = GetRequiredIntermediateSize(texture.GetD3DResource(), firstSubresource, numSubresources);

			Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource;
			GetD3DDevice()->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(requiredSize),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(intermediateResource.GetAddressOf()));

			UpdateSubresources(
				m_D3D_CommandList.Get(),
				texture.GetD3DResource(),
				intermediateResource.Get(),
				0, firstSubresource, numSubresources, subresourceData);
		
			ResourceTemporaryStorage(texture);
			UploadResourceTemporaryStorage(intermediateResource);
		}
	}
	void CommandList::SetDescriptorHeap(ID3D12DescriptorHeap* d3d_heaps)
	{
		if (m_D3D_DescriptorHeap.Get() != d3d_heaps)
		{
			m_D3D_DescriptorHeap = d3d_heaps;
			m_D3D_CommandList->SetDescriptorHeaps(1, m_D3D_DescriptorHeap.GetAddressOf());
		}
	}
	void CommandList::SetGraphicsRootSignature(const RootSignature& rootsig)
	{
		if (m_D3D_Graphics_RootSignature.Get() != rootsig.GetD3DRootSignature())
		{
			m_D3D_Graphics_RootSignature = rootsig.GetD3DRootSignature();

			m_D3D_CommandList->SetGraphicsRootSignature(rootsig.GetD3DRootSignature());
		}
		m_CurrRootSignatureState = RootSignatureState_Graphics;
	}
	void CommandList::SetComputeRootSignature(const RootSignature& rootsig)
	{
		if (m_D3D_Compute_RootSignature.Get() != rootsig.GetD3DRootSignature())
		{
			m_D3D_Compute_RootSignature = rootsig.GetD3DRootSignature();

			m_D3D_CommandList->SetComputeRootSignature(rootsig.GetD3DRootSignature());
		}
		m_CurrRootSignatureState = RootSignatureState_Compute;
	}
	void CommandList::SetPipelineState(const PipelineState& pso)
	{
		if (m_D3D_PipelineState.Get() != pso.GetD3DPipelineState())
		{
			m_D3D_PipelineState = pso.GetD3DPipelineState();
			m_D3D_CommandList->SetPipelineState(pso.GetD3DPipelineState());
		}

	}
	void CommandList::ClearRenderTarget(const Texture& texture, const JColor& color)
	{
		if (!texture.IsVaild())
		{
			return;
		}

		float clearColor[4];
		clearColor[0] = color.r;
		clearColor[1] = color.b;
		clearColor[2] = color.g;
		clearColor[3] = color.a;


 		TransitionBarrier(texture, D3D12_RESOURCE_STATE_RENDER_TARGET);

		m_D3D_CommandList->ClearRenderTargetView(texture.GetRtv(),
			clearColor, 0, nullptr);

		ResourceTemporaryStorage(texture);
	}
	void CommandList::ClearRenderTarget(const RenderTarget& rendertarget)
	{
		for (uint32_t i = 0; i < 8; ++i)
		{
			ClearRenderTarget(rendertarget.GetTexture(i), rendertarget.GetClearColor(i));
		}
		ClearDepthStencil(rendertarget.GetDepthTexture(),
			rendertarget.GetDepthClearFlags(), rendertarget.GetClearDepth(),
			rendertarget.GetClearStencil());
	}
	void CommandList::SetRenderTarget(const Texture& texture)
	{
		if (!texture.IsVaild())
		{
			return;
		}
		TransitionBarrier(texture, D3D12_RESOURCE_STATE_RENDER_TARGET);

		m_D3D_CommandList->OMSetRenderTargets(1, &texture.GetRtv(), false, nullptr);

		ResourceTemporaryStorage(texture);
	}
	void CommandList::SetRenderTarget(const RenderTarget& rendertarget)
	{
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvs;
		D3D12_CPU_DESCRIPTOR_HANDLE dsv = {};
		for (uint32_t i = 0; i < 8; ++i)
		{
			auto texture = rendertarget.GetTexture(i);
			if (texture.IsVaild())
			{
				rtvs.push_back(texture.GetRtv());
				TransitionBarrier(texture, D3D12_RESOURCE_STATE_RENDER_TARGET);
				ResourceTemporaryStorage(texture);
			}
		}

		if (rendertarget.GetDepthTexture().IsVaild())
		{
			dsv = rendertarget.GetDepthTexture().GetDsv();
		}


		if (dsv.ptr != 0)
		{
			m_D3D_CommandList->OMSetRenderTargets((uint32_t)rtvs.size(),
				rtvs.data(), false, &dsv);
		}
		else
		{
			m_D3D_CommandList->OMSetRenderTargets((uint32_t)rtvs.size(),
				rtvs.data(), false, nullptr);
		}
	}
	void CommandList::ClearDepthStencil(const Texture& texture, D3D12_CLEAR_FLAGS clearFlags,
		float depth, uint8_t stencil)
	{
		if (!texture.IsVaild())
		{
			return;
		}
		TransitionBarrier(texture, D3D12_RESOURCE_STATE_DEPTH_WRITE);

		m_D3D_CommandList->ClearDepthStencilView(texture.GetDsv(), clearFlags, depth, stencil, 0, nullptr);
		ResourceTemporaryStorage(texture);
	}
	void CommandList::BindSRV(uint32_t rootparam, const std::vector<Texture>& textures)
	{
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> handle;
	
		for (uint32_t i = 0; i < (uint32_t)textures.size(); ++i)
		{
			if (!textures[i].IsVaild())
			{
				RE_LOG_ERROR("{0} is Invalid While Bind SRV", textures[i].GetName());
				continue;
			}
			handle.push_back(textures[i].GetSrv());
		}
		m_DynamicDescriptorAllocator->PushCPUHandle(rootparam, handle);
	}
	void CommandList::BindUAV(uint32_t rootparam, const std::vector<Texture>& textures)
	{
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> handle;

		for (uint32_t i = 0; i < (uint32_t)textures.size(); ++i)
		{
			if (!textures[i].IsVaild())
			{
				RE_LOG_ERROR("{0} is Invalid While Bind UAV", textures[i].GetName());
				continue;
			}
			handle.push_back(textures[i].GetUav());
		}
		m_DynamicDescriptorAllocator->PushCPUHandle(rootparam, handle);
	}
	void CommandList::SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY primitiveTopology)
	{
		m_D3D_CommandList->IASetPrimitiveTopology(primitiveTopology);
	}
	void CommandList::BindGraphicsDynamicConstantBuffer(
		uint32_t rootparam, const std::vector<byte>& data)
	{
		auto alloc = m_UploadBuffer->Allocate(data.size(), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
		memcpy(alloc.CPU, data.data(), data.size());

		m_D3D_CommandList->SetGraphicsRootConstantBufferView(rootparam, alloc.GPU);

	}

	void CommandList::BindVertexBuffer(uint32_t slot, const VertexBuffer& vertex_buffer)
	{
		TransitionBarrier(vertex_buffer, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		auto vertex_buffer_view = vertex_buffer.GetView();

		m_D3D_CommandList->IASetVertexBuffers(slot, 1, &vertex_buffer_view);

		ResourceTemporaryStorage(vertex_buffer);

	}
	void CommandList::BindIndexBuffer(const IndexBuffer& index_buffer)
	{
		TransitionBarrier(index_buffer, D3D12_RESOURCE_STATE_INDEX_BUFFER);

		auto index_buffer_view = index_buffer.GetView();

		m_D3D_CommandList->IASetIndexBuffer(&index_buffer_view);

		ResourceTemporaryStorage(index_buffer);
	}
	void CommandList::SetViewport(const Viewport& viewport)
	{
		m_D3D_CommandList->RSSetViewports(1, &viewport.Get());
	}
	void CommandList::SetViewports(const std::vector<Viewport>& viewports)
	{
		std::vector<D3D12_VIEWPORT> d3d_viewports;


		for (auto viewport : viewports)
		{
			d3d_viewports.push_back(viewport.Get());
		}

		m_D3D_CommandList->RSSetViewports((uint32_t)d3d_viewports.size(), d3d_viewports.data());
	}
	void CommandList::SetScissorRect(const ScissorRect& rect)
	{
		m_D3D_CommandList->RSSetScissorRects(1, &rect.Get());
	}
	void CommandList::SetScissorRects(const std::vector<ScissorRect>& rects)
	{
		std::vector<D3D12_RECT> d3d_rects;


		for (auto rect : rects)
		{
			d3d_rects.push_back(rect.Get());
		}

		m_D3D_CommandList->RSSetScissorRects((uint32_t)d3d_rects.size(), d3d_rects.data());
	}
	void CommandList::Draw(uint32_t vertex_count, uint32_t instance_count, uint32_t start_vertex, uint32_t start_instance)
	{
		if (m_CurrRootSignatureState != RootSignatureState_Graphics)
		{
			RE_LOG_ERROR("RootSignature's Type is Compute or None. Please Bind RootSignature of GraphicsType.");
			return;
		}
		FlushResourceBarrier();

		m_DynamicDescriptorAllocator->Flush(this, true);

		m_D3D_CommandList->DrawInstanced(vertex_count, instance_count, start_vertex, start_instance);
	}
	void CommandList::DrawIndexed(uint32_t index_count, uint32_t instance_count, uint32_t start_index, int32_t base_vertex, uint32_t start_instance)
	{
		if (m_CurrRootSignatureState != RootSignatureState_Graphics)
		{
			RE_LOG_ERROR("RootSignature's Type is Compute or None. Please Bind RootSignature of GraphicsType.");
			return;
		}
		FlushResourceBarrier();
		m_DynamicDescriptorAllocator->Flush(this, true);
		m_D3D_CommandList->DrawIndexedInstanced(index_count, instance_count, start_index, base_vertex, start_instance);
	}
	void CommandList::Dispatch(uint32_t threadgroupX, uint32_t threadgroupY, uint32_t threadgroupZ)
	{
		if (m_CurrRootSignatureState != RootSignatureState_Compute)
		{
			RE_LOG_ERROR("RootSignature's Type is Graphics or None. Please Bind RootSignature of ComputeType.");
			return;
		}
		FlushResourceBarrier();
		m_DynamicDescriptorAllocator->Flush(this, false);
		m_D3D_CommandList->Dispatch(threadgroupX, threadgroupY, threadgroupZ);
	}
	void CommandList::CopyBuffer(
		Buffer& buffer,
		uint32_t numElements,
		uint32_t elementSize,
		const void* data,
		D3D12_RESOURCE_FLAGS flags)
	{
		auto d3d_device = GetD3DDevice();

		uint32_t buffer_size = numElements * elementSize;

		if (buffer_size == 0)
			return;

		buffer.CreateResource(CD3DX12_RESOURCE_DESC::Buffer(buffer_size, flags));


		if (data != nullptr)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> d3d_resource;
			GetD3DDevice()->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(buffer_size),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(d3d_resource.GetAddressOf()));

			D3D12_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pData = data;
			subresourceData.RowPitch = buffer_size;
			subresourceData.SlicePitch = subresourceData.RowPitch;


			TransitionBarrier(buffer, D3D12_RESOURCE_STATE_COPY_DEST);
			FlushResourceBarrier();

			UpdateSubresources(m_D3D_CommandList.Get(), buffer.GetD3DResource(),
				d3d_resource.Get(), 0, 0, 1, &subresourceData);

			UploadResourceTemporaryStorage(d3d_resource);

		}
		ResourceTemporaryStorage(buffer);
		buffer.CreateView(numElements, elementSize);
	}
	void CommandList::BindGraphicsDynamicConstantBuffer(uint32_t rootparam, uint32_t sizeInBytes, const void* bufferData)
	{
		auto alloc = m_UploadBuffer->Allocate(sizeInBytes, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
		memcpy(alloc.CPU, bufferData, sizeInBytes);

		m_D3D_CommandList->SetGraphicsRootConstantBufferView(rootparam, alloc.GPU);
	}
	void CommandList::BindGraphicsDynamicStructuredBuffer(
		uint32_t rootparam, uint32_t numElements, uint32_t elementSize, const void* bufferData)
	{

		size_t bufferSize = numElements * elementSize;

		auto alloc = m_UploadBuffer->Allocate(bufferSize, elementSize);

		memcpy(alloc.CPU, bufferData, bufferSize);

		m_D3D_CommandList->SetGraphicsRootShaderResourceView(rootparam, alloc.GPU);

	}
	void CommandList::BindDynamicVertexBuffer(uint32_t slot, uint32_t num_vertices, uint32_t vertex_size, const void* vertex_data)
	{
		uint32_t buffer_size = num_vertices * vertex_size;

		auto alloc = m_UploadBuffer->Allocate(buffer_size, vertex_size);
		memcpy(alloc.CPU, vertex_data, buffer_size);



		D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view = {};
		vertex_buffer_view.BufferLocation = alloc.GPU;
		vertex_buffer_view.SizeInBytes = buffer_size;
		vertex_buffer_view.StrideInBytes = vertex_size;

		m_D3D_CommandList->IASetVertexBuffers(slot, 1, &vertex_buffer_view);


	}
	void CommandList::BindDynamicIndexBuffer(uint32_t num_indicies, DXGI_FORMAT index_format, const void* index_data)
	{
		uint32_t index_size_inBytes = (index_format == DXGI_FORMAT_R16_UINT) ? 2 : 4;
		uint32_t buffer_size = num_indicies * index_size_inBytes;

		auto alloc = m_UploadBuffer->Allocate(buffer_size, index_size_inBytes);

		memcpy(alloc.CPU, index_data, buffer_size);

		D3D12_INDEX_BUFFER_VIEW index_buffer_view = {};
		index_buffer_view.BufferLocation = alloc.GPU;
		index_buffer_view.SizeInBytes = buffer_size;
		index_buffer_view.Format = index_format;

		m_D3D_CommandList->IASetIndexBuffer(&index_buffer_view);
	}


	void CommandList::UploadResourceTemporaryStorage(Microsoft::WRL::ComPtr<ID3D12Resource> resource)
	{
		m_TemporaryUploadResource.push_back(resource);
	}

	void CommandList::ResourceTemporaryStorage(const Resource& resource)
	{
		m_TemporaryResourceObject[resource.GetD3DResource()] = resource;
	}





	CommandList::DynamicDescriptorAllocator::GPUHeap::
		GPUHeap(ID3D12Device* d3d_device, uint32_t numDescriptor) :
		D3D_Device(d3d_device), CurrOffset(0), NumDescriptor(numDescriptor)
	{
		D3D_Heap = CreateD3DDescriptorHeap(
			d3d_device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, NumDescriptor);
		StartCpu = D3D_Heap->GetCPUDescriptorHandleForHeapStart();
		StartGpu = D3D_Heap->GetGPUDescriptorHandleForHeapStart();
	}
	CommandList::DynamicDescriptorAllocator::DynamicDescriptorAllocator(ID3D12Device* device, uint32_t numDescriptorPerHeap) :
		m_D3DDevice(device),
		m_NumDescriptorPerHeap(numDescriptorPerHeap),
		m_UsingGPUHeap(nullptr)
	{

	}

	void CommandList::DynamicDescriptorAllocator::PushCPUHandle(uint32_t rootparam, const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& handles)
	{
		if (m_NumDescriptorPerHeap < handles.size())
		{
			RE_LOG_ERROR("More CPUHnadle pushes than the maximum number of heaps.Adjust the value of  ** m_NumDescritporPerHeap ** ");
			return;
		}
		if (m_CPUCache.find(rootparam) != m_CPUCache.end())
		{
			RE_LOG_ERROR("RootParam: {0} is already set.It will be overwritten.", rootparam);
		}
		m_CPUCache[rootparam] = handles;
	}
	void CommandList::DynamicDescriptorAllocator::Flush(CommandList* commandList, bool is_graphics)
	{
		if (m_CPUCache.empty())
			return;

		// 처음 Flush이면 gpu heap 생성
		if (!m_UsingGPUHeap)
		{
			m_UsingGPUHeap = RequestGPUHeap();
		}
		// 처음이 아니면 현재 할당 가능한지 판별
		else
		{
			uint32_t curr_numDescriptor = 0;
			for (auto& cache : m_CPUCache)
			{
				curr_numDescriptor += (uint32_t)cache.second.size();
			}
			// 할당 불가능하면 다른 gpuheap을 생성
			if (!m_UsingGPUHeap->HasSpace(curr_numDescriptor))
			{
				auto gpu_heap = RequestGPUHeap();
				m_PendingGPUHeaps.push(m_GPUHeapPool[m_UsingGPUHeap]);
				m_GPUHeapPool.erase(m_UsingGPUHeap);
				m_UsingGPUHeap = gpu_heap;
			}
		}
		commandList->SetDescriptorHeap(m_UsingGPUHeap->D3D_Heap.Get());
		// 이제 rootparam별로 각 cpu를 복사해서 등록
		for (auto& handles : m_CPUCache)
		{

			auto temp_offset = m_UsingGPUHeap->CopyDescriptor(handles.second);
			auto gpu_address = m_UsingGPUHeap->GPUHandleOffset(temp_offset);

			(is_graphics) ?
				commandList->GetD3DCommandList()->SetGraphicsRootDescriptorTable(
					handles.first, gpu_address) :
				commandList->GetD3DCommandList()->SetComputeRootDescriptorTable(
					handles.first, gpu_address);
		}


		m_CPUCache.clear();
	}
	void CommandList::DynamicDescriptorAllocator::Reset()
	{
		for (auto& heap : m_GPUHeapPool)
		{
			m_PendingGPUHeaps.push(move(heap.second));
		}
		m_GPUHeapPool.clear();
		m_CPUCache.clear();
		m_UsingGPUHeap = nullptr;
	}
	CommandList::DynamicDescriptorAllocator::GPUHeap*
		CommandList::DynamicDescriptorAllocator::RequestGPUHeap()
	{
		if (m_PendingGPUHeaps.empty())
		{
			auto gpu_heap = make_shared<GPUHeap>(m_D3DDevice, m_NumDescriptorPerHeap);
			auto result = gpu_heap.get();
			m_GPUHeapPool[result] = std::move(gpu_heap);

			return result;
		}
		else
		{
			auto gpu_heap = m_PendingGPUHeaps.front(); m_PendingGPUHeaps.pop();
			auto result = gpu_heap.get();
			result->Reset();
			m_GPUHeapPool[result] = gpu_heap;
			return result;
		}
	}



}