#pragma once
#include"Dx12Include.h"
#include<queue>
#include<map>
#include<mutex>
#include<set>
namespace GR
{
	namespace Dx12
	{
		class Dx12GraphicsDevice;
		class DescriptorAllocatorPage;
		class DescriptorAllocator;
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


		class DescriptorAllocatorPage : public std::enable_shared_from_this<DescriptorAllocatorPage>
		{
			struct FreeBlockInfo;
			struct StaleDescriptorInfo;
		private:
			using OffsetType = uint32_t;
			using SizeType = uint32_t;


			using FreeListByOffset = std::map<OffsetType, FreeBlockInfo>;
			using FreeListBySize = std::multimap<SizeType, FreeListByOffset::iterator>;
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


			ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap;
			D3D12_DESCRIPTOR_HEAP_TYPE   m_HeapType;
			CD3DX12_CPU_DESCRIPTOR_HANDLE m_BaseDescriptor;
			std::uint32_t m_DescriptorHandleIncrementSize;
			std::uint32_t m_NumDescriptorInHeap;
			std::uint32_t m_NumFreeHandles;

			std::mutex m_AllocationMutex;
		public:
			DescriptorAllocatorPage(
				ID3D12Device* device,
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

		class DescriptorAllocator
		{
		private:
			using DescriptorHeapPool = std::vector<std::shared_ptr<DescriptorAllocatorPage>>;
			//
			ID3D12Device* m_Device;
			D3D12_DESCRIPTOR_HEAP_TYPE m_HeapType;
			std::uint32_t m_NumDescriptorsPerHeap;
			//
			DescriptorHeapPool m_HeapPool;
			//
			std::set<size_t> m_AvailableHeaps;
			std::mutex m_AllocationMutex;
		public:
			DescriptorAllocator(ID3D12Device* device,D3D12_DESCRIPTOR_HEAP_TYPE type, std::uint32_t numDescriptorPerHeap = 256);
			virtual ~DescriptorAllocator();


			DescriptorAllocation Allocate(std::uint32_t numDescriptors = 1);

			void ReleaseStaleDescriptors(std::uint64_t frameNumber);

		private:
			std::shared_ptr<DescriptorAllocatorPage> CreateAllocatorPage();
		};

	}
}