#include"pch.h"
#include"Commander.h"
#include"GraphicsDevice.h"
#include"UploadBuffer.h"
#include"Dx12Helper.h"
#include"GraphicsResource.h"
#include"DescriptorHeap.h"
#include"Viewport.h"
#include"ScissorRect.h"
#include"PipelineState.h"
#include"RootSignature.h"
#include"RootSignatureCache.h"
#include"GraphicsDevice.h"
using namespace std;
#define VALID_COMPUTE_QUEUE_RESOURCE_STATES \
    ( D3D12_RESOURCE_STATE_UNORDERED_ACCESS \
    | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE \
    | D3D12_RESOURCE_STATE_COPY_DEST \
    | D3D12_RESOURCE_STATE_COPY_SOURCE )
#define GPU_HEAP_SRV_COUNT 64
#define GPU_HEAP_UAV_COUNT 8
#define GPU_HEAP_CBV_COUNT 12
#define GPU_HEAP_ITEM_COUNT (GPU_HEAP_SRV_COUNT + GPU_HEAP_UAV_COUNT + GPU_HEAP_CBV_COUNT)

namespace GR
{
	namespace Dx12
	{

		std::wstring Commander::ms_GenerateMipMapsShaderPath = L"GenerateMipsCS.hlsl";
		Commander::GPUDescriptorAllocator::GPUDescriptorAllocator(ID3D12Device* device) : 
			m_D3D_DescirptorHeapType(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		{
			m_D3DDevice = device;
			m_GPUHeap = CreateD3DDescriptorHeap(device, m_D3D_DescirptorHeapType,
				D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 1024 * GPU_HEAP_ITEM_COUNT);
			m_StartCPUHeapHandle = m_GPUHeap->GetCPUDescriptorHandleForHeapStart();
			m_StartGPUHeapHandle = m_GPUHeap->GetGPUDescriptorHandleForHeapStart();
			m_IncrementDescritporSize = m_D3DDevice->GetDescriptorHandleIncrementSize(m_D3D_DescirptorHeapType);
			Reset();
		}
		void Commander::GPUDescriptorAllocator::StageDescriptorHandles(
			Commander::GPUDescriptorAllocator::EHeapType type, uint32_t rootIndex, uint32_t numHandles, D3D12_CPU_DESCRIPTOR_HANDLE* handle)
		{
			CPUDescriptorHandle cpuHandle;

			cpuHandle.rootIndex = rootIndex;
			cpuHandle.numHandles = numHandles;
			cpuHandle.handles.resize(numHandles);
			std::copy(handle, handle + numHandles, cpuHandle.handles.begin());
			
			m_CPUCache[type].push_back(cpuHandle);

		}
		void Commander::GPUDescriptorAllocator::StageDescriptorHandles(
			Commander::GPUDescriptorAllocator::EHeapType type, uint32_t rootIndex, uint32_t numHandles, D3D12_CPU_DESCRIPTOR_HANDLE handle)
		{
			CPUDescriptorHandle cpuHandle;

			cpuHandle.rootIndex  = rootIndex;
			cpuHandle.numHandles = numHandles;
			cpuHandle.handles.push_back(handle);

			m_CPUCache[type].push_back(cpuHandle);
		}
		void Commander::GPUDescriptorAllocator::BindDescriptorTable(Commander* commander, bool is_compute)
		{
			for (int type = SRV; type < NUMTYPE; ++type)
			{
				for (auto& cpuhandle : m_CPUCache[type])
				{
					D3D12_CPU_DESCRIPTOR_HANDLE CPU = m_StartCPUHeapHandle;
					CPU.ptr += (m_Offset[type] * m_IncrementDescritporSize);
					D3D12_GPU_DESCRIPTOR_HANDLE GPU = m_StartGPUHeapHandle;
					GPU.ptr += (m_Offset[type] * m_IncrementDescritporSize);

					// 불연속 Heap
					if (cpuhandle.numHandles == (uint32_t)cpuhandle.handles.size())
					{
						for (uint32_t i = 0; i < cpuhandle.numHandles; ++i)
						{
							if (cpuhandle.handles[i].ptr != 0)
							{
								m_D3DDevice->CopyDescriptorsSimple(1, CPU, cpuhandle.handles[i], m_D3D_DescirptorHeapType);
							}
							CPU.ptr += m_IncrementDescritporSize;
						}
					}
					// 연속적인 Heap
					else
					{
						m_D3DDevice->CopyDescriptorsSimple(cpuhandle.numHandles, CPU, cpuhandle.handles[0],
							m_D3D_DescirptorHeapType);
					}
					(is_compute) ? 
						commander->GetD3DCommandList()->SetComputeRootDescriptorTable(cpuhandle.rootIndex, GPU) : 
						commander->GetD3DCommandList()->SetGraphicsRootDescriptorTable(cpuhandle.rootIndex, GPU);
					m_Offset[type] += cpuhandle.numHandles;
				}
			}
			Flush();
		}
		void Commander::GPUDescriptorAllocator::BindDescriptorHeap(Commander* commander)
		{
			commander->GetD3DCommandList()->SetDescriptorHeaps(1, m_GPUHeap.GetAddressOf());
		}
		void Commander::GPUDescriptorAllocator::Reset()
		{
			Flush();
			m_Offset[SRV] = 0;
			m_Offset[UAV] = 1024 * GPU_HEAP_SRV_COUNT;
			m_Offset[CBV] = 1024 * (GPU_HEAP_SRV_COUNT + GPU_HEAP_UAV_COUNT);
		}
		void Commander::GPUDescriptorAllocator::Flush()
		{
			for (int i = 0; i < NUMTYPE; ++i)
			{
				m_CPUCache[i].clear();
			}
		}
		Commander::Commander(ComPtr<ID3D12Device> device, D3D12_COMMAND_LIST_TYPE type) :
			m_CmdListType(type), m_D3DDevice(device)
		{
			m_D3D_CommandAllocator = CreateD3DCommandAllocator(device, type);
			m_D3D_CommandList = CreateD3DCommandList(device, m_D3D_CommandAllocator, type);
			m_GPUAllocator = make_unique<GPUDescriptorAllocator>(m_D3DDevice.Get());
			m_UploadBuffer = make_unique<UploadBuffer>(device);
			
			m_GPUAllocator->BindDescriptorHeap(this);
			m_CurrPipeLineState = nullptr;
			m_CurrRootSignature = nullptr;
		}


		void Commander::TransitionBarrier(GPUResource& resource, D3D12_RESOURCE_STATES newState, bool is_flush)
		{
			D3D12_RESOURCE_STATES oldState = resource.m_UsageState;

			if (m_CmdListType == D3D12_COMMAND_LIST_TYPE_COMPUTE)
			{
				assert((oldState & VALID_COMPUTE_QUEUE_RESOURCE_STATES) == oldState && "ComputeCommander is not support this resource_state");
				assert((newState & VALID_COMPUTE_QUEUE_RESOURCE_STATES) == newState && "ComputeCommander is not support this resource_state");
			}

			if (oldState != newState)
			{
				assert(m_ResourceBarriers.size() < 16 && "Exceeded arbitrary limit on buffered barriers");
				D3D12_RESOURCE_BARRIER barrierDesc = {};
				barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				barrierDesc.Transition.pResource = resource.GetResource();
				barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				barrierDesc.Transition.StateBefore = oldState;
				barrierDesc.Transition.StateAfter = newState;

				if (newState == resource.m_TransitioningState)
				{
					barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_END_ONLY;
					resource.m_TransitioningState = (D3D12_RESOURCE_STATES)-1;
				}
				else
				{
					barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				}
				m_ResourceBarriers.push_back(barrierDesc);
				resource.m_UsageState = newState;
			}
			else if(newState == D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
			{
				UAVBarrier(resource);
			}
			if (is_flush || m_ResourceBarriers.size() == 16)
			{
				FlushResourceBarriers();
			}

			BackUpObject(resource);
		}
		void Commander::BeginTransitionBarrier(GPUResource& resource, D3D12_RESOURCE_STATES newState, bool is_flush)
		{
			if (resource.m_TransitioningState != (D3D12_RESOURCE_STATES)-1)
				TransitionBarrier(resource, resource.m_TransitioningState);

			D3D12_RESOURCE_STATES oldState = resource.m_UsageState;

			if (oldState != newState)
			{
				assert(m_ResourceBarriers.size() < 16 && "Exceeded arbitrary limit on buffered barriers");
				D3D12_RESOURCE_BARRIER barrierDesc = {};

				barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				barrierDesc.Transition.pResource = resource.GetResource();
				barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				barrierDesc.Transition.StateBefore = oldState;
				barrierDesc.Transition.StateAfter = newState;

				barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_BEGIN_ONLY;

				m_ResourceBarriers.push_back(barrierDesc);
				resource.m_TransitioningState = newState;
			}

			if (is_flush || m_ResourceBarriers.size() == 16)
			{
				FlushResourceBarriers();
			}
			BackUpObject(resource);
		}
		void Commander::UAVBarrier(GPUResource& Resource, bool is_flush)
		{
			assert(m_ResourceBarriers.size() < 16 && "Exceeded arbitrary limit on buffered barriers");

			D3D12_RESOURCE_BARRIER barrierDesc = {};

			barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
			barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrierDesc.UAV.pResource = Resource.GetResource();

			if (is_flush)
				FlushResourceBarriers();

			BackUpObject(Resource);
		}
		void Commander::AliasBarrier(GPUResource& Before, GPUResource& After, bool is_flush)
		{
			assert(m_ResourceBarriers.size() < 16 && "Exceeded arbitrary limit on buffered barriers");
			D3D12_RESOURCE_BARRIER barrierDesc = {};


			barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_ALIASING;
			barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrierDesc.Aliasing.pResourceBefore = Before.GetResource();
			barrierDesc.Aliasing.pResourceAfter = After.GetResource();

			if (is_flush)
				FlushResourceBarriers();

			BackUpObject(Before); 		BackUpObject(After);
		}
		void Commander::FlushResourceBarriers()
		{
			if (!m_ResourceBarriers.empty())
			{
				m_D3D_CommandList->ResourceBarrier((uint32_t)m_ResourceBarriers.size(), m_ResourceBarriers.data());
				m_ResourceBarriers.clear();
			}
		}
		void Commander::CopyResource(GPUResource& dest, GPUResource& src)
		{
			TransitionBarrier(dest, D3D12_RESOURCE_STATE_COPY_DEST);
			TransitionBarrier(src, D3D12_RESOURCE_STATE_COPY_SOURCE);

			FlushResourceBarriers();


			m_D3D_CommandList->CopyResource(dest.GetResource(), src.GetResource());



			BackUpObject(dest);
			BackUpObject(src);
		}
		void Commander::CopyResourceRegion(
			GPUResource& dest_resource, uint32_t dstMip, uint32_t dstX, uint32_t dstY, uint32_t dstZ,
			GPUResource& src_resource, uint32_t srcMip, uint32_t arraySlice)
		{
			TransitionBarrier(dest_resource, D3D12_RESOURCE_STATE_COPY_DEST);
			TransitionBarrier(src_resource, D3D12_RESOURCE_STATE_COPY_SOURCE);

			FlushResourceBarriers();


			D3D12_RESOURCE_DESC dst_desc = dest_resource->GetDesc();
			D3D12_RESOURCE_DESC src_desc = src_resource->GetDesc();


			D3D12_TEXTURE_COPY_LOCATION dst = {};
			dst.pResource = dest_resource.GetResource();
			dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			dst.SubresourceIndex = D3D12CalcSubresource(dstMip, arraySlice, 0, dst_desc.MipLevels, dst_desc.DepthOrArraySize);

			D3D12_TEXTURE_COPY_LOCATION src = {};
			src.pResource = src_resource.GetResource();
			src.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			src.SubresourceIndex = D3D12CalcSubresource(srcMip, arraySlice, 0, src_desc.MipLevels, src_desc.DepthOrArraySize);


			m_D3D_CommandList->CopyTextureRegion(&dst, dstX, dstY, dstZ, &src, nullptr);


			BackUpObject(dest_resource);
			BackUpObject(src_resource);

		}
		void Commander::CopyBuffer(
			GPUBuffer& buffer,
			uint32_t numElements, uint32_t elementSize,
			const void* bufferData,
			D3D12_RESOURCE_FLAGS flags)
		{
			auto gp_device = GraphicsDevice::GetPointer();
			auto d3d_device = m_D3DDevice.Get();


			uint32_t bufferSize = numElements * elementSize;


			if (bufferSize == 0)
				return;

			GPUResource resource = gp_device->CreateGPUResource(CD3DX12_RESOURCE_DESC::Buffer(bufferSize, flags));

			if (bufferData != nullptr)
			{

				GPUResource upload = gp_device->CreateGPUResource(
					CD3DX12_RESOURCE_DESC::Buffer(bufferSize), nullptr,
					GraphicsDevice::UPLOAD,
					D3D12_RESOURCE_STATE_GENERIC_READ);


				D3D12_SUBRESOURCE_DATA subResourceData = {};
				subResourceData.pData = bufferData;
				subResourceData.RowPitch = bufferSize;
				subResourceData.SlicePitch = subResourceData.RowPitch;

				TransitionBarrier(resource, D3D12_RESOURCE_STATE_COPY_DEST, true);

				UpdateSubresources(m_D3D_CommandList.Get(), resource.GetResource(),
					upload.GetResource(), 0, 0, 1, &subResourceData);


				BackUpObject(upload);
			}
			BackUpObject(resource);

			buffer.m_Resource = resource.m_Resource;
			buffer.m_TransitioningState = resource.m_TransitioningState;
			buffer.m_UsageState = resource.m_UsageState;
			buffer.CreateView(numElements, elementSize);
		}
		void Commander::BackUpObject(const GPUResource& resource)
		{
			m_BackedUpObject.push_back(resource.m_Resource);
		}
		void Commander::BackUpObject(ComPtr<ID3D12Object> obj)
		{
			m_BackedUpObject.push_back(obj);
		}
		void Commander::FlushBackUpObject()
		{
			m_BackedUpObject.clear();
		}
		void Commander::Close()
		{
			FlushResourceBarriers();


			m_D3D_CommandList->Close();
		}
		void Commander::Reset()
		{
		
			m_D3D_CommandAllocator->Reset();
			m_D3D_CommandList->Reset(m_D3D_CommandAllocator.Get(), nullptr);


			m_CurrPipeLineState = nullptr;
			m_CurrRootSignature = nullptr;

			m_GPUAllocator->Reset();
			m_GPUAllocator->BindDescriptorHeap(this);
			m_UploadBuffer->Reset();
			FlushBackUpObject();
		}
		void GraphicsCommander::ClearColor(ColorTexture& texture)
		{
			TransitionBarrier(texture, D3D12_RESOURCE_STATE_RENDER_TARGET, true);

			auto color = texture.GetClearColor();
			float clearValue[4];
			clearValue[0] = color.x;
			clearValue[1] = color.y;
			clearValue[2] = color.z;
			clearValue[3] = color.w;
			m_D3D_CommandList->ClearRenderTargetView(texture.GetRTV(), clearValue, 0, nullptr);
			BackUpObject(texture);
			
		}
		void GraphicsCommander::ClearDepth(DepthTexture& texture)
		{
			TransitionBarrier(texture, D3D12_RESOURCE_STATE_DEPTH_WRITE, true);

			m_D3D_CommandList->ClearDepthStencilView(
				texture.GetDSV(), D3D12_CLEAR_FLAG_DEPTH, texture.GetClearDepth(), texture.GetClearStencil(), 0, nullptr);
			BackUpObject(texture);
		}
		void GraphicsCommander::ClearStencil(DepthTexture& texture)
		{
			TransitionBarrier(texture, D3D12_RESOURCE_STATE_DEPTH_WRITE, true);

			m_D3D_CommandList->ClearDepthStencilView(
				texture.GetDSV(), D3D12_CLEAR_FLAG_STENCIL, texture.GetClearDepth(), texture.GetClearStencil(), 0, nullptr);
			BackUpObject(texture);
		}
		void GraphicsCommander::ClearDepthStencil(DepthTexture& texture)
		{
			TransitionBarrier(texture, D3D12_RESOURCE_STATE_DEPTH_WRITE, true);

			m_D3D_CommandList->ClearDepthStencilView(
				texture.GetDSV(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, texture.GetClearDepth(), texture.GetClearStencil(), 0, nullptr);
			BackUpObject(texture);
		}

		void GraphicsCommander::SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE RTV, D3D12_CPU_DESCRIPTOR_HANDLE DSV)
		{
			m_D3D_CommandList->OMSetRenderTargets(1, &RTV, false, &DSV);
		}
		void GraphicsCommander::SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE RTV)
		{
			m_D3D_CommandList->OMSetRenderTargets(1, &RTV, false, nullptr);
		}
		void GraphicsCommander::SetRenderTargets(uint32_t numRtv, const D3D12_CPU_DESCRIPTOR_HANDLE RTV[], D3D12_CPU_DESCRIPTOR_HANDLE DSV)
		{
			m_D3D_CommandList->OMSetRenderTargets(numRtv, RTV, false, &DSV);
			
		}
		void GraphicsCommander::SetRenderTargets(uint32_t numRtv, const D3D12_CPU_DESCRIPTOR_HANDLE RTV[])
		{
			m_D3D_CommandList->OMSetRenderTargets(numRtv, RTV, false, nullptr);
		}

		void GraphicsCommander::SetViewport(const Viewport& viewport)
		{
			m_D3D_CommandList->RSSetViewports(1, &viewport.Get());
		}
		void GraphicsCommander::SetViewports(uint32_t numViewport, Viewport* viewport)
		{
			std::vector<D3D12_VIEWPORT> v(numViewport);
			for (uint32_t i = 0; i < numViewport; ++i)
			{
				v[i] = viewport[i].Get();
			}
			m_D3D_CommandList->RSSetViewports(numViewport, v.data());
		}

		void GraphicsCommander::SetScissorRect(const ScissorRect& rect)
		{
			m_D3D_CommandList->RSSetScissorRects(1, &rect.Get());

		}
		void GraphicsCommander::SetScissorRects(uint32_t numScissorRect, ScissorRect* rect)
		{
			std::vector<D3D12_RECT> v(numScissorRect);
			for (uint32_t i = 0; i < numScissorRect; ++i)
			{
				v[i] = rect[i].Get();
			}
			m_D3D_CommandList->RSSetScissorRects(numScissorRect, v.data());
		}

		void GraphicsCommander::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology)
		{
			m_D3D_CommandList->IASetPrimitiveTopology(topology);
		}


		void GraphicsCommander::SetConstants(uint32_t rootparam, uint32_t num32BitValues, const void* srcData)
		{
			m_D3D_CommandList->SetGraphicsRoot32BitConstants(rootparam, num32BitValues, srcData, 0);
		}
		void GraphicsCommander::SetConstantBuffer(uint32_t rootparam, GPUResource& resource)
		{
			m_D3D_CommandList->SetGraphicsRootConstantBufferView(rootparam, resource.GetResource()->GetGPUVirtualAddress());
			BackUpObject(resource);
		}
		void GraphicsCommander::SetDynamicConstantBuffer(uint32_t rootparam, uint32_t sizeInByte, const void* data)
		{
			auto alloc = m_UploadBuffer->Allocate(sizeInByte, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
			memcpy(alloc.CPU, data, sizeInByte);

			m_D3D_CommandList->SetGraphicsRootConstantBufferView(rootparam, alloc.GPU);
		}
		void GraphicsCommander::SetDynamicStructuredBuffer(uint32_t rootparam, uint32_t numElement, uint32_t elementSize, const void* data)
		{
			uint32_t bufferSize = numElement * elementSize;
			auto alloc = m_UploadBuffer->Allocate(bufferSize, elementSize);
			memcpy(alloc.CPU, data, bufferSize);
			m_D3D_CommandList->SetGraphicsRootShaderResourceView(rootparam, alloc.GPU);
		}
		void GraphicsCommander::SetShaderResourceView(uint32_t rootparam, GPUResource& resource)
		{
			m_D3D_CommandList->SetGraphicsRootShaderResourceView(rootparam, resource->GetGPUVirtualAddress());
			BackUpObject(resource);
		}
		void GraphicsCommander::SetVertexBuffer(VertexBuffer& buffer)
		{
			TransitionBarrier(buffer, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, true);
			m_D3D_CommandList->IASetVertexBuffers(0, 1, &buffer.GetView());
		}
		void GraphicsCommander::SetDynamicVertexBuffer(uint32_t numVertices, uint32_t vertexStride, const void* data)
		{
			uint32_t bufferSize = numVertices * vertexStride;
			auto alloc = m_UploadBuffer->Allocate(bufferSize, vertexStride);

			memcpy(alloc.CPU, data, bufferSize);

			D3D12_VERTEX_BUFFER_VIEW view;
			view.BufferLocation = alloc.GPU;
			view.SizeInBytes = bufferSize;
			view.StrideInBytes = vertexStride;

			m_D3D_CommandList->IASetVertexBuffers(0, 1, &view);
		}
		void GraphicsCommander::SetIndexBuffer(IndexBuffer& buffer)
		{
			TransitionBarrier(buffer, D3D12_RESOURCE_STATE_INDEX_BUFFER, true);
			m_D3D_CommandList->IASetIndexBuffer(&buffer.GetView());
		}
		void GraphicsCommander::SetDynamicIndexBuffer(uint32_t indexCount, uint32_t elementSize, const void* data)
		{
			assert(elementSize == 2 || elementSize == 4 && "Index's format must be 16 or 32 bit integer");

			DXGI_FORMAT format = (elementSize == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
			uint32_t bufferSize = indexCount * elementSize;

			auto alloc = m_UploadBuffer->Allocate(bufferSize, elementSize);

			memcpy(alloc.CPU, data, bufferSize);


			D3D12_INDEX_BUFFER_VIEW view;
			view.BufferLocation = alloc.GPU;
			view.SizeInBytes = bufferSize;
			view.Format = format;

			m_D3D_CommandList->IASetIndexBuffer(&view);
		}

		void GraphicsCommander::SetPipelineState(GraphicsPSO& pso)
		{
			auto pPso = pso.GetD3DPipelineState();
			if (m_CurrPipeLineState != pPso)
			{
				m_D3D_CommandList->SetPipelineState(pPso);
				m_CurrPipeLineState = pPso;
			}
		}
		void GraphicsCommander::SetRootSignature(RootSignature& rootsig)
		{
			auto pRootsig = rootsig.GetD3DRootSignature();
			if (m_CurrRootSignature != pRootsig)
			{
				m_D3D_CommandList->SetGraphicsRootSignature(pRootsig);
				m_CurrRootSignature = pRootsig;
			}

		}
		void GraphicsCommander::SetSRVDescriptorTable(uint32_t rootparam, uint32_t numHandles, D3D12_CPU_DESCRIPTOR_HANDLE* handle)
		{
			m_GPUAllocator->StageDescriptorHandles(GPUDescriptorAllocator::SRV,
				rootparam, numHandles, handle);
		}
		void GraphicsCommander::SetUAVDescriptorTable(uint32_t rootparam, uint32_t numHandles, D3D12_CPU_DESCRIPTOR_HANDLE* handle)
		{
			m_GPUAllocator->StageDescriptorHandles(GPUDescriptorAllocator::UAV,
				rootparam, numHandles, handle);
		}
		void GraphicsCommander::SetCBVDescriptorTable(uint32_t rootparam, uint32_t numHandles, D3D12_CPU_DESCRIPTOR_HANDLE* handle)
		{
			m_GPUAllocator->StageDescriptorHandles(GPUDescriptorAllocator::CBV,
				rootparam, numHandles, handle);
		}
		void GraphicsCommander::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertexLocation, uint32_t startInstanceLocation)
		{
			m_GPUAllocator->BindDescriptorTable(this, false);
			m_D3D_CommandList->DrawInstanced(vertexCount, instanceCount, startVertexLocation, startInstanceLocation);
		}
		void GraphicsCommander::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndexLocation, uint32_t baseVertexLocation, uint32_t startInstanceLocation)
		{
			m_GPUAllocator->BindDescriptorTable(this, false);
			m_D3D_CommandList->DrawIndexedInstanced(indexCount, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
		}
		ComputeCommander*  GraphicsCommander::ConvertComputeCommander()
		{
			Commander* c = this;
			return static_cast<ComputeCommander*>(c);

		}



		struct alignas(16) GenerateMipsCB
		{
			uint32_t SrcMipLevel;	// Texture level of source mip
			uint32_t NumMipLevels;	// Number of OutMips to write: [1-4]
			uint32_t SrcDimension;  // Width and height of the source texture are even or odd.
			uint32_t Padding;       // Pad to 16 byte alignment.
			DirectX::XMFLOAT2 TexelSize;	// 1.0 / OutMip1.Dimensions
		};


		void ComputeCommander::SetConstants(uint32_t rootparam, uint32_t num32BitValues, const void* srcData)
		{
			m_D3D_CommandList->SetComputeRoot32BitConstants(rootparam, num32BitValues, srcData, 0);
		}
		void ComputeCommander::SetConstantBuffer(uint32_t rootparam, GPUResource& resource)
		{
			m_D3D_CommandList->SetComputeRootConstantBufferView(rootparam, resource->GetGPUVirtualAddress());
			BackUpObject(resource);
		}
		void ComputeCommander::SetDynamicConstantBuffer(uint32_t rootparam, uint32_t sizeInByte, const void* data)
		{
			auto alloc = m_UploadBuffer->Allocate(sizeInByte, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
			memcpy(alloc.CPU, data, sizeInByte);
			m_D3D_CommandList->SetComputeRootConstantBufferView(rootparam, alloc.GPU);
		}


		void ComputeCommander::SetDynamicStructuredBuffer(uint32_t rootparam, uint32_t numElement, uint32_t elementSize, const void* data)
		{
			uint32_t bufferSize = numElement * elementSize;
			auto alloc = m_UploadBuffer->Allocate(bufferSize, elementSize);
			memcpy(alloc.CPU, data, bufferSize);

			m_D3D_CommandList->SetComputeRootShaderResourceView(rootparam, alloc.GPU);
		}
		void ComputeCommander::SetShaderResourceView(uint32_t rootparam, GPUResource& resource)
		{
			m_D3D_CommandList->SetComputeRootShaderResourceView(rootparam, resource->GetGPUVirtualAddress());
			BackUpObject(resource);
		}
		void ComputeCommander::SetPipelineState(ComputePSO& pso)
		{
			auto pPso = pso.GetD3DPipelineState();
			if (pPso != m_CurrPipeLineState)
			{
				m_D3D_CommandList->SetPipelineState(pPso);
				m_CurrPipeLineState = pPso;
			}
		}
		void ComputeCommander::SetRootSignature(RootSignature& rootsig)
		{
			auto pRootsig = rootsig.GetD3DRootSignature();
			if (m_CurrRootSignature != pRootsig)
			{
				m_D3D_CommandList->SetComputeRootSignature(pRootsig);
				m_CurrRootSignature = pRootsig;
			}
		}
		void ComputeCommander::SetSRVDescriptorTable(uint32_t rootparam, uint32_t numHandles, D3D12_CPU_DESCRIPTOR_HANDLE* handle)
		{
			m_GPUAllocator->StageDescriptorHandles(GPUDescriptorAllocator::SRV, rootparam, numHandles, handle);
		}
		void ComputeCommander::SetUAVDescriptorTable(uint32_t rootparam, uint32_t numHandles, D3D12_CPU_DESCRIPTOR_HANDLE* handle)
		{
			m_GPUAllocator->StageDescriptorHandles(GPUDescriptorAllocator::UAV, rootparam, numHandles, handle);
		}
		void ComputeCommander::SetCBVDescriptorTable(uint32_t rootparam, uint32_t numHandles, D3D12_CPU_DESCRIPTOR_HANDLE* handle)
		{
			m_GPUAllocator->StageDescriptorHandles(GPUDescriptorAllocator::CBV, rootparam, numHandles, handle);
		}
		void ComputeCommander::Dispatch(uint64_t groupX, uint64_t groupY, uint64_t groupZ)
		{
			m_GPUAllocator->BindDescriptorTable(this, true);
			m_D3D_CommandList->Dispatch(groupX, groupY, groupZ);

		}
		void ComputeCommander::GenerateMipMaps(Texture& texture)
		{
			auto resourceDesc = texture->GetDesc();


			if (resourceDesc.MipLevels == 1)
				return;

			GenerateMipMaps_UAV(texture);
	
		
		}
		void ComputeCommander::GenerateMipMaps_UAV(Texture& texture)
		{
			auto device = GraphicsDevice::GetPointer();
			auto resourceDesc = texture->GetDesc();
			auto UAVHandle = device->Allocate(GraphicsDevice::SRV_UAV_CBV, 4);
			auto increaseSize = device->GetDescriptorIncrementSize(GraphicsDevice::SRV_UAV_CBV);
			for (uint32_t i = 0; i < 4; ++i)
			{
				D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
				uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
				uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				uavDesc.Texture2D.MipSlice = i;
				uavDesc.Texture2D.PlaneSlice = 0;

				device->GetD3DDevice()->CreateUnorderedAccessView(nullptr, nullptr, &uavDesc,
					UAVHandle.GetDescriptorHandle(i));

			}

			Texture stagingTexture;

			if (!(resourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS))
			{
				auto statgingDesc = resourceDesc;
				statgingDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

				GPUResource stagingResource = device->CreateGPUResource(
					statgingDesc, nullptr, GraphicsDevice::DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST);
				stagingTexture = device->CreateTexture(stagingResource);

				CopyResource(stagingTexture, texture);
			}
			else
			{
				stagingTexture = texture;
			}
			auto rootsig = device->GetRootSignatureFromCache(ERootSignature::C_GenerateMipMaps);
			ComputePSO pso;
			if (!m_GenerateMipMapsPSO)
			{
				m_GenerateMipMapsPSO = make_shared<ComputePSO>();
				ComputeShader CS;
				auto dir = device->GetComputeShaderDirPath();
				device->CreateShader(dir + ms_GenerateMipMapsShaderPath, &CS);
				m_GenerateMipMapsPSO->BindComputeShader(CS);
				m_GenerateMipMapsPSO->BindRootSignature(rootsig.GetD3DRootSignature());
				m_GenerateMipMapsPSO->Finalize();
			}
			pso = *m_GenerateMipMapsPSO;
			//auto pso = device->GetComputePSOFromCache(rootsig, PSOCache::GENERATEMIPMAPS, PSOCache::USE_NULL);
			SetRootSignature(rootsig);
			SetPipelineState(pso);

			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = resourceDesc.Format;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.ViewDimension = resourceDesc.DepthOrArraySize > 1 ? D3D12_SRV_DIMENSION_TEXTURE2DARRAY : D3D12_SRV_DIMENSION_TEXTURE2D;
			if (resourceDesc.DepthOrArraySize > 1)
			{
				srvDesc.Texture2DArray.ArraySize = 1;
				srvDesc.Texture2DArray.FirstArraySlice = 0;
				srvDesc.Texture2DArray.MipLevels = resourceDesc.MipLevels;
				srvDesc.Texture2DArray.MostDetailedMip = 0;
				srvDesc.Texture2DArray.PlaneSlice = 0;
				srvDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
			}
			else
			{
				srvDesc.Texture2D.MipLevels = resourceDesc.MipLevels;
				srvDesc.Texture2D.MostDetailedMip = 0;
				srvDesc.Texture2D.PlaneSlice = 0;
				srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
			}

			for (uint32_t arraySlice = 0; arraySlice < resourceDesc.DepthOrArraySize; ++arraySlice)
			{
				if (arraySlice > 0)
					srvDesc.Texture2DArray.FirstArraySlice = arraySlice;


				GenerateMipsCB mipCB;
				for (uint32_t srcMip = 0; srcMip < resourceDesc.MipLevels - 1u;)
				{
					uint64_t srcWidth = resourceDesc.Width >> srcMip;
					uint32_t srcHeight = resourceDesc.Height >> srcMip;
					uint32_t dstWidth = static_cast<uint32_t>(srcWidth >> 1);
					uint32_t dstHeight = srcHeight >> 1;

					mipCB.SrcDimension = (srcHeight & 1) << 1 | (srcWidth & 1);

					DWORD mipCount;

					_BitScanForward(&mipCount, (dstWidth == 1 ? dstHeight : dstWidth) |
						(dstHeight == 1 ? dstWidth : dstHeight));

					mipCount = std::min<DWORD>(4, mipCount + 1);
					mipCount = (srcMip + mipCount) > resourceDesc.MipLevels ?
						resourceDesc.MipLevels - srcMip : mipCount;

					dstWidth = std::max<DWORD>(1, dstWidth);
					dstHeight = std::max<DWORD>(1, dstHeight);

					mipCB.SrcMipLevel = srcMip;
					mipCB.NumMipLevels = mipCount;
					mipCB.TexelSize.x = 1.0f / (float)dstWidth;
					mipCB.TexelSize.y = 1.0f / (float)dstHeight;



					SetDynamicConstantBuffer(C_GenerateMipMaps_RootParam_GenerateMipsCB, sizeof(GenerateMipsCB), &mipCB);
					SetSRVDescriptorTable(C_GenerateMipMaps_RootParam_InputTexture, 1, &stagingTexture.GetSRV(&srvDesc));
					std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> outputHandles(4);
					for (uint32_t mip = 0; mip < mipCount; ++mip)
					{
						D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
						if (resourceDesc.DepthOrArraySize > 1)
						{
							uavDesc.Format = resourceDesc.Format;
							uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
							uavDesc.Texture2DArray.ArraySize = 1;
							uavDesc.Texture2DArray.MipSlice = srcMip + mip + 1;
							uavDesc.Texture2DArray.PlaneSlice = 0;
							uavDesc.Texture2DArray.FirstArraySlice = srvDesc.Texture2DArray.FirstArraySlice;
						}
						else
						{
							uavDesc.Format = resourceDesc.Format;
							uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
							uavDesc.Texture2D.MipSlice = srcMip + mip + 1;
						}
						outputHandles[mip] = stagingTexture.GetUAV(&uavDesc);
					}

					for (uint32_t mip = mipCount; mip < 4; ++mip)
					{
						outputHandles[mip] = UAVHandle.GetDescriptorHandle(mip);
					}

					SetUAVDescriptorTable(C_GenerateMipMaps_RootParam_OutputTexture, 4, outputHandles.data());

					Dispatch(Math::DivideByMultiple(dstWidth, 8), Math::DivideByMultiple(dstHeight, 8), 1);
					UAVBarrier(stagingTexture);
					srcMip += mipCount;
				}


			}
			
			if (stagingTexture.GetResource() != texture.GetResource())
			{
				CopyResource(texture, stagingTexture);
			}
		}
	}
}