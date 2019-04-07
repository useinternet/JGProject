#pragma once

#include"Buffer.h"
#include"ByteAddressBuffer.h"



namespace Dx12
{
	class StructuredBuffer : Buffer
	{
		uint32_t m_NumElements;
		uint32_t m_ElementSize;

		DescriptorAllocation m_SRV;
		DescriptorAllocation m_UAV;

		// structured buffer 을 위한 내부 카운터를 저장하기위한 버퍼
		ByteAddressBuffer m_CounterBuffer;


	public:
		StructuredBuffer(const std::string& name = "StructuredBuffer");
		StructuredBuffer(const D3D12_RESOURCE_DESC& desc, uint32_t numElement, uint32_t elementSize,
			const std::string& name = "StructuredBuffer");

	public:
		virtual uint32_t GetNumElements() const {
			return m_NumElements;
		}
		virtual uint32_t GetElementSize() const {
			return m_ElementSize;
		}

		virtual void CreateViews(uint32_t numElments, uint32_t elementSize) override;

		virtual D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc = nullptr) const
		{
			return m_SRV.GetDescriptorHandle();
		}

		virtual D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc = nullptr) const override
		{
			return m_UAV.GetDescriptorHandle();
		}
		const ByteAddressBuffer& GetCounterBuffer() const
		{
			return m_CounterBuffer;
		}
	};
}