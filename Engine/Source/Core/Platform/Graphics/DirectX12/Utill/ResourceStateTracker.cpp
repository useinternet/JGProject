#include "pch.h"
#include "ResourceStateTracker.h"
#include "../DirectX12API.h"


namespace JG
{
	static Dictionary<ID3D12Resource*, ResourceInfo> gResourceRefMap;
	static std::shared_mutex gResourceMutex;
	static bool gResourceLock = false;


	void ResourceStateTracker::TransitionBarrier(ComPtr<ID3D12Resource> d3dResource, D3D12_RESOURCE_STATES state, uint32_t subResource)
	{
		JGASSERT_IF(d3dResource != nullptr, "ResourceStateTracker::TransitionBarrier resource is null");

		// 처음 변경하러온 리소스 변경은 보류한다.
		if (mResourceStates.find(d3dResource.Get()) == mResourceStates.end())
		{
			mPendingResourceBarriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(
				d3dResource.Get(), state, state, subResource));
		}
		else
		{
			// SubResource 에따라 상태 변경
			auto resourceState = mResourceStates[d3dResource.Get()];
			if (subResource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES &&
				!resourceState.StateMap.empty())
			{
				for (auto& state_pair : resourceState.StateMap)
				{
					D3D12_RESOURCE_STATES prevState = state_pair.second;
					D3D12_RESOURCE_STATES nextState = state;

					if (prevState != nextState)
					{
						mResourceBarriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(
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
					mResourceBarriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(
						d3dResource.Get(), prevState, nextState, subResource));
				}
			}
		}

		mResourceStates[d3dResource.Get()].Set(subResource, state);
	}

	void ResourceStateTracker::UAVBarrier(ComPtr<ID3D12Resource> d3dResource)
	{
		if (d3dResource == nullptr)
		{
			mResourceBarriers.push_back(
				CD3DX12_RESOURCE_BARRIER::UAV(d3dResource.Get()));

		}
		else
		{
			mResourceBarriers.push_back(
				CD3DX12_RESOURCE_BARRIER::UAV(nullptr));
		}
	}

	void ResourceStateTracker::AliasBarrier(ComPtr<ID3D12Resource> beforeD3DResource, ComPtr<ID3D12Resource> afterD3DResource)
	{
		ID3D12Resource* before_d3d_rsc = (beforeD3DResource != nullptr) ?
			beforeD3DResource.Get() : nullptr;
		ID3D12Resource* after_d3d_rsc = (afterD3DResource != nullptr) ?
			afterD3DResource.Get() : nullptr;


		mResourceBarriers.push_back(
			CD3DX12_RESOURCE_BARRIER::Aliasing(before_d3d_rsc, after_d3d_rsc));
	}

	void ResourceStateTracker::FlushResourceBarrier(ID3D12GraphicsCommandList* cmdList)
	{
		if (mResourceBarriers.empty()) return;


		cmdList->ResourceBarrier((uint32_t)mResourceBarriers.size(), mResourceBarriers.data());
		mResourceBarriers.clear();
	}

	bool ResourceStateTracker::FlushPendingResourceBarrier(ID3D12GraphicsCommandList* cmdList)
	{
		if (mPendingResourceBarriers.empty()) return false;

		List<D3D12_RESOURCE_BARRIER> pendingBarriers;
		for (auto& pending_barrier : mPendingResourceBarriers)
		{
			auto d3d_pResource = pending_barrier.Transition.pResource;
			if (gResourceRefMap.find(d3d_pResource) != gResourceRefMap.end())
			{
				auto resourceState = gResourceRefMap[d3d_pResource].State;
				uint32_t subresource = pending_barrier.Transition.Subresource;
				if (subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES &&
					!resourceState.StateMap.empty())
				{
					for (auto subresourceState : resourceState.StateMap)
					{
						auto prevState = subresourceState.second;
						auto nextState = pending_barrier.Transition.StateAfter;
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
					auto prevState = resourceState.Get(subresource);
					auto nextState = pending_barrier.Transition.StateAfter;
					if (prevState != nextState)
					{
						pending_barrier.Transition.Subresource = subresource;
						pending_barrier.Transition.StateBefore = prevState;
						pending_barrier.Transition.StateAfter  = nextState;
						pendingBarriers.push_back(pending_barrier);
					}
				}
			}
		}
		if (pendingBarriers.empty()) return false;



		cmdList->ResourceBarrier((uint32_t)pendingBarriers.size(), pendingBarriers.data());
		mPendingResourceBarriers.clear();
		return true;
	}

	void ResourceStateTracker::CommitResourceState()
	{
		for (auto& stateMap : mResourceStates)
		{
			gResourceRefMap[stateMap.first].State = stateMap.second;
		}
		mResourceStates.clear();
	}

	void ResourceStateTracker::Reset()
	{
		mResourceBarriers.clear();
		mPendingResourceBarriers.clear();
		mResourceStates.clear();
	}
	void ResourceStateTracker::Lock()
	{
		if (gResourceLock == false)
		{
			gResourceMutex.lock();
			gResourceLock = true;
		}
	}
	void ResourceStateTracker::UnLock()
	{
		if (gResourceLock == true)
		{
			gResourceMutex.unlock();
			gResourceLock = false;
		}
	}
	void ResourceStateTracker::ForEach(const std::function<void(const ID3D12Resource*, const ResourceInfo&)>& action)
	{
		for (auto& _pair : gResourceRefMap)
		{
			action(_pair.first, _pair.second);
		}
	}
	void ResourceStateTracker::RegisterResource(const String& name, ID3D12Resource* d3dResource, D3D12_RESOURCE_STATES initState)
	{
		if (d3dResource == nullptr) return;

		std::lock_guard<std::shared_mutex> lock(gResourceMutex);
		auto iter = gResourceRefMap.find(d3dResource);
		if (iter == gResourceRefMap.end())
		{
			JG_LOG_TRACE("Register {0} in ResourceRefMap", name);
			
			gResourceRefMap.emplace(d3dResource, ResourceInfo(name, initState));
		}
		else
		{
			if ((*iter).second.Name != name)
			{
				JG_LOG_ERROR("Add Counting Resource : {0}   Ref : {1}", gResourceRefMap[d3dResource].Name, gResourceRefMap[d3dResource].RefCount);
			}
			(*iter).second.RefCount += 1;
		
		}
		if (gResourceRefMap[d3dResource].Name == "1_TargetTexture_1" && gResourceRefMap[d3dResource].RefCount == 2)
		{
			int n = 0;
		}
		JG_LOG_TRACE("Add Counting Resource : {0}   Ref : {1}", gResourceRefMap[d3dResource].Name, gResourceRefMap[d3dResource].RefCount);
	}
	void ResourceStateTracker::SetResourceName(ID3D12Resource* d3dResource, const String& name)
	{
		std::lock_guard<std::shared_mutex> lock(gResourceMutex);
		auto iter = gResourceRefMap.find(d3dResource);
		if (iter != gResourceRefMap.end())
		{
			JG_LOG_TRACE("ReName Resource : {0} -> {1}", (*iter).second.Name, name);
			(*iter).second.Name = name;
		}
	}
	bool ResourceStateTracker::GetResourceName(ID3D12Resource* d3dResource, String* out_name)
	{
		if (out_name == nullptr) return false;
		std::lock_guard<std::shared_mutex> lock(gResourceMutex);
		auto iter = gResourceRefMap.find(d3dResource);
		if (iter != gResourceRefMap.end())
		{
			*out_name = (*iter).second.Name;
			return true;
		}
		out_name = nullptr;
		return false;
	}
	void ResourceStateTracker::UnRegisterResource(ID3D12Resource* d3dResource)
	{
		if (d3dResource == nullptr) return;
		std::lock_guard<std::shared_mutex> lock(gResourceMutex);
		auto iter = gResourceRefMap.find(d3dResource);
		if (iter != gResourceRefMap.end())
		{
			(*iter).second.RefCount -= 1;

			JG_LOG_TRACE("Remove Counting Resource : {0}   Ref : {1}", (*iter).second.Name, (*iter).second.RefCount);
			if ((*iter).second.RefCount <= 0)
			{
				JG_LOG_TRACE("UnRegister {0} in ResourceRefMap", (*iter).second.Name);
				gResourceRefMap.erase(iter);
			}
		}
	}
	void ResourceStateTracker::ClearCache()
	{
		gResourceRefMap.clear();
	}
}