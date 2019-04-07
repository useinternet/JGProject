#pragma once
#include"Buffer.h"


namespace Dx12
{
	class VertexBuffer : public Buffer
	{
	private:
		uint32_t m_NumVertices;
		uint32_t m_VertexStride;

		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
	public:
		VertexBuffer(const std::string& name = "VertexBuffer");
		virtual ~VertexBuffer();

		virtual void CreateViews(uint32_t numElements, uint32_t elementSize) override;

		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const
		{
			return m_VertexBufferView;
		}
		uint32_t GetNumVertices() const {
			return m_NumVertices;
		}
		uint32_t GetVertexStride() const {
			return m_VertexStride;
		}

		virtual D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc = nullptr) const override;
		virtual D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc = nullptr) const override;
	};
}