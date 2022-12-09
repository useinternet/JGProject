#include "PCH/PCH.h"
#include "UploadAllocator.h"
#include "DirectX12/DirectX12API.h"

PUploadAllocator::PPage::PPage(ID3D12Device* device, uint64 sizeInBytes) :
	_pageSize(sizeInBytes),
	_offset(0),
	_CPU(nullptr),
	_GPU(D3D12_GPU_VIRTUAL_ADDRESS(0))
{
	CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC   resourceDesc   = CD3DX12_RESOURCE_DESC::Buffer(_pageSize);

	_d3D12Resource = HDirectXAPI::CreateCommittedResource("UploadAllocator",
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr);

	_GPU = _d3D12Resource->GetGPUVirtualAddress();
	_d3D12Resource->Map(0, nullptr, &_CPU);
}

PUploadAllocator::PPage::~PPage()
{
	_d3D12Resource->Unmap(0, nullptr);
	_CPU = nullptr;
	_GPU = D3D12_GPU_VIRTUAL_ADDRESS(0);
	HDirectXAPI::DestroyCommittedResource(_d3D12Resource);
	_d3D12Resource.Reset();
	_d3D12Resource = nullptr;
}

bool PUploadAllocator::PPage::HasSpace(uint64 sizeInBytes, uint64 alignment) const
{
	uint64 alignedSize = (sizeInBytes + (alignment - 1)) & ~(alignment - 1);
	uint64 alignedOffset = (_offset + (alignment - 1)) & ~(alignment - 1);

	return alignedOffset + alignedSize <= _pageSize;
}

PUploadAllocator::HAllocation PUploadAllocator::PPage::Allocate(uint64 sizeInBytes, uint64 alignment)
{
	if (HasSpace(sizeInBytes, alignment) == false)
	{
		JG_ASSERT("Full Space in UploadAllocator");
	}

	uint64 alignedSize = (sizeInBytes + (alignment - 1)) & ~(alignment - 1);
	_offset = (_offset + (alignment - 1)) & ~(alignment - 1);

	HAllocation allocation;
	allocation.CPU = static_cast<uint8*>(_CPU) + _offset;
	allocation.GPU = _GPU + _offset;
	allocation.OwnerPage = this;
	_offset += alignedSize;

	return allocation;
}
void PUploadAllocator::PPage::Reset()
{
	_offset = 0;
}


PUploadAllocator::PUploadAllocator(uint64 pageSize) :
	_pageSize(pageSize) { }

PUploadAllocator::HAllocation PUploadAllocator::Allocate(uint64 sizeInBytes, uint64 alignment)
{
	if (sizeInBytes > _pageSize)
	{
		throw std::bad_alloc();
	}

	if (_currentPage == nullptr || _currentPage->HasSpace(sizeInBytes, alignment) == false)
	{
		_currentPage = requestPage();
	}

	return _currentPage->Allocate(sizeInBytes, alignment);
}
void PUploadAllocator::Reset()
{
	_currentPage    = nullptr;
	_availablePages = _pagePool;

	for (auto& page : _availablePages)
	{
		page->Reset();
	}
}

PSharedPtr<PUploadAllocator::PPage> PUploadAllocator::requestPage()
{
	PSharedPtr<PUploadAllocator::PPage> page;

	if (_availablePages.empty() == false)
	{
		page = _availablePages.front();
		_availablePages.pop_front();
	}
	else
	{
		page = ::Allocate<PUploadAllocator::PPage>(HDirectXAPI::GetDevice(), _pageSize);
		_pagePool.push_back(page);
	}

	return page;
}