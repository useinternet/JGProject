#pragma once
#include "DxObject.h"
class Color;

namespace RE
{
	class Resource;
	class ResourceDataMap;
	class UploadBuffer;
	class Buffer;
	class Texture;
	class VertexBuffer;
	class IndexBuffer;
	class RenderTarget;
	class RootSignature;
	class PipelineState;
	class CommandList : DxObject
	{
		class DynamicDescriptorAllocator;
	public:
		CommandList(D3D12_COMMAND_LIST_TYPE type);

	public: // ResourceBarrier Function && CommandList Function
		void TransitionBarrier(
			const Resource& resource, 
			D3D12_RESOURCE_STATES after_state, 
			uint32_t subresource = ResourceBarrier_All_Subresource,
			bool flush = false);


		void UavBarrier(
			const Resource& resource,
			bool flush = false);


		void AliasBarrier(
			const Resource& beforeResource, 
			const Resource& afterResource,
			bool flush = false);
		void FlushResourceBarrier();


		void DeleteTemporaryResource(const Resource& resource);

		void Reset();
		bool Close(CommandList* pendingCmdList);
		void Close();
	public: 	// Copy Function
	
		void CopyResource(const Resource& destResource, const Resource& sourceResource);
		void ResolveSubresource(Resource& dstRes, const Resource& srcRes, uint32_t dstSubresource = 0, uint32_t srcSubresource = 0);
		void CopyTextureSubresource(Texture& texture, uint32_t firstSubresource, uint32_t numSubresources, D3D12_SUBRESOURCE_DATA* subresourceData);


		template<typename VertexType>
		void CopyVertexBuffer(VertexBuffer& vertex_buffer, const std::vector<VertexType>& vertices) 
		{
			CopyBuffer(vertex_buffer, (uint32_t)vertices.size(), (uint32_t)sizeof(VertexType),
				vertices.data(), D3D12_RESOURCE_FLAG_NONE);
		}

		template<typename IndexType>
		void CopyIndexBuffer(IndexBuffer& index_buffer, const std::vector<IndexType>& indices)
		{
			CopyBuffer(index_buffer, (uint32_t)indices.size(), (uint32_t)sizeof(IndexType), indices.data(),
				D3D12_RESOURCE_FLAG_NONE);
		}
	private:

	public:
		void SetDescriptorHeap(ID3D12DescriptorHeap* d3d_heaps);
		// Rendering Function
		void SetGraphicsRootSignature(const RootSignature& rootsig);
		void SetComputeRootSignature(const RootSignature& rootsig);
		void SetPipelineState(const PipelineState& pso);
		void ClearRenderTarget(const Texture& texture, const Color& color);
		void ClearRenderTarget(const RenderTarget& rendertarget);
		void SetRenderTarget(const Texture& texture);
		void SetRenderTarget(const RenderTarget& rendertarget);
		void ClearDepthStencil(const Texture& texture, D3D12_CLEAR_FLAGS clearFlags, 
			float depth = 1.0f, uint8_t stencil = 0);



		void BindSRV(uint32_t rootparam, const std::vector<Texture>& textures);
		void BindUAV(uint32_t rootparam, const std::vector<Texture>& textures);


		void SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY primitiveTopology);

		template<typename DataType>
		void BindGraphicsDynamicConstantBuffer(uint32_t rootparam, const DataType& data)
		{
			BindGraphicsDynamicConstantBuffer(rootparam, sizeof(DataType), &data);
		}
		void BindGraphicsDynamicConstantBuffer(
			uint32_t rootparam, const std::vector<byte>& data);
		template<typename DataType>
		void BindGraphicsDynamicStructuredBuffer(uint32_t rootparam, const std::vector<DataType>& data_array)
		{
			BindGraphicsDynamicStructuredBuffer(rootparam, 
				(uint32_t)data_array.size(), (uint32_t)sizeof(DataType), data_array.data());
		}
		void BindGraphicsDynamicStructuredBuffer(uint32_t rootparam, uint32_t dataSize, const std::vector<byte>& data)
		{
			uint32_t numElements = (uint32_t)data.size() / dataSize;
			uint32_t elementSize = dataSize;
			BindGraphicsDynamicStructuredBuffer(rootparam, numElements, elementSize, data.data());
		}

		void BindVertexBuffer(uint32_t slot, const VertexBuffer& vertex_buffer);
		void BindIndexBuffer(const IndexBuffer& index_buffer);
		
		template<typename VertexType>
		void BindDynamicVertexBuffer(uint32_t slot, const std::vector<VertexType>& vertices)
		{
			BindDynamicVertexBuffer(slot, (uint32_t)vertices.size(), (uint32_t)sizeof(VertexType), vertices.data());
		}

		void BindDynamicVertexBuffer(uint32_t slot, uint32_t type_size, const std::vector<byte>& bt_vertices)
		{
			
			BindDynamicVertexBuffer(slot, (uint32_t)bt_vertices.size() / type_size, type_size, bt_vertices.data());
		}

		template<typename IndexType>
		void BindDynamicIndexBuffer(const std::vector<IndexType>& indicies)
		{
			auto index_size = sizeof(IndexType);
			assert(index_size == 2 || index_size == 4 && "index_size only supported 2 or 4 bit");

			DXGI_FORMAT format = (index_size == 2) ?
				DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

			BindDynamicIndexBuffer((uint32_t)indicies.size(), format, indicies.data());
		}

		void SetViewport(const Viewport& viewport);
		void SetViewports(const std::vector<Viewport>& viewports);
		void SetScissorRect(const ScissorRect& rect);
		void SetScissorRects(const std::vector<ScissorRect>& rects);



		void Draw(uint32_t vertex_count, uint32_t instance_count = 1, uint32_t start_vertex = 0, uint32_t start_instance = 0);
		void DrawIndexed(uint32_t index_count, uint32_t instance_count = 1, uint32_t start_index = 0, int32_t base_vertex = 0, uint32_t start_instance = 0);
		void Dispatch(uint32_t threadgroupX, uint32_t threadgroupY, uint32_t threadgroupZ);
	public://
		ID3D12GraphicsCommandList* GetD3DCommandList() const {
			return m_D3D_CommandList.Get();
		}
	private:
		void CopyBuffer(
			Buffer& buffer,
			uint32_t numElements, uint32_t elementSize, const void* data,
			D3D12_RESOURCE_FLAGS flags);


		void BindGraphicsDynamicConstantBuffer(
			uint32_t rootparam, uint32_t sizeInBytes, const void* bufferData);
		void BindGraphicsDynamicStructuredBuffer(
			uint32_t rootparam, uint32_t numElements, uint32_t elementSize, const void* bufferData);


		void BindDynamicVertexBuffer(
			uint32_t slot, uint32_t num_vertices, uint32_t vertex_size, const void* vertex_data);
		void BindDynamicIndexBuffer(
			uint32_t num_indicies, DXGI_FORMAT index_format, const void* index_data);

		

		void UploadResourceTemporaryStorage(Microsoft::WRL::ComPtr<ID3D12Resource> resource);
		void ResourceTemporaryStorage(const Resource& resource);
	private:
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_D3D_CommandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_D3D_Allocator;
		D3D12_COMMAND_LIST_TYPE m_Type;
	private:
		std::shared_ptr<ResourceDataMap> m_ResourceDataMap;
		std::shared_ptr<UploadBuffer>     m_UploadBuffer;
		std::shared_ptr<DynamicDescriptorAllocator> m_DynamicDescriptorAllocator;
		std::unordered_map<ID3D12Resource*, ReObject> m_TemporaryResourceObject;
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_TemporaryUploadResource;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_D3D_Graphics_RootSignature;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_D3D_Compute_RootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_D3D_PipelineState;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_D3D_DescriptorHeap;

		enum RootSignatureState
		{
			RootSignatureState_None,
			RootSignatureState_Graphics,
			RootSignatureState_Compute
		};
		RootSignatureState m_CurrRootSignatureState;
		//
	private:
		class DynamicDescriptorAllocator
		{
			using CPUCache = std::map<uint32_t, std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>>;
			struct GPUHeap
			{
				ID3D12Device* D3D_Device;
				Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> D3D_Heap;
				D3D12_CPU_DESCRIPTOR_HANDLE StartCpu;
				D3D12_GPU_DESCRIPTOR_HANDLE StartGpu;
				uint32_t CurrOffset;
				uint32_t NumDescriptor;
				GPUHeap(ID3D12Device* d3d_device, uint32_t numDescriptor);
				void Reset()
				{
					CurrOffset = 0;
				}
				bool HasSpace(uint32_t numDescriptor)
				{
					return (NumDescriptor - CurrOffset) >= numDescriptor;
				}
				D3D12_CPU_DESCRIPTOR_HANDLE CPUHandleOffset(uint32_t offset)
				{
					auto incrementSize = D3D_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
					return { StartCpu.ptr + (incrementSize * offset) };
				}
				D3D12_GPU_DESCRIPTOR_HANDLE GPUHandleOffset(uint32_t offset)
				{
					auto incrementSize = D3D_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
					return { StartGpu.ptr + (incrementSize * offset) };
				}
				uint32_t CopyDescriptor(const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& handles)
				{
					uint32_t result = CurrOffset;
					auto dest_start_cpu_handle = CPUHandleOffset(CurrOffset);
					auto dest_cpu_handle_size = (uint32_t)handles.size();
					std::vector<uint32_t> src_range(dest_cpu_handle_size, 1);
					
					D3D_Device->CopyDescriptors(
						1, &dest_start_cpu_handle, &dest_cpu_handle_size,
						(uint32_t)handles.size(), handles.data(), src_range.data(),
						D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
					CurrOffset += (uint32_t)handles.size();
					return result;
				}
			};
		public:
			DynamicDescriptorAllocator(ID3D12Device* device, uint32_t numDescriptorPerHeap = 2048);
			void PushCPUHandle(uint32_t rootparam, const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& handles);
			void Flush(CommandList* commandList, bool is_graphics);
			void Reset();
		private:
			GPUHeap* RequestGPUHeap();
		private:
			ID3D12Device* m_D3DDevice;
			uint32_t m_NumDescriptorPerHeap;
			std::queue<std::shared_ptr<GPUHeap>> m_PendingGPUHeaps;
			std::map<GPUHeap*, std::shared_ptr<GPUHeap>> m_GPUHeapPool;
			GPUHeap* m_UsingGPUHeap;
			CPUCache m_CPUCache;
			/*
			1. root param 과 텍스쳐 배열 을 받음
			2. root param을 키로 CPU_Handle배열을 저장
			3. CommandList에서 Draw나 Dispatch시에 이 클래스에 신호 
			4. root param에 해당하는 CPU_Handle들을 현재 Descriptor힙에 복사
			5. 만약 현재 할당된 메모리(1024)가 초과 될경우 새로운 DescriptorHeap을 생성 후 거기에 복사 후
			SetDescriptor 로 현재 DescriporHeap을 변경
			6. 바인딩
			7. 그려준다.
			*/
		};
	};
}