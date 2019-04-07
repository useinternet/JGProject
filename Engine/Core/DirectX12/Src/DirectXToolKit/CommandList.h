#pragma once


#include"DirectXCommon/DirectXCommon.h"
#include"Resource.h"
namespace Dx12
{
	class RootSignature;
	class UploadBuffer;
	class GraphicsPSO;
	class ComputePSO;
	class PSO;
	class VertexBuffer;
	class IndexBuffer;
	class Buffer;
	class Viewport;
	class ScissorRect;
	class Texture;
	class DescriptorHeap;
	class RenderTarget;
	class GenerateMipMapsCS;
	class CommandList
	{
	private:
		static std::unordered_map<std::string, ComPtr<ID3D12Resource>> ms_GlobalTextures;
		static std::mutex ms_GlobalTexturesMutex;
	private:
		D3D12_COMMAND_LIST_TYPE                m_CmdListType;
		ComPtr<ID3D12GraphicsCommandList>      m_D3D_CommandList;
		ComPtr<ID3D12CommandAllocator>         m_D3D_CommandAllocator;
		std::shared_ptr<UploadBuffer>          m_UploadBuffer;
		std::vector<ComPtr<ID3D12Object>>      m_TrackObjects;

		std::shared_ptr<DescriptorHeap>        m_DescriptorHeap;
		uint32_t m_NumDescriptor    = 1000000;
		uint32_t m_DescriptorOffset = 0;


		std::shared_ptr<GenerateMipMapsCS> m_GenerateMipMapsCS;
	public:
		CommandList(D3D12_COMMAND_LIST_TYPE type);

		ComPtr<ID3D12GraphicsCommandList> GetComPtr() const {
			return m_D3D_CommandList;
		}
		ID3D12GraphicsCommandList* Get() const {
			return m_D3D_CommandList.Get();
		}
	public:
		void CopyBuffer(
			Buffer& buffer,
			uint32_t numElements,
			uint32_t elementSize,
			const void* bufferData, 
			D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);

		template<typename DataType>
		void CopyBuffer(
			Buffer& buffer,
			const std::vector<DataType>& datas,
			D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE) {

			CopyBuffer(buffer, (uint32_t)datas.size(), (uint32_t)sizeof(DataType),
				datas.data(), flags);
		}



		void CopyResource(const Resource& destResource, const Resource& srcResource);
		void ClearRenderTarget(const RenderTarget& rendertarget);

		void TransitionBarrier(const Resource& resource, D3D12_RESOURCE_STATES afterstate);
		void UAVBarrier(const Resource& resource);
		void AliasingBarrier(const Resource& beforeResource, const Resource& afterResource);

		void GenerateMipMaps(const Texture& texture);
	public:
		void SetGraphicsConstants(uint32_t rootparam, uint32_t num32BitValues,const void* srcData);
		void SetGraphicsConstantBufferView(uint32_t rootparam, uint32_t sizeInbyte,const void* data);
		void SetGraphicsConstantBufferView(uint32_t rootparam, const Resource& resource);
		void SetGraphicsDescriptorTable(uint32_t rootparam, D3D12_CPU_DESCRIPTOR_HANDLE handle);
		void SetGraphicsDescriptorTables(uint32_t rootparam, uint32_t numHandles, const D3D12_CPU_DESCRIPTOR_HANDLE* handle);
		void SetGraphicsShaderResourceView(uint32_t rootparam, uint32_t numElement, uint32_t elementSize,const void* data);
		void SetGraphicsShaderResourceView(uint32_t rootparam, const Resource& resource);
		void SetGraphicsUnorderedAccessView(uint32_t rootparam, const Resource& resource);

		template<typename T>
		void SetGraphicsConstants(uint32_t rootparam, const T& data) {
			SetGraphicsConstants(rootparam, (uint32_t)(sizeof(T) / 4), &data);
		}
		template<typename T>
		void SetGraphicsConstantBufferView(uint32_t rootparam, T* data) {
			SetGraphicsConstantBufferView(rootparam, sizeof(T), data);
		}
		template<typename T>
		void SetGraphicsShaderResourceView(uint32_t rootparam, const std::vector<T>& data)
		{
			SetGraphicsShaderResourceView(rootparam, (uint32_t)data.size(), (uint32_t)sizeof(T), data.data());
		}

		void SetComputeConstants(uint32_t rootparam, uint32_t num32BitValues, const void* srcData);
		void SetComputeConstantBufferView(uint32_t rootparam, uint32_t sizeInbyte, const void* data);
		void SetComputeConstantBufferView(uint32_t rootparam, const Resource& resource);
		void SetComputeDescriptorTable(uint32_t rootparam, D3D12_CPU_DESCRIPTOR_HANDLE handle);
		void SetComputeDescriptorTables(uint32_t rootparam, uint32_t numHandles, const D3D12_CPU_DESCRIPTOR_HANDLE* handle);
		void SetComputeShaderResourceView(uint32_t rootparam, uint32_t numElement, uint32_t elementSize, const void* data);
		void SetComputeShaderResourceView(uint32_t rootparam, const Resource& resource);
		void SetComputeUnorderedAccessView(uint32_t rootparam, const Resource& resource);

		template<typename T>
		void SetComputeConstants(uint32_t rootparam, const T& data) {
			SetComputeConstants(rootparam, (uint32_t)(sizeof(T) / 4), &data);
		}
		template<typename T>
		void SetComputeConstantBufferView(uint32_t rootparam, T* data) {
			SetGraphicsConstantBufferView(rootparam, sizeof(T), data);
		}
		template<typename T>
		void SetComputeShaderResourceView(uint32_t rootparam, const std::vector<T>& data)
		{
			SetComputeShaderResourceView(rootparam, (uint32_t)data.size(), (uint32_t)sizeof(T), data.data());
		}

	public:
		void SetViewport(const Viewport& viewport);
		void SetScissorRect(const ScissorRect& rect);
		void SetPipelineState(const PSO& pso);
		void SetRenderTarget(const RenderTarget& rtv);
		void SetGraphicsRootSignature(const RootSignature& rootSig);
		void SetComputeRootSignature(const RootSignature& rootSig);
		void SetVertexBuffer(const VertexBuffer& buffer);
		void SetIndexBuffer(const IndexBuffer& buffer);
		void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY type);
	public:
		void LoadTextureFromFile(const std::string& path, Texture& texture, ETextureUsage usage);
	public:
		// close
		void Close();
		void Reset();
		void FlushTrackObject();
		void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t startVertexLocation = 0, uint32_t startInstanceLocation = 0);
		void DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t startIndexLocation = 0, 
			int baseVertexLocatoin = 0, uint32_t startInstanceLocation = 0);
		void Dispatch(uint32_t thread_group_x, uint32_t thread_group_y = 1, uint32_t thread_group_z = 1);
	private:
		void SetDescriptorHeap(const DescriptorHeap& heap);
		void TrackResource(const Resource& resource);
		void TrackResource(const ComPtr<ID3D12Object>& object);
		void GenerateMipMaps_UAV(const Texture& texture);
		void GenerateMipMaps_BGR(const Texture& texture);
		void GenerateMipMaps_SRGB(const Texture& texture);
	};
}