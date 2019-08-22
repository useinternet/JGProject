#pragma once
#include "DxObject.h"


namespace RE
{
	class DescriptorHeap;
	class DescriptorHandle : DxObject
	{
		friend DescriptorHeap;
	public:
		DescriptorHandle();
		DescriptorHandle(DescriptorHeap* owner, uint32_t block_key);
		DescriptorHandle(DescriptorHandle&& rhs);
		DescriptorHandle& operator=(DescriptorHandle&& rhs);
		~DescriptorHandle();
		D3D12_CPU_DESCRIPTOR_HANDLE CPU() const;
		bool IsValid() const;
	public:
	
		DescriptorHandle(const DescriptorHandle& copy) = delete;
		DescriptorHandle& operator=(const DescriptorHandle& copy) = delete;

	private:
		mutable DescriptorHeap* m_pOwnerHeap;
		uint32_t m_Block_Key;

		D3D12_CPU_DESCRIPTOR_HANDLE m_CPU;
	};
}