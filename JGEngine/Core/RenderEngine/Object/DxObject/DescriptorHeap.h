#pragma once

#include "DxObject.h"
#include "Defines/ReEnumDefines.h"


namespace RE
{

	class DescriptorHandle;
	class DescriptorAllocator;
	class DescriptorHeap : public DxObject
	{
	public:

		struct DescriptorBlock
		{
			uint32_t startPos;
			uint32_t endPos;
			uint32_t numDescriptor;
			bool     isAllocate;
			uint64_t frameCount;
			uint64_t valueIndex;
			DescriptorBlock() :
				startPos(0),
				endPos(0),
				numDescriptor(0),
				frameCount(0),
				valueIndex(0),
				isAllocate(false) {}

			bool HasSpace(uint32_t numDescriptor) const
			{
				if (numDescriptor <= this->numDescriptor && isAllocate == false)
				{
					return true;
				}
				return false;
			}
		};

	public:
		DescriptorHeap(DescriptorAllocator* owner_allocator, uint32_t numDescriptor = 1024);

		// 할당할려는 메모리만큼 공간이 비어져있는지 체크
		bool HasSpace(uint32_t numDescriptor) const;

		// 할당
		DescriptorHandle Allocate(uint32_t numDescriptor);


		// 할당 해제
		void Free(DescriptorHandle* handle);
		void RenewalHandle(const DescriptorHandle* handle);
		//
		void ReleaseDescriptorBlock();

		// Debug
		void GetDebugInfo(Debug::DescriptorHeapInfo& out_debug_info);
	private:
		void FreeBlock(const DescriptorBlock& block);
		DescriptorBlock MergeBlock(const DescriptorBlock& block1, const DescriptorBlock& block2, bool is_allocate);
	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_D3D_Heap;
		std::map<uint32_t, DescriptorBlock> m_DescriptorBlocks;
		std::map<uint64_t, std::vector<DescriptorBlock>> m_FreeReservatedBlocks;


		std::mutex m_Mutex;
		DescriptorAllocator* m_OwnerAllcator;
		uint32_t m_NumDescriptor;
		uint32_t m_IncreasementSize;
	};
}
