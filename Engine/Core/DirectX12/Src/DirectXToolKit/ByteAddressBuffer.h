#pragma once
#include"Buffer.h"
#include"DescriptorAllocation.h"

namespace Dx12
{
	class ByteAddressBuffer : public Buffer
	{
		uint32_t m_BufferSize;
		DescriptorAllocation m_SRV;
		DescriptorAllocation m_UAV;
	public:
		ByteAddressBuffer(const std::string& name = "ByteAddressBuffer");
		ByteAddressBuffer(const D3D12_RESOURCE_DESC& desc,
			uint32_t numElements, uint32_t elementSize,
			const std::string& name = "ByteAddressBuffer");


	public:
		uint32_t GetBufferSize() const {
			return m_BufferSize;
		}

		virtual void CreateViews(uint32_t numElements, uint32_t elementSize) override;



		virtual D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc = nullptr) const override
		{
			return m_SRV.GetDescriptorHandle();
		}

		virtual D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc = nullptr) const override
		{
			return m_UAV.GetDescriptorHandle();
		}
	};
}