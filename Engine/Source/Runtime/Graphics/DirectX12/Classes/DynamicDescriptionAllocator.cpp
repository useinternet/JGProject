#include "PCH/PCH.h"
#include "DynamicDescriptionAllocator.h"
#include "DirectX12/DirectX12API.h"
#include "RootSignature.h"

PDynamicDescriptionAllocator::PDynamicDescriptionAllocator(uint32 numDescriptor) : _numDescriptor(numDescriptor)
{
	_increaseSize = HDirectXAPI::GetDevice()->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
	);

	_dx12Heap = HDirectX12Helper::CreateD3DDescriptorHeap(HDirectXAPI::GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, _numDescriptor);
}

void PDynamicDescriptionAllocator::CommitRootSignature(PRootSignature& rootSig)
{
	Reset();
	for (int32 i = 0; i < (int32)rootSig._rootSigInitType.size(); ++i)
	{
		_rootParamInitTypeMap[i] = rootSig._rootSigInitType[i];
	}

	for (auto& info_pair : rootSig._descriptorTableInfoByRootParam)
	{
		uint32 rootParam = info_pair.first;
		uint32 numDescriptor = info_pair.second.NumDescirptor;
		auto     rangeType = info_pair.second.Type;

		_descriptorTableType[rootParam] = rangeType;
		_CPUCache[rootParam].NumDescriptor = numDescriptor;
	}
}

void PDynamicDescriptionAllocator::CommitDescriptorTable(uint32 rootParam, const HList<D3D12_CPU_DESCRIPTOR_HANDLE>& handles)
{
	_CPUCache[rootParam].CPUHandles = handles;
	uint64 hash = handles.size();
	for (const D3D12_CPU_DESCRIPTOR_HANDLE& handle : handles)
	{
		hash ^= handle.ptr + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	}

	_CPUCache[rootParam].HashCode = hash;
}

void PDynamicDescriptionAllocator::Reset(bool clearHandleOffset)
{
	if (clearHandleOffset)
	{
		_pushedHandleOffset = 0;
	}

	_descriptorCache.clear();
	_rootParamInitTypeMap.clear();
	_descriptorTableType.clear();
	_CPUCache.clear();
}

ComPtr<ID3D12DescriptorHeap> PDynamicDescriptionAllocator::PushDescriptorTable(HDX12ComPtr<HDX12CommandList> d3dCmdList, HDX12ComPtr<HDX12DescriptorHeap> bindedD3DDescriptorHeap, bool bGraphics)
{
	if (bindedD3DDescriptorHeap.Get() != _dx12Heap.Get())
	{
		RequestDescriptorHeap();
		d3dCmdList->SetDescriptorHeaps(1, _dx12Heap.GetAddressOf());
	}

	if (!_CPUCache.empty())
	{
		HList<D3D12_CPU_DESCRIPTOR_HANDLE> cpu_Handles;
		for (HPair<const int32, HCPUCache>& cache_pair : _CPUCache)
		{
			int32  rootParam = cache_pair.first;
			uint64  hashCode = cache_pair.second.HashCode;
			D3D12_GPU_DESCRIPTOR_HANDLE submitGPU;
			if (_descriptorCache.find(hashCode) == _descriptorCache.end())
			{
				D3D12_CPU_DESCRIPTOR_HANDLE* handles = cache_pair.second.CPUHandles.data();
				CD3DX12_CPU_DESCRIPTOR_HANDLE startCPU(_dx12Heap->GetCPUDescriptorHandleForHeapStart());
				CD3DX12_GPU_DESCRIPTOR_HANDLE startGPU(_dx12Heap->GetGPUDescriptorHandleForHeapStart());

				uint32 handleCount = (uint32)cache_pair.second.CPUHandles.size();
				if (handleCount == 0) continue;
				CD3DX12_GPU_DESCRIPTOR_HANDLE gpu = startGPU.Offset(_pushedHandleOffset, _increaseSize);
				CD3DX12_CPU_DESCRIPTOR_HANDLE cpu = startCPU.Offset(_pushedHandleOffset, _increaseSize);

				_pushedHandleOffset += handleCount;
				if (_pushedHandleOffset >= _numDescriptor)
				{
					_pushedHandleOffset = _numDescriptor;
					JG_LOG(Graphics, ELogLevel::Error, "Need Add DynamicDescriptorAllocator Size  {0} => {1} ", _numDescriptor, _numDescriptor * 2);
				}

				HList<uint32> srcDescriptorRangeSize(handleCount, 1);

				HDirectXAPI::GetDevice()->CopyDescriptors(
					1, &cpu, &handleCount,
					handleCount, handles, srcDescriptorRangeSize.data(),
					D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				submitGPU = gpu;
				_descriptorCache[hashCode] = gpu;
			}
			else
			{
				submitGPU = _descriptorCache[hashCode];
			}

			if (bGraphics)
			{
				d3dCmdList->SetGraphicsRootDescriptorTable(rootParam, submitGPU);
			}
			else
			{
				d3dCmdList->SetComputeRootDescriptorTable(rootParam, submitGPU);
			}

			cache_pair.second.CPUHandles.clear();
		}
	}

	return _dx12Heap;
}
int32 PDynamicDescriptionAllocator::GetDescriptorInitAsType(uint32 rootParam) const
{
	JG_ASSERT_IF(_rootParamInitTypeMap.find(rootParam) != _rootParamInitTypeMap.end(),
		"DynamicDescriptorAllocator::GetDescriptorInitAsType  :  NonExistent rootParam");

	return _rootParamInitTypeMap.at(rootParam);
}
D3D12_DESCRIPTOR_RANGE_TYPE PDynamicDescriptionAllocator::GetDescriptorTableType(uint32 rootParam) const
{
	JG_ASSERT_IF(_descriptorTableType.find(rootParam) != _descriptorTableType.end(),
		"DynamicDescriptorAllocator::GetDescriptorTableType  :  NonExistent rootParam");

	return _descriptorTableType.at(rootParam);
}

D3D12_GPU_DESCRIPTOR_HANDLE PDynamicDescriptionAllocator::UploadDirect(D3D12_CPU_DESCRIPTOR_HANDLE handle, HDX12ComPtr<HDX12CommandList> d3dCmdList, HDX12ComPtr<HDX12DescriptorHeap>* d3dDescriptorHeap)
{
	if (d3dDescriptorHeap->Get() != _dx12Heap.Get())
	{
		RequestDescriptorHeap();
		*d3dDescriptorHeap = _dx12Heap;
		d3dCmdList->SetDescriptorHeaps(1, d3dDescriptorHeap->GetAddressOf());
	}
	CD3DX12_CPU_DESCRIPTOR_HANDLE startCPU(_dx12Heap->GetCPUDescriptorHandleForHeapStart());
	CD3DX12_GPU_DESCRIPTOR_HANDLE startGPU(_dx12Heap->GetGPUDescriptorHandleForHeapStart());

	CD3DX12_GPU_DESCRIPTOR_HANDLE gpu = startGPU.Offset(_pushedHandleOffset, _increaseSize);
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpu = startCPU.Offset(_pushedHandleOffset, _increaseSize);

	HDirectXAPI::GetDevice()->CopyDescriptorsSimple(1, cpu, handle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	_pushedHandleOffset += 1;

	return gpu;
}

void PDynamicDescriptionAllocator::RequestDescriptorHeap()
{
	if (_bResizeDirty == false)
	{
		return;
	}

	JG_LOG(Graphics, ELogLevel::Error, "Resize DynamicDescriptorHeap Size {0} => {1}", _numDescriptor, _numDescriptor * 2);

	_numDescriptor *= 2;

	_dx12Heap = HDirectX12Helper::CreateD3DDescriptorHeap(HDirectXAPI::GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, _numDescriptor);
}