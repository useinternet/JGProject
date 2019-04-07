#pragma once
#include"DirectXCommon/DirectXCommon.h"

namespace Dx12
{
	class DescriptorAllocation;
	class DescriptorHeap;
	class DescriptorAllocatorPage : public std::enable_shared_from_this<DescriptorAllocatorPage>
	{
		struct FreeBlockInfo;
		struct StaleDescriptorInfo;
	private:
		using OffsetType = uint32_t;
		using SizeType   = uint32_t;


		using FreeListByOffset     = std::map<OffsetType, FreeBlockInfo>;
		using FreeListBySize       = std::multimap<SizeType, FreeListByOffset::iterator>;
		using StaleDescriptorQueue = std::queue<StaleDescriptorInfo>;

		struct FreeBlockInfo
		{
			SizeType Size;
			FreeListBySize::iterator FreeListBySizeIt;

			FreeBlockInfo(SizeType size) : Size(size) {}
		};

		struct StaleDescriptorInfo
		{
			OffsetType Offset;
			SizeType   Size;
			std::uint64_t FrameNumber;



			StaleDescriptorInfo(OffsetType offset, SizeType size, std::uint64_t frame) :
				Offset(offset), Size(size), FrameNumber(frame) { }
		};
	private:
		ID3D12Device* m_Device;

		FreeListByOffset m_FreeListByOffset;
		FreeListBySize   m_FreeListBySize;
		StaleDescriptorQueue m_StaleDescriptors;


		std::shared_ptr<DescriptorHeap> m_DescriptorHeap;
		D3D12_DESCRIPTOR_HEAP_TYPE   m_HeapType;
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_BaseDescriptor;
		std::uint32_t m_DescriptorHandleIncrementSize;
		std::uint32_t m_NumDescriptorInHeap;
		std::uint32_t m_NumFreeHandles;

		std::mutex m_AllocationMutex;
	public:
		DescriptorAllocatorPage(
			D3D12_DESCRIPTOR_HEAP_TYPE type, 
			std::uint32_t numDescriptors);

		D3D12_DESCRIPTOR_HEAP_TYPE GetHeapType() const;
		std::uint32_t NumFreeHandles() const;
		bool HasSpace(std::uint32_t numDescriptors) const;
	


		DescriptorAllocation Allocate(std::uint32_t numDescriptors);

		void Free(DescriptorAllocation&& descriptor, std::uint64_t frameNumber);

		void ReleaseStaleDescriptors(std::uint64_t frameNumber);

	protected:
		std::uint32_t ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE handle);
		void AddNewBlock(std::uint32_t offset, std::uint32_t numDescriptors);
		void FreeBlock(std::uint32_t offset, std::uint32_t numDescriptors);
	};
}