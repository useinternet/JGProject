#pragma once
#include"DirectXCommon/DirectXCommon.h"
#include<atomic>
namespace Dx12
{
	class Resource;
	class ResourceStateMap
	{
		struct ResourceState
		{
			D3D12_RESOURCE_STATES State;
			uint32_t  RefCount;

			ResourceState() : State(D3D12_RESOURCE_STATE_COMMON), RefCount(0) {}
		};
	public:
		static std::unordered_map<ID3D12Resource*, ResourceState> ms_ResourceStateMap;
		static std::mutex ms_ResourceStateMapMutex;

		static void AddResourceState(ID3D12Resource* resource, D3D12_RESOURCE_STATES initState);
		static void TransitionResourceState(ID3D12Resource* resource, D3D12_RESOURCE_STATES state);
		static void RemoveResourceState(ID3D12Resource* resource);
		static D3D12_RESOURCE_STATES GetResourceState(ID3D12Resource* resource);
	};
}