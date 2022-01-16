#include "pch.h"
#include "DescriptorAllocator.h"
#include "Platform/Graphics/DirectX12/DirectX12API.h"

using namespace std;

namespace JG
{
	DescriptorAllocation DescriptorAllocator::Allocate()
	{
		lock_guard<mutex> lock(mAllocateMutex);
		for (auto& page : mPages)
		{
			if (page->HasSpace())
			{
				return move(page->Allocate());
			}
		}
		auto page = RequestPage();

		return move(page->Allocate());
	}
	void DescriptorAllocator::UpdatePage()
	{
		for (auto& page : mPages)
		{
			page->UpdatePage();
		}
	}
	DescriptorAllocator::CPUDescriptorPage* DescriptorAllocator::RequestPage()
	{
		u64 i = mPages.size();
		auto page = make_unique<CPUDescriptorPage>();
		auto result = page.get();
		page->D3DType  = mD3DType;
		page->HeapSize = mSizePerHep;
		page->D3DDescriptorHeap = CreateD3DDescriptorHeap(DirectX12API::GetD3DDevice(), mD3DType,
			D3D12_DESCRIPTOR_HEAP_FLAG_NONE, (uint32_t)page->HeapSize);
		mPages.push_back(move(page));

		return result;
	}
	bool DescriptorAllocator::CPUDescriptorPage::HasSpace()
	{
		if (!AbandonedHandles.empty())
		{
			return true;
		}
		if (Offset >= HeapSize) return false;
		else return true;
	}
	DescriptorAllocation DescriptorAllocator::CPUDescriptorPage::Allocate()
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE cpu_handle;
		if (!AbandonedHandles.empty())
		{
			cpu_handle = move(AbandonedHandles.front()); AbandonedHandles.pop();
		}
		else
		{
			cpu_handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(D3DDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
			cpu_handle.Offset((uint32_t)Offset++, DirectX12API::GetD3DDevice()->GetDescriptorHandleIncrementSize(D3DType));
		}
		DescriptorAllocation alloc;
		alloc._CPU = cpu_handle;
		alloc._OwnerPage = this;

		return move(alloc);
	}

	void DescriptorAllocator::CPUDescriptorPage::UpdatePage()
	{
		u64 frameBufferIndex = DirectX12API::GetInstance()->GetBufferIndex();
		while (!PendingHandles[frameBufferIndex].empty())
		{
			AbandonedHandles.push(PendingHandles[frameBufferIndex].front());
			PendingHandles[frameBufferIndex].pop();
		}
	}

	void DescriptorAllocator::CPUDescriptorPage::Free(D3D12_CPU_DESCRIPTOR_HANDLE handle)
	{
		lock_guard<mutex> lock(FreeMutex);
		u64 frameBufferIndex = DirectX12API::GetInstance()->GetBufferIndex();

		PendingHandles[frameBufferIndex].push(handle);
	}

	DescriptorAllocation::~DescriptorAllocation()
	{
		if (_OwnerPage)
		{
			_OwnerPage->Free(_CPU);
		}
	}

	DescriptorAllocation::DescriptorAllocation(DescriptorAllocation&& rhs)
	{
		_CPU = rhs._CPU;
		_OwnerPage = rhs._OwnerPage;

		rhs._CPU.ptr = 0;
		rhs._OwnerPage = nullptr;
	}
	
	DescriptorAllocation& DescriptorAllocation::operator=(DescriptorAllocation&& rhs)
	{
		_CPU = rhs._CPU;
		_OwnerPage = rhs._OwnerPage;

		rhs._CPU.ptr = 0;
		rhs._OwnerPage = nullptr;
		return *this;
	}
}