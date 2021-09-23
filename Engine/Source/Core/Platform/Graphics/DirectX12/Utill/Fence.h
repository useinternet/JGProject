#pragma once
#include "JGCore.h"
#include "DirectX12Helper.h"

namespace JG
{
	class Fence
	{
	private:
		ComPtr<ID3D12Fence>  mD3DFence;
		std::mutex           mEventMutex;
		//std::atomic_uint64_t mFenceValue;
		std::uint64_t mFenceValue;
		HANDLE               mFenceEvent;

	
	public:
		Fence();

		
		uint64_t GetValue() const {
			return mFenceValue;
		}
		// Fence 占쏙옙 占쏙옙占쏙옙
		void IncreaseValue() {
			++mFenceValue;
			//mFenceValue.fetch_add(1);
		}
		void WaitForFenceValue(uint64_t value, std::chrono::milliseconds duration = std::chrono::milliseconds::max());

		ID3D12Fence* Get() const {
			return mD3DFence.Get();
		}
	};
}



