#pragma once
#include "Core.h"
#include "JGGraphicsDefine.h"
#include "DirectX12Helper.h"


class HResourceState
{
public:
	HMap<uint32, D3D12_RESOURCE_STATES> StateMap;
	D3D12_RESOURCE_STATES State;

public:
	D3D12_RESOURCE_STATES Get(uint32_t subresource) const {
		if (subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES) return State;
		else
		{
			auto iter = StateMap.find(subresource);
			if (iter == StateMap.end())
			{
				return StateMap.at(D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
			}
			return iter->second;
		}

	}
	void Set(uint32_t subresource, D3D12_RESOURCE_STATES state)
	{
		if (subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
		{
			State = state;
			StateMap.clear();
		}
		else
		{
			StateMap[subresource] = state;

		}
	}
};

class HResourceInfo
{
public:
	HResourceState State;
	PName Name;
	uint64 RefCount = 0;
public:
	HResourceInfo() = default;
	HResourceInfo(const PName& name, D3D12_RESOURCE_STATES initState) : Name(name), RefCount(1)
	{
		State.Set(D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, initState);
	}
};

class PResourceStateTracker : public IMemoryObject
{
private:
	HList<D3D12_RESOURCE_BARRIER> _resourceBarriers;
	HList<D3D12_RESOURCE_BARRIER> _pendingResourceBarriers;
	HHashMap<HDX12Resource*, HResourceState> _resourceStates;
public:
	virtual ~PResourceStateTracker() = default;

	void TransitionBarrier(HDX12ComPtr<HDX12Resource> d3dResource, D3D12_RESOURCE_STATES state, uint32_t subResource);

	void UAVBarrier(HDX12ComPtr<HDX12Resource> d3dResource);
	void AliasBarrier(HDX12ComPtr<HDX12Resource> beforeD3DResource, HDX12ComPtr<HDX12Resource> afterD3DResource);

	void FlushResourceBarrier(HDX12CommandList* cmdList);
	bool FlushPendingResourceBarrier(HDX12CommandList* cmdList);
	void CommitResourceState();
	void Reset();

public:
	static void Lock();
	static void UnLock();
	static void RegisterResource(const PName& name, HDX12Resource* d3dResource, D3D12_RESOURCE_STATES initState);
	static void SetResourceName(HDX12Resource* d3dResource, const PName& name);
	static PName GetResourceName(HDX12Resource* d3dResource);
	static void UnRegisterResource(HDX12Resource* d3dResource);
};