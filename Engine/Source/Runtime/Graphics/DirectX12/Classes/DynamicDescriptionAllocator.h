#pragma once
#include "Core.h"
#include "DirectX12Helper.h"

class PRootSignature;
class PDynamicDescriptionAllocator : public IMemoryObject
{
private:
	struct HCPUCache
	{
		HList<D3D12_CPU_DESCRIPTOR_HANDLE> CPUHandles;
		uint64 HashCode = 0;
		uint32 NumDescriptor = 0;
	};

private:
	HDX12ComPtr<HDX12DescriptorHeap> _dx12Heap;
	HMap<int32, int32>       _rootParamInitTypeMap;
	HMap<int32, HCPUCache>	 _CPUCache;
	HMap<uint32, D3D12_DESCRIPTOR_RANGE_TYPE> _descriptorTableType;
	HHashMap<uint64, D3D12_GPU_DESCRIPTOR_HANDLE> _descriptorCache;

	bool   _bResizeDirty = false;
	uint32 _numDescriptor      = 0;
	uint32 _pushedHandleOffset = 0;
	uint32 _increaseSize       = 0;

public:
	PDynamicDescriptionAllocator(uint32 numDescriptor = 1024);
	virtual ~PDynamicDescriptionAllocator() = default;
public:
	void CommitRootSignature(PRootSignature& rootSig);
	void CommitDescriptorTable(uint32 rootParam, const HList<D3D12_CPU_DESCRIPTOR_HANDLE>& handles);

	void Reset(bool clearHandleOffset = false);
	HDX12ComPtr<HDX12DescriptorHeap> PushDescriptorTable(HDX12ComPtr<HDX12CommandList> d3dCmdList, HDX12ComPtr<HDX12DescriptorHeap> bindedD3DDescriptorHeap, bool is_graphics);

	int32  GetDescriptorInitAsType(uint32 rootParam) const;
	D3D12_DESCRIPTOR_RANGE_TYPE GetDescriptorTableType(uint32 rootParam) const;
	D3D12_GPU_DESCRIPTOR_HANDLE UploadDirect(D3D12_CPU_DESCRIPTOR_HANDLE handle, HDX12ComPtr<HDX12CommandList> d3dCmdList, HDX12ComPtr<HDX12DescriptorHeap>* d3dDescriptorHeap);

private:
	void RequestDescriptorHeap();

};