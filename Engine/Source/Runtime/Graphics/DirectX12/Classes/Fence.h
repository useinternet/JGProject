#pragma once
#include "Core.h"
#include "DirectX12Helper.h"

class PFence : public IMemoryObject
{
private:
	HDX12ComPtr<HDX12Fence>  _dx12Fence;
	HMutex _eventMutex;
	uint64 _fenceValue;
	void*  _fenceEvent;
public:
	PFence();
	virtual ~PFence() = default;

	uint64 GetValue() const { return _fenceValue; }

	void IncreaseValue() { ++_fenceValue; }
	void WaitForFenceValue(uint64 value, HMilliseconds duration = HMilliseconds::max());

	HDX12Fence* Get() const { return _dx12Fence.Get(); }
};