#include "pch.h"
#include "DescriptorAllocator.h"
#include "DescriptorHeap.h"
#include "DescriptorHandle.h"
using namespace std;


namespace RE
{

	DescriptorAllocator::DescriptorAllocator(
		D3D12_DESCRIPTOR_HEAP_TYPE type,
		uint32_t numDescriptor_per_heap) : DxObject("DescriptorAllocator"),
		m_NumDescriptorPerHeap(numDescriptor_per_heap),
		m_Type(type)
	{
		auto heap = make_shared<DescriptorHeap>(this, numDescriptor_per_heap);
		m_DescriptorHeaps.push_back(move(heap));

	}
	DescriptorHandle DescriptorAllocator::Allocate(uint32_t numDescriptor)
	{
		lock_guard<mutex> lock(m_Mutex);
		bool full_space = true;
		for (auto& heap : m_DescriptorHeaps)
		{
			if (heap->HasSpace(numDescriptor))
			{
				full_space = false;
				return move(heap->Allocate(numDescriptor));
			}
		}

		if (full_space)
		{
			auto heap = make_shared<DescriptorHeap>(this, m_NumDescriptorPerHeap);
			auto p_heap = heap.get();
			m_DescriptorHeaps.push_back(move(heap));



			return move(p_heap->Allocate(numDescriptor));
		}
		return DescriptorHandle(nullptr, -1);
	}
	void DescriptorAllocator::ReleaseFreeReserveredDescriptorHandles()
	{
		for (auto& heap : m_DescriptorHeaps)
		{
			heap->ReleaseDescriptorBlock();
		}
	}
	D3D12_DESCRIPTOR_HEAP_TYPE  DescriptorAllocator::GetType() const
	{
		return m_Type;
	}
	//void DescriptorAllocator::GetDebugInfo(Debug::DescritporAllocatorInfo& out_debug_info)
	//{
	//	lock_guard<mutex> lock(m_Mutex);
	//	uint32_t count = 0;
	//	for (auto& heap : m_DescriptorHeaps)
	//	{
	//		Debug::DescriptorHeapInfo heap_info;
	//		heap_info.num = count++;
	//		heap->GetDebugInfo(heap_info);
	//		out_debug_info.descriptor_heapInfo.push_back(heap_info);
	//	}
	//}
}