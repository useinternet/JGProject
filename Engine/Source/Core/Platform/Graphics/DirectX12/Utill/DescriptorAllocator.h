#pragma once

#include "JGCore.h"
#include "DirectX12Helper.h"


namespace JG
{
	class DescriptorAllocation;
	class DescriptorAllocator
	{
	public:
		DescriptorAllocation Allocate();
		void                 UpdatePage();
	public:
		class CPUDescriptorPage
		{
		public:
			ComPtr<ID3D12DescriptorHeap>			D3DDescriptorHeap;
			D3D12_DESCRIPTOR_HEAP_TYPE              D3DType;
			Queue<D3D12_CPU_DESCRIPTOR_HANDLE> AbandonedHandles;
			SortedDictionary<u64, Queue<D3D12_CPU_DESCRIPTOR_HANDLE>> PendingHandles;
			u64 HeapSize = 0;
			u64 Offset   = 0;
			std::mutex FreeMutex;
		public:
			bool HasSpace();
			DescriptorAllocation Allocate();
			void UpdatePage();
			void Free(D3D12_CPU_DESCRIPTOR_HANDLE handle);

		};
	public:
		DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type, u64 sizeperHeap = 1024) :
			mD3DType(type), mSizePerHep(sizeperHeap) { }
	private:
		CPUDescriptorPage* RequestPage();
	private:
		u64							   mSizePerHep = 1024;
		D3D12_DESCRIPTOR_HEAP_TYPE     mD3DType;
		List<UniquePtr<DescriptorAllocator::CPUDescriptorPage>> mPages;
		std::mutex                     mAllocateMutex;
	};

	class DescriptorAllocation
	{
		friend DescriptorAllocator::CPUDescriptorPage;
	private:
		DescriptorAllocation(const DescriptorAllocation& copy) = delete;
		DescriptorAllocation& operator=(const DescriptorAllocation& rhs) = delete;
	public:
		DescriptorAllocation() {}
		~DescriptorAllocation();
		DescriptorAllocation(DescriptorAllocation&& rhs);
		DescriptorAllocation& operator=(DescriptorAllocation&& rhs);
	public:
		D3D12_CPU_DESCRIPTOR_HANDLE CPU() const {
			return _CPU;
		}
		bool IsValid() const {
			return _CPU.ptr != 0;
		}
	private:
		D3D12_CPU_DESCRIPTOR_HANDLE _CPU;
		DescriptorAllocator::CPUDescriptorPage* _OwnerPage = nullptr;
	};
	
}



