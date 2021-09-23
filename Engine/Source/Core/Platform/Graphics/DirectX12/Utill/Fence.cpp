#include "pch.h"
#include "Fence.h"
#include "../DirectX12API.h"


namespace JG
{
	Fence::Fence()
	{
		mFenceEvent = ::CreateEvent(nullptr, false, false, nullptr);
		JGASSERT_IF(mFenceEvent != nullptr, "Failed to create fence event.");


		HRESULT hr = DirectX12API::GetD3DDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mD3DFence));
		JGASSERT_IF(SUCCEEDED(hr) , "Failed Create Fence Object");
	}

	void Fence::WaitForFenceValue(uint64_t value, std::chrono::milliseconds duration)
	{
		std::lock_guard<std::mutex> lock(mEventMutex);

		if (mD3DFence->GetCompletedValue() < value)
		{
			HRESULT hr = mD3DFence->SetEventOnCompletion(value, mFenceEvent);
			JGASSERT_IF(SUCCEEDED(hr) , "failed SetEventOnCompletion in Fence Object");
			::WaitForSingleObject(mFenceEvent, static_cast<DWORD>(duration.count()));
		}
	}
}
