#include "pch.h"
#include "DescriptorHandle.h"
#include "DescriptorHeap.h"



namespace RE
{
	DescriptorHandle::DescriptorHandle() : 
		DxObject("DescriptorHandle"),
		m_pOwnerHeap(nullptr),
		m_Block_Key(-1),
		m_CPU({ 0 })
	{

	}
	DescriptorHandle::DescriptorHandle(DescriptorHeap* owner, uint32_t block_key) :
		DxObject("DescriptorHandle"),
		m_pOwnerHeap(owner),
		m_Block_Key(block_key),
		m_CPU({ 0 })
	{

	}
	DescriptorHandle::DescriptorHandle(DescriptorHandle&& rhs) :
		m_pOwnerHeap(rhs.m_pOwnerHeap),
		m_Block_Key(rhs.m_Block_Key),
		m_CPU(rhs.m_CPU)
	{
		rhs.m_pOwnerHeap = nullptr;
		rhs.m_Block_Key = -1;
		rhs.m_CPU = { 0 };
	}
	DescriptorHandle& DescriptorHandle::operator=(DescriptorHandle&& rhs)
	{
		m_pOwnerHeap = rhs.m_pOwnerHeap;
		m_Block_Key = rhs.m_Block_Key;
		m_CPU = rhs.m_CPU;

		rhs.m_pOwnerHeap = nullptr;
		rhs.m_Block_Key = -1;
		rhs.m_CPU = { 0 };

		return *this;
	}
	DescriptorHandle::~DescriptorHandle()
	{
		if (m_pOwnerHeap && m_Block_Key != -1)
		{
			m_pOwnerHeap->Free(this);
		}


		m_pOwnerHeap = nullptr;
		m_Block_Key = -1;
		m_CPU = { 0 };
	}
	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle::CPU() const
	{
		m_pOwnerHeap->RenewalHandle(this);
		return m_CPU;
	}
	bool DescriptorHandle::IsValid() const
	{
		return m_CPU.ptr;
	}
}