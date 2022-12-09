#pragma once
#include "Core.h"
#include "DirectX12Helper.h"

class PUploadAllocator : public IMemoryObject
{
public:
	class PPage;
	struct HAllocation
	{
		void* CPU;
		D3D12_GPU_VIRTUAL_ADDRESS GPU;
		PPage* OwnerPage = nullptr;
	};


	class PPage : public IMemoryObject
	{
		HDX12ComPtr<HDX12Resource> _d3D12Resource;

		void* _CPU;
		D3D12_GPU_VIRTUAL_ADDRESS _GPU;

		uint64 _pageSize;
		uint64 _offset;
	public:
		PPage(ID3D12Device* device, uint64 sizeInBytes);
		virtual ~PPage();

		bool HasSpace(uint64 sizeInBytes, uint64 alignment) const;

		PUploadAllocator::HAllocation Allocate(uint64 sizeInBytes, uint64 alignment);

		void Reset();
		HDX12Resource* Get() const { return _d3D12Resource.Get(); }
	};

	using HPagePool = HDeque<PSharedPtr<PUploadAllocator::PPage>>;

private:
	HPagePool _pagePool;
	HPagePool _availablePages;

	PSharedPtr<PPage> _currentPage;

	uint64 _pageSize;
public:
	explicit PUploadAllocator(uint64 pageSize = _2MB);
	virtual ~PUploadAllocator() = default;

public:
	uint64 GetPageSize() const { return _pageSize; }
	PUploadAllocator::HAllocation Allocate(uint64 sizeInBytes, uint64 alignment);
	void Reset();

private:
	PSharedPtr<PPage> requestPage();
};