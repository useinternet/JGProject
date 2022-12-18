#pragma once
#include "Core.h"
#include "DirectX12Helper.h"

class HDescriptionAllocation;
class PDescriptionAllocator : public IMemoryObject
{
public:
	HDescriptionAllocation Allocate();
	void                  UpdatePage();

public:
	class PCPUDescriptionPage : public IMemoryObject
	{
	public:
		HDX12ComPtr<HDX12DescriptorHeap>		D3DDescriptorHeap;
		D3D12_DESCRIPTOR_HEAP_TYPE              D3DType;
		HQueue<D3D12_CPU_DESCRIPTOR_HANDLE> AbandonedHandles;
		HQueue<D3D12_CPU_DESCRIPTOR_HANDLE> PendingHandles;
		uint64 HeapSize = 0;
		uint64 Offset   = 0;
		HMutex FreeMutex;

	public:
		virtual ~PCPUDescriptionPage() = default;

	public:
		bool HasSpace();
		HDescriptionAllocation Allocate();
		void UpdatePage();
		void Free(D3D12_CPU_DESCRIPTOR_HANDLE handle);

	};
public:
	PDescriptionAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type, uint64 sizeperHeap = 1024) :
		_descriptorHeapType(type), _sizePerHeap(sizeperHeap) { }

	virtual ~PDescriptionAllocator() = default;
private:
	PCPUDescriptionPage* RequestPage();
private:
	uint64 _sizePerHeap = 1024;
	D3D12_DESCRIPTOR_HEAP_TYPE     _descriptorHeapType;
	HList<PSharedPtr<PDescriptionAllocator::PCPUDescriptionPage>> _pages;
	HMutex                     _allocateMutex;
};

class HDescriptionAllocation
{
	friend PDescriptionAllocator::PCPUDescriptionPage;
private:
	HDescriptionAllocation(const HDescriptionAllocation& copy) = delete;
	HDescriptionAllocation& operator=(const HDescriptionAllocation& rhs) = delete;
public:
	HDescriptionAllocation() {}
	~HDescriptionAllocation();
	HDescriptionAllocation(HDescriptionAllocation&& rhs);
	HDescriptionAllocation& operator=(HDescriptionAllocation&& rhs);
public:
	D3D12_CPU_DESCRIPTOR_HANDLE CPU() const {
		return _CPU;
	}

	bool IsValid() const {
		return _CPU.ptr != 0;
	}

private:
	D3D12_CPU_DESCRIPTOR_HANDLE _CPU;
	PDescriptionAllocator::PCPUDescriptionPage* _ownerPage = nullptr;
};