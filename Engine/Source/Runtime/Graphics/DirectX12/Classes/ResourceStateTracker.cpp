#include "PCH/PCH.h"
#include "ResourceStateTracker.h"
#include "DirectX12/DirectX12API.h"


static HSharedMutex RESOURCE_MUTEX;
static bool bRESOURCE_LOCK = false;

void PResourceStateTracker::TransitionBarrier(HDX12ComPtr<HDX12Resource> d3dResource, D3D12_RESOURCE_STATES state, uint32 subResource)
{
	JG_ASSERT_IF(d3dResource != nullptr, "ResourceStateTracker::TransitionBarrier resource is null");

	// 처음 변경하러온 리소스 변경은 보류한다.
	if (_resourceStates.find(d3dResource.Get()) == _resourceStates.end())
	{
		_pendingResourceBarriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(
			d3dResource.Get(), state, state, subResource));
	}
	else
	{
		// SubResource 에따라 상태 변경
		const HResourceState& resourceState = _resourceStates[d3dResource.Get()];
		if (subResource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES &&
			!resourceState.StateMap.empty())
		{
			for (const HPair<const uint32, D3D12_RESOURCE_STATES>& state_pair : resourceState.StateMap)
			{
				D3D12_RESOURCE_STATES prevState = state_pair.second;
				D3D12_RESOURCE_STATES nextState = state;

				if (prevState != nextState)
				{
					_resourceBarriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(
						d3dResource.Get(), prevState, nextState, subResource));
				}
			}
		}
		else
		{
			D3D12_RESOURCE_STATES prevState = resourceState.Get(subResource);
			D3D12_RESOURCE_STATES nextState = state;
			if (prevState != nextState)
			{
				_resourceBarriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(
					d3dResource.Get(), prevState, nextState, subResource));
			}
		}
	}

	_resourceStates[d3dResource.Get()].Set(subResource, state);
}

void PResourceStateTracker::UAVBarrier(HDX12ComPtr<HDX12Resource> d3dResource)
{
	if (d3dResource == nullptr)
	{
		_resourceBarriers.push_back(
			CD3DX12_RESOURCE_BARRIER::UAV(d3dResource.Get()));

	}
	else
	{
		_resourceBarriers.push_back(
			CD3DX12_RESOURCE_BARRIER::UAV(nullptr));
	}
}

void PResourceStateTracker::AliasBarrier(HDX12ComPtr<HDX12Resource> beforeD3DResource, HDX12ComPtr<HDX12Resource> afterD3DResource)
{
	ID3D12Resource* before_d3d_rsc = (beforeD3DResource != nullptr) ?
		beforeD3DResource.Get() : nullptr;
	ID3D12Resource* after_d3d_rsc = (afterD3DResource != nullptr) ?
		afterD3DResource.Get() : nullptr;


	_resourceBarriers.push_back(
		CD3DX12_RESOURCE_BARRIER::Aliasing(before_d3d_rsc, after_d3d_rsc));
}

void PResourceStateTracker::FlushResourceBarrier(HDX12CommandList* cmdList)
{
	if (_resourceBarriers.empty()) return;


	cmdList->ResourceBarrier((uint32_t)_resourceBarriers.size(), _resourceBarriers.data());
	_resourceBarriers.clear();
}

bool PResourceStateTracker::FlushPendingResourceBarrier(HDX12CommandList* cmdList)
{
	if (_pendingResourceBarriers.empty()) return false;

	HList<D3D12_RESOURCE_BARRIER> pendingBarriers;
	for (D3D12_RESOURCE_BARRIER& pending_barrier : _pendingResourceBarriers)
	{
		HHashMap<HDX12Resource*, HResourceInfo>& _resourceRefMap = HDirectXAPI::GetResourceRefCacheRef();

		HDX12Resource* d3d_pResource = pending_barrier.Transition.pResource;
		if (_resourceRefMap.find(d3d_pResource) != _resourceRefMap.end())
		{
			const HResourceState& resourceState = _resourceRefMap[d3d_pResource].State;
			uint32_t subresource = pending_barrier.Transition.Subresource;
			if (subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES &&
				!resourceState.StateMap.empty())
			{
				for (auto subresourceState : resourceState.StateMap)
				{
					D3D12_RESOURCE_STATES prevState = subresourceState.second;
					D3D12_RESOURCE_STATES nextState = pending_barrier.Transition.StateAfter;
					if (prevState != nextState)
					{
						pending_barrier.Transition.Subresource = subresourceState.first;
						pending_barrier.Transition.StateBefore = prevState;
						pending_barrier.Transition.StateAfter = nextState;
						pendingBarriers.push_back(pending_barrier);
					}
				}
			}
			else
			{
				D3D12_RESOURCE_STATES prevState = resourceState.Get(subresource);
				D3D12_RESOURCE_STATES nextState = pending_barrier.Transition.StateAfter;
				if (prevState != nextState)
				{
					pending_barrier.Transition.Subresource = subresource;
					pending_barrier.Transition.StateBefore = prevState;
					pending_barrier.Transition.StateAfter = nextState;
					pendingBarriers.push_back(pending_barrier);
				}
			}
		}
	}
	if (pendingBarriers.empty()) return false;



	cmdList->ResourceBarrier((uint32_t)pendingBarriers.size(), pendingBarriers.data());
	_pendingResourceBarriers.clear();
	return true;
}

void PResourceStateTracker::CommitResourceState()
{
	HHashMap<HDX12Resource*, HResourceInfo>& resourceRefMap = HDirectXAPI::GetResourceRefCacheRef();

	for (HPair<HDX12Resource* const, HResourceState>& stateMap : _resourceStates)
	{
		resourceRefMap[stateMap.first].State = stateMap.second;
	}

	resourceRefMap.clear();
}

void PResourceStateTracker::Reset()
{
	_resourceBarriers.clear();
	_pendingResourceBarriers.clear();
	_resourceStates.clear();
}

void PResourceStateTracker::Lock()
{
	if (bRESOURCE_LOCK == false)
	{
		RESOURCE_MUTEX.lock();
		bRESOURCE_LOCK = true;
	}
}
void PResourceStateTracker::UnLock()
{
	if (bRESOURCE_LOCK == true)
	{
		RESOURCE_MUTEX.unlock();
		bRESOURCE_LOCK = false;
	}
}

void PResourceStateTracker::RegisterResource(const PName& name, HDX12Resource* d3dResource, D3D12_RESOURCE_STATES initState)
{
	if (d3dResource == nullptr) return;

	HLockGuard<HSharedMutex> lock(RESOURCE_MUTEX);

	HHashMap<HDX12Resource*, HResourceInfo>& resourceRefMap = HDirectXAPI::GetResourceRefCacheRef();

	auto iter = resourceRefMap.find(d3dResource);
	if (iter == resourceRefMap.end())
	{
		JG_LOG(Graphics, ELogLevel::Trace, "Register %s in ResourceRefMap", name);

		resourceRefMap.emplace(d3dResource, HResourceInfo(name, initState));
	}
	else
	{
		if ((*iter).second.Name != name)
		{
			JG_LOG(Graphics, ELogLevel::Error, "Add Counting Resource : {0}   Ref : {1}", resourceRefMap[d3dResource].Name, resourceRefMap[d3dResource].RefCount);
		}
		(*iter).second.RefCount += 1;

	}

	JG_LOG(Graphics, ELogLevel::Trace, "Add Counting Resource : {0}   Ref : {1}", resourceRefMap[d3dResource].Name, resourceRefMap[d3dResource].RefCount);
}
void PResourceStateTracker::SetResourceName(ID3D12Resource* d3dResource, const PName& name)
{
	HLockGuard<HSharedMutex>  lock(RESOURCE_MUTEX);

	HHashMap<HDX12Resource*, HResourceInfo>& resourceRefMap = HDirectXAPI::GetResourceRefCacheRef();

	auto iter = resourceRefMap.find(d3dResource);
	if (iter != resourceRefMap.end())
	{
		JG_LOG(Graphics, ELogLevel::Trace, "ReName Resource : {0} -> {1}", (*iter).second.Name, name);
		(*iter).second.Name = name;
	}
}

PName PResourceStateTracker::GetResourceName(HDX12Resource* d3dResource)
{
	HLockGuard<HSharedMutex> lock(RESOURCE_MUTEX);

	HHashMap<HDX12Resource*, HResourceInfo>& resourceRefMap = HDirectXAPI::GetResourceRefCacheRef();

	auto iter = resourceRefMap.find(d3dResource);
	if (iter != resourceRefMap.end())
	{
		return (*iter).second.Name;
	}

	return NAME_NONE;
}

void PResourceStateTracker::UnRegisterResource(HDX12Resource* d3dResource)
{
	if (d3dResource == nullptr) return;

	HLockGuard<HSharedMutex> lock(RESOURCE_MUTEX);
	HHashMap<HDX12Resource*, HResourceInfo>& resourceRefMap = HDirectXAPI::GetResourceRefCacheRef();

	auto iter = resourceRefMap.find(d3dResource);
	if (iter != resourceRefMap.end())
	{
		(*iter).second.RefCount -= 1;

		JG_LOG(Graphics, ELogLevel::Trace, "Remove Counting Resource : {0}   Ref : {1}", (*iter).second.Name, (*iter).second.RefCount);
		if ((*iter).second.RefCount <= 0)
		{
			JG_LOG(Graphics, ELogLevel::Trace, "UnRegister {0} in ResourceRefMap", (*iter).second.Name);
			resourceRefMap.erase(iter);
		}
	}
}