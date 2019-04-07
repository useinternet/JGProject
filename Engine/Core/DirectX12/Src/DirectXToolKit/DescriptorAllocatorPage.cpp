#include<PCH.h>
#include"DescriptorAllocatorPage.h"
#include"DescriptorAllocation.h"
#include"DescriptorHeap.h"
#include"DirectXCore.h"
#include"DxUtil.h"
using namespace Dx12;
using namespace std;


DescriptorAllocatorPage::DescriptorAllocatorPage(
	D3D12_DESCRIPTOR_HEAP_TYPE type,
	uint32_t                   numDescriptors) :
	m_Device(DxDevice::GetDevice()),
	m_HeapType(type), m_NumDescriptorInHeap(numDescriptors)
{ 
	m_DescriptorHeap = make_unique<DescriptorHeap>(type, numDescriptors);

	m_BaseDescriptor = m_DescriptorHeap->GetStartCPUHeapHandle();
	m_DescriptorHandleIncrementSize = m_Device->GetDescriptorHandleIncrementSize(m_HeapType);
	m_NumFreeHandles = m_NumDescriptorInHeap;


	AddNewBlock(0, m_NumFreeHandles);
}
D3D12_DESCRIPTOR_HEAP_TYPE DescriptorAllocatorPage::GetHeapType() const
{
	return m_HeapType;
}
uint32_t DescriptorAllocatorPage::NumFreeHandles() const
{
	return m_NumFreeHandles;
}

bool DescriptorAllocatorPage::HasSpace(uint32_t numDescriptors) const
{
	return m_FreeListBySize.lower_bound(numDescriptors) != m_FreeListBySize.end();
}

DescriptorAllocation DescriptorAllocatorPage::Allocate(uint32_t numDescriptors)
{
	std::lock_guard<std::mutex> lock(m_AllocationMutex);

	// 할당할 메모리가 여유분의 메모리보다 클 경우 null 메모리 반환
	if (numDescriptors > m_NumFreeHandles)
	{
		return DescriptorAllocation();
	}
	// 해당 메모리 크기보다 같거나 큰 메모리 블록 반복자를 불러온다.
	auto smallestBlockIt = m_FreeListBySize.lower_bound(numDescriptors);
	if (smallestBlockIt == m_FreeListBySize.end())
	{
		// 없으면 null 메모리 반환
		return DescriptorAllocation();
	}

	auto blockSize = smallestBlockIt->first;
	auto offsetIt  = smallestBlockIt->second;

	auto offset = offsetIt->first;

	m_FreeListBySize.erase(smallestBlockIt);
	m_FreeListByOffset.erase(offsetIt);

	auto newOffset = offset + numDescriptors;
	auto newSize = blockSize - numDescriptors;


	if (newSize > 0)
	{
		AddNewBlock(newOffset, newSize);
	}

	m_NumFreeHandles -= numDescriptors;

	return DescriptorAllocation(
		CD3DX12_CPU_DESCRIPTOR_HANDLE(m_BaseDescriptor, offset, m_DescriptorHandleIncrementSize),
		numDescriptors, m_DescriptorHandleIncrementSize, shared_from_this());
}

void DescriptorAllocatorPage::Free(DescriptorAllocation&& descriptor, uint64_t frameNumber)
{
	auto offset = ComputeOffset(descriptor.GetDescriptorHandle());
	std::lock_guard<mutex> lock(m_AllocationMutex);

	m_StaleDescriptors.emplace(offset, descriptor.GetNumHandles(), frameNumber);
}

void DescriptorAllocatorPage::ReleaseStaleDescriptors(uint64_t frameNumber)
{
	lock_guard<mutex> lock(m_AllocationMutex);

	while (!m_StaleDescriptors.empty() && m_StaleDescriptors.front().FrameNumber <= frameNumber)
	{
		auto& staleDesriptor = m_StaleDescriptors.front();

		auto offset = staleDesriptor.Offset;
		auto numDescriptor = staleDesriptor.Size;

		FreeBlock(offset, numDescriptor);
		m_StaleDescriptors.pop();
	}
}

uint32_t DescriptorAllocatorPage::ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	return static_cast<uint32_t>(handle.ptr - m_BaseDescriptor.ptr) / m_DescriptorHandleIncrementSize;
}
void DescriptorAllocatorPage::AddNewBlock(uint32_t offset, uint32_t numDescriptors)
{
	auto offsetIt = m_FreeListByOffset.emplace(offset, numDescriptors);
	auto sizeIt   = m_FreeListBySize.emplace(numDescriptors, offsetIt.first);

	offsetIt.first->second.FreeListBySizeIt = sizeIt;
}
void DescriptorAllocatorPage::FreeBlock(uint32_t offset, uint32_t numDescriptors)
{
	// 다음 블럭 
	auto nextBlockIt = m_FreeListByOffset.upper_bound(offset);
	auto prevBlockIt = nextBlockIt;


	// 만약 다음 블럭이 첫 블럭이 아니라면 
	if (prevBlockIt != m_FreeListByOffset.begin())
	{
		//  전 블럭으로 이동
		--prevBlockIt;
	}
	else 
	{
		prevBlockIt = m_FreeListByOffset.end();
	}

	// 사용 가능한 메모리의 크기를 늘린다.
	m_NumFreeHandles += numDescriptors;


	//
	if (prevBlockIt != m_FreeListByOffset.end() &&
		offset == prevBlockIt->first + prevBlockIt->second.Size)
	{
// PrevBlock.Offset           Offset
 // |                          |
 // |<-----PrevBlock.Size----->|<------Size-------->|
 //
		offset = prevBlockIt->first;
		numDescriptors += prevBlockIt->second.Size;

		// 메모리 제거
		m_FreeListBySize.erase(prevBlockIt->second.FreeListBySizeIt);
		m_FreeListByOffset.erase(prevBlockIt);
	}

	if (nextBlockIt != m_FreeListByOffset.end() &&
		offset + numDescriptors == nextBlockIt->first)
	{ 
// Offset               NextBlock.Offset 
// |                    |
// |<------Size-------->|<-----NextBlock.Size----->|

		numDescriptors += nextBlockIt->second.Size;

		// 제거

		m_FreeListBySize.erase(nextBlockIt->second.FreeListBySizeIt);
		m_FreeListByOffset.erase(nextBlockIt);
	}
	AddNewBlock(offset, numDescriptors);

}