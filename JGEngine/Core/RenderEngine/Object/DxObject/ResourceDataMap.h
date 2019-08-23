#pragma once
#include "DxObject.h"
#include "DescriptorHandle.h"
namespace RE
{
	class CommandList;
	class Resource;
	class DescriptorHandle;


	class _ResourceData;
	class ResourceStateMap;

	class ResourceDataMap : DxObject
	{
	public:
		ResourceDataMap();

		void TransitionBarrier(const Resource& resource, D3D12_RESOURCE_STATES after_state, uint32_t subresource);
		void UavBarrier(const Resource& resource);
		void AliasBarrier(const Resource& before_resource, const Resource& after_resource);



		void FlushResourceBarrier(CommandList* commandList);
		uint32_t FlushPendingResourceBarrier(CommandList* pendingCommandList);
		void CommitResourceState();
		void Reset();

		static void StateLock();
		static void StateUnLock();
		static void DataLock();
		static void DataUnLock();

		static void RegisterResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES state);
		static void DeRegisterResource(ID3D12Resource* resource);
		static const _ResourceData* GetData(ID3D12Resource* resource);
	private:
		void ResourceBarrier(const D3D12_RESOURCE_BARRIER& barrier);
		// 
	
		std::vector<D3D12_RESOURCE_BARRIER> m_ResourceBarriers;
		std::vector<D3D12_RESOURCE_BARRIER> m_PendingResourceBarriers;
		std::unordered_map<ID3D12Resource*, ResourceStateMap> m_FinalResourceState;

		static std::unordered_map<ID3D12Resource*, _ResourceData> ms_GlobalResourceDatas;
		static std::mutex ms_GlobalStateMutex;
		static std::mutex ms_GlobalRegisterMutex;
		static std::mutex ms_GlobalDataMutex;
		static bool ms_IsStateLocked;
		static bool ms_IsDataLocked;
	};

	class ResourceStateMap
	{
	public:
		ResourceStateMap(D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON)
		{
			this->state = state;
		}
		D3D12_RESOURCE_STATES  GetResourceState(uint32_t subresource) const
		{
			auto iter = subResourceDataMap.find(subresource);

			if (iter == subResourceDataMap.end())
				return state;
			return iter->second;
		}
		void SetResourceState(uint32_t subresource, D3D12_RESOURCE_STATES state)
		{
			if (subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
			{
				this->state = state;
				subResourceDataMap.clear();
			}
			else
			{
				subResourceDataMap.emplace(subresource, state);
			}

		}
	public:
		D3D12_RESOURCE_STATES state;
		std::map<uint32_t, D3D12_RESOURCE_STATES> subResourceDataMap;
	};
	class _ResourceData
	{
		using DescriptorHandlePool = std::unordered_map<std::size_t, DescriptorHandle>;
	public:
		uint32_t RefCount;
		mutable DescriptorHandlePool SrvDescriptorHandles;
		mutable DescriptorHandlePool RtvDescriptorHandles;
		mutable DescriptorHandlePool DsvDescriptorHandles;
		mutable DescriptorHandlePool UavDescriptorHandles;
		ResourceStateMap ResourceState;
	public:
		_ResourceData() : RefCount(0) {}
	};
	
}