#pragma once
#include"DirectXCommon/DirectXCommon.h"
#include"DescriptorAllocation.h"
namespace Dx12
{
	class DescriptorAllocatorPage;
	class DescriptorAllocator
	{
	private:
		using DescriptorHeapPool = std::vector<std::shared_ptr<DescriptorAllocatorPage>>;
		//
		D3D12_DESCRIPTOR_HEAP_TYPE m_HeapType;
		std::uint32_t m_NumDescriptorsPerHeap;
		//
		DescriptorHeapPool m_HeapPool;
		//
		std::set<size_t> m_AvailableHeaps;
		std::mutex m_AllocationMutex;
	public:
		DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type, std::uint32_t numDescriptorPerHeap = 256);
		virtual ~DescriptorAllocator();


		DescriptorAllocation Allocate(std::uint32_t numDescriptors = 1);

		void ReleaseStaleDescriptors(std::uint64_t frameNumber);

	private:
		std::shared_ptr<DescriptorAllocatorPage> CreateAllocatorPage();
	};
}