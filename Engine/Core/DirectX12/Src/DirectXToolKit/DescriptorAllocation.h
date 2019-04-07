#pragma once
#include"DirectXCommon/DirectXCommon.h"


namespace Dx12
{
	class DescriptorAllocatorPage;
	class DescriptorAllocation
	{
		D3D12_CPU_DESCRIPTOR_HANDLE m_Descriptor;

		std::uint32_t m_NumHandles;
		std::uint32_t m_DescriptorSize;

		std::shared_ptr<DescriptorAllocatorPage> m_Page;
	public:
		DescriptorAllocation();
		DescriptorAllocation(
			D3D12_CPU_DESCRIPTOR_HANDLE descriptor,
			std::uint32_t numHandles,
			std::uint32_t descriptorSize,
			std::shared_ptr<DescriptorAllocatorPage> page);
		~DescriptorAllocation();

	public:
		DescriptorAllocation(const DescriptorAllocation&) = delete;
		DescriptorAllocation& operator=(const DescriptorAllocation&) = delete;

		DescriptorAllocation(DescriptorAllocation&& allocation);
		DescriptorAllocation& operator=(DescriptorAllocation&& other);

	public:
		bool IsNull() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle(std::uint32_t offset = 0) const;
		std::uint32_t GetNumHandles() const;
		std::shared_ptr<DescriptorAllocatorPage> GetDescriptorAllocatorPage() const;
	private:
		void Free();
	};

}