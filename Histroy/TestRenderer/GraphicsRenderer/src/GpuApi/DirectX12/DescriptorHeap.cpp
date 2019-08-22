#include"pch.h"
#include"DescriptorHeap.h"
#include"GraphicsDevice.h"
#include"Dx12Helper.h"
using namespace std;


namespace GR
{
	namespace Dx12
	{
		DescriptorAllocation::DescriptorAllocation() :
			m_Descriptor{ 0 },
			m_NumHandles(0),
			m_DescriptorSize(0),
			m_Page(nullptr) { }

		DescriptorAllocation::DescriptorAllocation(
			D3D12_CPU_DESCRIPTOR_HANDLE descriptor,
			uint32_t numHandles,
			uint32_t descriptorSize,
			shared_ptr<DescriptorAllocatorPage> page) :
			m_Descriptor(descriptor),
			m_NumHandles(numHandles),
			m_DescriptorSize(descriptorSize),
			m_Page(page) { }

		DescriptorAllocation::~DescriptorAllocation()
		{
			Free();
		}

		DescriptorAllocation::DescriptorAllocation(DescriptorAllocation&& allocation) :
			m_Descriptor(allocation.m_Descriptor),
			m_NumHandles(allocation.m_NumHandles),
			m_DescriptorSize(allocation.m_DescriptorSize),
			m_Page(move(allocation.m_Page))
		{
			allocation.m_Descriptor.ptr = 0;
			allocation.m_NumHandles = 0;
			allocation.m_DescriptorSize = 0;
		}
		DescriptorAllocation& DescriptorAllocation::operator=(DescriptorAllocation&& other)
		{
			Free();

			m_Descriptor = other.m_Descriptor;
			m_NumHandles = other.m_NumHandles;
			m_DescriptorSize = other.m_DescriptorSize;
			m_Page = move(other.m_Page);

			other.m_Descriptor.ptr = 0;
			other.m_NumHandles = 0;
			other.m_DescriptorSize = 0;

			return *this;
		}

		bool DescriptorAllocation::IsNull() const
		{
			return m_Descriptor.ptr == 0;
		}
		D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocation::GetDescriptorHandle(uint32_t offset) const
		{
			assert(offset < m_NumHandles);
			return { m_Descriptor.ptr + (m_DescriptorSize * offset) };
		}
		uint32_t DescriptorAllocation::GetNumHandles() const
		{
			return m_NumHandles;
		}
		shared_ptr<DescriptorAllocatorPage> DescriptorAllocation::GetDescriptorAllocatorPage() const
		{
			return m_Page;
		}

		void DescriptorAllocation::Free()
		{
			if (!IsNull() && m_Page)
			{
				m_Page->Free(std::move(*this), GraphicsDevice::GetEngineFrame());

				m_Descriptor.ptr = 0;
				m_NumHandles = 0;
				m_DescriptorSize = 0;
				m_Page.reset();
			}
		}
		DescriptorAllocatorPage::DescriptorAllocatorPage(
			ID3D12Device* device,
			D3D12_DESCRIPTOR_HEAP_TYPE type,
			uint32_t                   numDescriptors) :
			m_Device(device),
			m_HeapType(type), m_NumDescriptorInHeap(numDescriptors)
		{
			m_DescriptorHeap = CreateD3DDescriptorHeap(m_Device, type, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, numDescriptors);
			m_BaseDescriptor = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
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
			auto offsetIt = smallestBlockIt->second;

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
			auto sizeIt = m_FreeListBySize.emplace(numDescriptors, offsetIt.first);

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

		DescriptorAllocator::DescriptorAllocator(ID3D12Device* device,D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptorPerHeap) :
			m_Device(device),
			m_HeapType(type),
			m_NumDescriptorsPerHeap(numDescriptorPerHeap) { }
		DescriptorAllocator::~DescriptorAllocator() { }

		DescriptorAllocation DescriptorAllocator::Allocate(uint32_t numDescriptors)
		{
			lock_guard<mutex> lock(m_AllocationMutex);

			DescriptorAllocation allocation;

			for (auto iter = m_AvailableHeaps.begin(); iter != m_AvailableHeaps.end(); ++iter)
			{
				auto allocatorPage = m_HeapPool[*iter];
				allocation = allocatorPage->Allocate(numDescriptors);

				if (allocatorPage->NumFreeHandles() == 0)
				{
					iter = m_AvailableHeaps.erase(iter);
				}

				if (!allocation.IsNull())
					break;
			}
			// 메모리 를 저장할수있는 페이지가 없을 경우 새로 만든다.
			if (allocation.IsNull())
			{
				m_NumDescriptorsPerHeap = std::max(m_NumDescriptorsPerHeap, numDescriptors);
				auto newPage = CreateAllocatorPage();

				allocation = newPage->Allocate(numDescriptors);
			}

			return allocation;
		}


		void DescriptorAllocator::ReleaseStaleDescriptors(uint64_t frameNumber)
		{
			lock_guard<mutex> lcok(m_AllocationMutex);

			for (size_t i = 0; i < m_HeapPool.size(); ++i)
			{
				auto page = m_HeapPool[i];

				page->ReleaseStaleDescriptors(frameNumber);

				if (page->NumFreeHandles() > 0)
				{
					m_AvailableHeaps.insert(i);
				}
			}

		}
		shared_ptr<DescriptorAllocatorPage> DescriptorAllocator::CreateAllocatorPage()
		{
			auto newPage = make_shared<DescriptorAllocatorPage>(m_Device, m_HeapType, m_NumDescriptorsPerHeap);

			m_HeapPool.emplace_back(newPage);
			m_AvailableHeaps.insert(m_HeapPool.size() - 1);

			return newPage;
		}
	}
}