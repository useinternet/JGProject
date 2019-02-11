#pragma once
#include"DxCommon/DxCommon.h"

namespace JGRC
{
	class GpuCpuSynchronizer
	{
	private:
		Microsoft::WRL::ComPtr<ID3D12Fence> Fence;
		UINT64 FenceOffset = 0;
	public:
		void CreateSynchronizer(ID3D12Device* Device)
		{
			ThrowIfFailed(Device->CreateFence(0, D3D12_FENCE_FLAG_NONE,
				IID_PPV_ARGS(Fence.GetAddressOf())));
		}
		ID3D12Fence* GetFence() const {
			return Fence.Get();
		}
		void IncreaseOffset() {
			FenceOffset++;
		}
		UINT64 GetOffset() const {
			return FenceOffset;
		}
	};
}