#include "pch.h"
#include "DynamicDescriptorAllocator.h"
#include "Platform/Graphics/DirectX12/DirectX12API.h"
#include "RootSignature.h"

namespace JG
{
	DynamicDescriptorAllocator::DynamicDescriptorAllocator(u32 numDescriptor) : mNumDescriptor(numDescriptor)
	{
		mIncreaseSize = DirectX12API::GetD3DDevice()->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
		);
		mD3DHeap = CreateD3DDescriptorHeap(DirectX12API::GetD3DDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, mNumDescriptor);


	
	}

	void DynamicDescriptorAllocator::CommitRootSignature(RootSignature& rootSig)
	{
		Reset();
		for (i32 i = 0; i < (i32)rootSig.mRootSigInitType.size(); ++i)
		{
			mRootParamInitTypeMap[i] = rootSig.mRootSigInitType[i];
		}

		for (auto& info_pair : rootSig.mDescriptorTableInfoByRootParam)
		{
			u32 rootParam	   = info_pair.first;
			u32 numDescriptor  = info_pair.second.NumDescirptor;
			auto     rangeType = info_pair.second.Type;

			mDescriptorTableType[rootParam]    = rangeType;
			mCPUCache[rootParam].NumDescriptor = numDescriptor;
		}
	}
	void DynamicDescriptorAllocator::CommitDescriptorTable(u32 rootParam, const List<D3D12_CPU_DESCRIPTOR_HANDLE>& handles)
	{
		mCPUCache[rootParam].CPUHandles = handles;
		u64 hash = handles.size();
		for (auto& handle : handles)
		{
			hash ^= handle.ptr + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		}
		mCPUCache[rootParam].HashCode = hash;
	}
	void DynamicDescriptorAllocator::Reset(bool clearHandleOffset)
	{
		if (clearHandleOffset)
		{
			mPushedHandleOffset = 1;
		}
		mDescriptorCache.clear();
		mRootParamInitTypeMap.clear();
		mDescriptorTableType.clear();
		mCPUCache.clear();
	}


	void DynamicDescriptorAllocator::PushDescriptorTable(ComPtr<ID3D12GraphicsCommandList> d3dCmdList, ComPtr<ID3D12DescriptorHeap>& d3dDescriptorHeap, bool is_graphics)
	{
		if (d3dDescriptorHeap.Get() != mD3DHeap.Get())
		{
			RequestDescriptorHeap();
			d3dDescriptorHeap = mD3DHeap;
			d3dCmdList->SetDescriptorHeaps(1, d3dDescriptorHeap.GetAddressOf());
		}
		
		if (!mCPUCache.empty())
		{
			List<D3D12_CPU_DESCRIPTOR_HANDLE> cpu_Handles;
			for (auto& cache_pair : mCPUCache)
			{
				i32  rootParam = cache_pair.first;
				u64  hashCode = cache_pair.second.HashCode;
				D3D12_GPU_DESCRIPTOR_HANDLE submitGPU;
				if (mDescriptorCache.find(hashCode) == mDescriptorCache.end())
				{
					auto handles = cache_pair.second.CPUHandles.data();
					CD3DX12_CPU_DESCRIPTOR_HANDLE startCPU(mD3DHeap->GetCPUDescriptorHandleForHeapStart());
					CD3DX12_GPU_DESCRIPTOR_HANDLE startGPU(mD3DHeap->GetGPUDescriptorHandleForHeapStart());

					u32 handleCount = (u32)cache_pair.second.CPUHandles.size();
					if (handleCount == 0) continue;
					auto gpu = startGPU.Offset(mPushedHandleOffset, mIncreaseSize);
					auto cpu = startCPU.Offset(mPushedHandleOffset, mIncreaseSize);

					mPushedHandleOffset += handleCount;
					if (mPushedHandleOffset >= mNumDescriptor)
					{
						mPushedHandleOffset = mNumDescriptor;
						JG_CORE_CRITICAL("Need Add DynamicDescriptorAllocator Size  {0} => {1} ", mNumDescriptor, mNumDescriptor * 2);
					}

					List<u32> srcDescriptorRangeSize(handleCount, 1);

					DirectX12API::GetD3DDevice()->CopyDescriptors(
						1, &cpu, &handleCount,
						handleCount, handles, srcDescriptorRangeSize.data(),
						D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

					submitGPU = gpu;
					mDescriptorCache[hashCode] = gpu;
				}
				else
				{
					submitGPU = mDescriptorCache[hashCode];
				}

				if (is_graphics)
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
	}
	i32 DynamicDescriptorAllocator::GetDescriptorInitAsType(u32 rootParam) const
	{
		JGASSERT_IF(mRootParamInitTypeMap.find(rootParam) != mRootParamInitTypeMap.end(),
			"DynamicDescriptorAllocator::GetDescriptorInitAsType  :  NonExistent rootParam");
		return mRootParamInitTypeMap.at(rootParam);
	}
	D3D12_DESCRIPTOR_RANGE_TYPE DynamicDescriptorAllocator::GetDescriptorTableType(u32 rootParam) const
	{
		JGASSERT_IF(mDescriptorTableType.find(rootParam) != mDescriptorTableType.end(),
			"DynamicDescriptorAllocator::GetDescriptorTableType  :  NonExistent rootParam");
		return mDescriptorTableType.at(rootParam);
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DynamicDescriptorAllocator::UploadDirect(D3D12_CPU_DESCRIPTOR_HANDLE handle, ComPtr<ID3D12GraphicsCommandList> d3dCmdList, ComPtr<ID3D12DescriptorHeap>* d3dDescriptorHeap)
	{
		if (d3dDescriptorHeap->Get() != mD3DHeap.Get())
		{
			RequestDescriptorHeap();
			*d3dDescriptorHeap = mD3DHeap;
			d3dCmdList->SetDescriptorHeaps(1, d3dDescriptorHeap->GetAddressOf());
		}
		CD3DX12_CPU_DESCRIPTOR_HANDLE startCPU(mD3DHeap->GetCPUDescriptorHandleForHeapStart());
		CD3DX12_GPU_DESCRIPTOR_HANDLE startGPU(mD3DHeap->GetGPUDescriptorHandleForHeapStart());

		auto gpu = startGPU.Offset(mPushedHandleOffset, mIncreaseSize);
		auto cpu = startCPU.Offset(mPushedHandleOffset, mIncreaseSize);

		DirectX12API::GetD3DDevice()->CopyDescriptorsSimple(1, cpu, handle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


		mPushedHandleOffset += 1;

		return gpu;
	}

	void DynamicDescriptorAllocator::RequestDescriptorHeap()
	{
		if (mResizeDirty == false)
		{
			return;
		}
		JG_CORE_ERROR("Resize DynamicDescriptorHeap Size {0} => {1}", mNumDescriptor, mNumDescriptor * 2);
		mNumDescriptor *= 2;

		mD3DHeap = CreateD3DDescriptorHeap(DirectX12API::GetD3DDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, mNumDescriptor);
	}

}