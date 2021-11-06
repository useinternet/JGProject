#include "pch.h"
#include "UploadAllocator.h"
#include "Platform/Graphics/DirectX12/DirectX12API.h"

namespace JG
{
	UploadAllocator::Page::Page(ID3D12Device* device, u64 sizeInBytes) :
		mPageSize(sizeInBytes),
		mOffset(0),
		mCPUptr(nullptr),
		mGPUptr(D3D12_GPU_VIRTUAL_ADDRESS(0))
	{
		mD3D12Resource = DirectX12API::CreateCommittedResource("UploadAllocator",
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(mPageSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr);
		mGPUptr = mD3D12Resource->GetGPUVirtualAddress();
		mD3D12Resource->Map(0, nullptr, &mCPUptr);
	}
	UploadAllocator::Page::~Page()
	{
		mD3D12Resource->Unmap(0, nullptr);
		mCPUptr = nullptr;
		mGPUptr = D3D12_GPU_VIRTUAL_ADDRESS(0);
		DirectX12API::DestroyCommittedResource(mD3D12Resource);
		mD3D12Resource.Reset();
		mD3D12Resource = nullptr;
	}

	bool UploadAllocator::Page::HasSpace(u64 sizeInBytes, u64 alignment) const
	{
		u64 alignedSize = (sizeInBytes + (alignment - 1)) & ~(alignment - 1);
		u64 alignedOffset = (mOffset + (alignment - 1)) & ~(alignment - 1);

		return alignedOffset + alignedSize <= mPageSize;
	}

	UploadAllocator::Allocation UploadAllocator::Page::Allocate(u64 sizeInBytes, u64 alignment)
	{
		if (!HasSpace(sizeInBytes, alignment))
		{
			throw std::bad_alloc();
		}

		u64 alignedSize = (sizeInBytes + (alignment - 1)) & ~(alignment - 1);
		mOffset = (mOffset + (alignment - 1)) & ~(alignment - 1);

		Allocation allocation;
		allocation.CPU = static_cast<uint8_t*>(mCPUptr) + mOffset;
		allocation.GPU = mGPUptr + mOffset;
		allocation.OwnerPage = this;
		mOffset += alignedSize;

		return allocation;
	}
	void UploadAllocator::Page::Reset()
	{
		mOffset = 0;
	}


	UploadAllocator::UploadAllocator(u64 pageSize) :
		mPageSize(pageSize) { }
	UploadAllocator::~UploadAllocator() {}

	UploadAllocator::Allocation UploadAllocator::Allocate(u64 sizeInBytes, u64 alignment)
	{
		if (sizeInBytes > mPageSize)
		{
			throw std::bad_alloc();
		}

		if (!mCurrentPage || !mCurrentPage->HasSpace(sizeInBytes, alignment))
		{
			mCurrentPage = RequestPage();
		}

		return mCurrentPage->Allocate(sizeInBytes, alignment);
	}
	void UploadAllocator::Reset()
	{
		mCurrentPage = nullptr;
		mAvailablePages = mPagePool;

		for (auto& page : mAvailablePages)
		{
			page->Reset();
		}
	}

	std::shared_ptr<UploadAllocator::Page> UploadAllocator::RequestPage()
	{
		std::shared_ptr<UploadAllocator::Page> page;

		if (!mAvailablePages.empty())
		{
			page = mAvailablePages.front();
			mAvailablePages.pop_front();
		}
		else
		{
			page = CreateSharedPtr<UploadAllocator::Page>(DirectX12API::GetD3DDevice(), mPageSize);
			mPagePool.push_back(page);
		}
		return page;
	}
}