#pragma once
#include"Buffer.h"


namespace Dx12
{

	class IndexBuffer : public Buffer
	{
	private:
		uint32_t m_NumIndicies;

		DXGI_FORMAT m_IndexFormat;

		D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
	public:
		IndexBuffer(const std::string& name = "IndexBuffer");
		virtual ~IndexBuffer();

		virtual void CreateViews(uint32_t numElements, uint32_t elementSize = sizeof(uint32_t));

		uint32_t GetNumIndicies() const {
			return m_NumIndicies;
		}
		DXGI_FORMAT GetIndexFormat() const {
			return m_IndexFormat;
		}

		D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const {
			return m_IndexBufferView;
		}

		virtual D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc = nullptr) const override;
		virtual D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc = nullptr) const override;
	};
}