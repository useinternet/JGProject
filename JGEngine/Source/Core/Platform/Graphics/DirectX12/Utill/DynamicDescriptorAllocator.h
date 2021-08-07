﻿#pragma once


#include "JGCore.h"
#include "DirectX12Helper.h"


namespace JG
{
	class RootSignature;
	class DynamicDescriptorAllocator
	{
	private:
		struct CPUCache
		{
			List<D3D12_CPU_DESCRIPTOR_HANDLE> CPUHandles;
			u32 NumDescriptor = 0;
		};
	private:
		ComPtr<ID3D12DescriptorHeap> mD3DHeap;
		SortedDictionary<i32, i32>       mRootParamInitTypeMap;
		SortedDictionary<i32, CPUCache>	 mCPUCache;
		SortedDictionary<u32, D3D12_DESCRIPTOR_RANGE_TYPE> mDescriptorTableType;

		bool mResizeDirty        = false;
		u32 mNumDescriptor		 = 0;
		u32 mPushedHandleOffset  = 0;
		u32 mIncreaseSize		 = 0;
	public:
		DynamicDescriptorAllocator(u32 numDescriptor = 1024);
	public:
		void CommitRootSignature(RootSignature& rootSig);
		void CommitDescriptorTable(u32 rootParam, const List<D3D12_CPU_DESCRIPTOR_HANDLE>& handles);

		void Reset();
		void PushDescriptorTable(ComPtr<ID3D12GraphicsCommandList> d3dCmdList, ComPtr<ID3D12DescriptorHeap> d3dDescriptorHeap, bool is_graphics);


		i32  GetDescriptorInitAsType(u32 rootParam) const;
		D3D12_DESCRIPTOR_RANGE_TYPE GetDescriptorTableType(u32 rootParam) const;
	private:
		void RequestDescriptorHeap();

	};
}

