#pragma once


#include "JGCore.h"
#include "DirectX12Helper.h"




namespace JG
{
	class UploadAllocator
	{
		
	public:
		class Page;
		struct Allocation
		{
			void* CPU;
			D3D12_GPU_VIRTUAL_ADDRESS GPU;
			Page* OwnerPage = nullptr;
		};
		class Page
		{
			ComPtr<ID3D12Resource> mD3D12Resource;
			//
			void* mCPUptr;
			D3D12_GPU_VIRTUAL_ADDRESS mGPUptr;
			//
			u64 mPageSize;
			u64 mOffset;
		public:
			Page(ID3D12Device* device, u64 sizeInBytes);
			~Page();

			bool HasSpace(u64 sizeInBytes, u64 alignment) const;
			
			UploadAllocator::Allocation Allocate(u64 sizeInBytes, u64 alignment);
			
			void Reset();
			ID3D12Resource* Get() const {
				return mD3D12Resource.Get();
			}
		};
		using PagePool = std::deque<SharedPtr<UploadAllocator::Page>>;

		PagePool mPagePool;
		PagePool mAvailablePages;

		SharedPtr<Page> mCurrentPage;

		u64 mPageSize;

	public:
		explicit UploadAllocator(u64 pageSize = _2MB);
		virtual ~UploadAllocator();
	public:
		u64 GetPageSize() const { return mPageSize; }
		UploadAllocator::Allocation Allocate(u64 sizeInBytes, u64 alignment);
		void Reset();
	private:
		SharedPtr<Page> RequestPage();
	};

	
}