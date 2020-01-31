#pragma once
#include "DxObject.h"
#include "Defines/ReEnumDefines.h"




namespace RE
{
	class DescriptorHandle;
	class DescriptorHeap;
	class DescriptorAllocator : public DxObject
	{
	public:
		
	public:
		DescriptorAllocator(
			D3D12_DESCRIPTOR_HEAP_TYPE type,
			uint32_t numDescriptor_per_heap = 1024);
	public:
		DescriptorHandle Allocate(uint32_t numDescriptor = 1);
		void ReleaseFreeReserveredDescriptorHandles();
		D3D12_DESCRIPTOR_HEAP_TYPE  GetType() const;
		// void GetDebugInfo(Debug::DescritporAllocatorInfo& out_debug_info);
	private:
		uint32_t m_NumDescriptorPerHeap;
		D3D12_DESCRIPTOR_HEAP_TYPE m_Type;
		std::vector<std::shared_ptr<DescriptorHeap>> m_DescriptorHeaps;
		std::mutex m_Mutex;
	};
}