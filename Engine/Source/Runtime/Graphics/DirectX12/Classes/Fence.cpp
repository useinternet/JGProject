#include "PCH/PCH.h"
#include "Fence.h"
#include "DirectX12/DirectX12API.h"

PFence::PFence()
{
	_fenceEvent = ::CreateEvent(nullptr, false, false, nullptr);
	JG_ASSERT_IF(_fenceEvent != nullptr, "Failed to create fence event.");
	

	HRESULT hr = HDirectXAPI::GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_dx12Fence));
	JG_ASSERT_IF(SUCCEEDED(hr), "Failed Create Fence Object");
}

void PFence::WaitForFenceValue(uint64 value, HMilliseconds duration)
{
	HLockGuard<HMutex> lock(_eventMutex);

	if (_dx12Fence->GetCompletedValue() < value)
	{
		HRESULT hr = _dx12Fence->SetEventOnCompletion(value, _fenceEvent);
		JG_ASSERT_IF(SUCCEEDED(hr), "failed SetEventOnCompletion in Fence Object");
		::WaitForSingleObject(_fenceEvent, static_cast<DWORD>(duration.count()));
	}
}
