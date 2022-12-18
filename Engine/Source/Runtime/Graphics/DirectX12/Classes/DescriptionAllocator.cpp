#include "PCH/PCH.h"
#include "DescriptionAllocator.h"
#include "DirectX12/DirectX12API.h"

HDescriptionAllocation PDescriptionAllocator::Allocate()
{
	HLockGuard<HMutex> lock(_allocateMutex);
	for (auto& page : _pages)
	{
		if (page->HasSpace())
		{
			return std::move(page->Allocate());
		}
	}
	auto page = RequestPage();

	return std::move(page->Allocate());
}

void PDescriptionAllocator::UpdatePage()
{
	for (auto& page : _pages)
	{
		page->UpdatePage();
	}
}
PDescriptionAllocator::PCPUDescriptionPage* PDescriptionAllocator::RequestPage()
{
	uint64 i = _pages.size();
	PSharedPtr<PCPUDescriptionPage> page = ::Allocate<PCPUDescriptionPage>();
	PCPUDescriptionPage* result = page.GetRawPointer();
	page->D3DType  = _descriptorHeapType;
	page->HeapSize = _sizePerHeap;
	page->D3DDescriptorHeap = HDirectX12Helper::CreateD3DDescriptorHeap(HDirectXAPI::GetDevice(), _descriptorHeapType,
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE, (uint32)page->HeapSize);

	_pages.push_back(std::move(page));

	return result;
}
bool PDescriptionAllocator::PCPUDescriptionPage::HasSpace()
{
	if (!AbandonedHandles.empty())
	{
		return true;
	}

	if (Offset >= HeapSize) return false;
	else return true;
}

HDescriptionAllocation PDescriptionAllocator::PCPUDescriptionPage::Allocate()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpu_handle;
	if (!AbandonedHandles.empty())
	{
		cpu_handle = std::move(AbandonedHandles.front()); AbandonedHandles.pop();
	}
	else
	{
		cpu_handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(D3DDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		cpu_handle.Offset((uint32_t)Offset++, HDirectXAPI::GetDevice()->GetDescriptorHandleIncrementSize(D3DType));
	}

	HDescriptionAllocation alloc;
	alloc._CPU = cpu_handle;
	alloc._ownerPage = this;

	return std::move(alloc);
}

void PDescriptionAllocator::PCPUDescriptionPage::UpdatePage()
{
	while (!PendingHandles.empty())
	{
		AbandonedHandles.push(PendingHandles.front());
		PendingHandles.pop();
	}
}

void PDescriptionAllocator::PCPUDescriptionPage::Free(D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	HLockGuard<HMutex> lock(FreeMutex);
	PendingHandles.push(handle);
}

HDescriptionAllocation::~HDescriptionAllocation()
{
	if (_ownerPage)
	{
		_ownerPage->Free(_CPU);
	}
}

HDescriptionAllocation::HDescriptionAllocation(HDescriptionAllocation&& rhs)
{
	_CPU = rhs._CPU;
	_ownerPage = rhs._ownerPage;

	rhs._CPU.ptr = 0;
	rhs._ownerPage = nullptr;
}

HDescriptionAllocation& HDescriptionAllocation::operator=(HDescriptionAllocation&& rhs)
{
	_CPU = rhs._CPU;
	_ownerPage = rhs._ownerPage;

	rhs._CPU.ptr = 0;
	rhs._ownerPage = nullptr;

	return *this;
}