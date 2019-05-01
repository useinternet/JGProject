#include<PCH.h>
#include"ResourceStateMap.h"

using namespace Dx12;
using namespace std;

std::unordered_map<ID3D12Resource*, ResourceStateMap::ResourceState> ResourceStateMap::ms_ResourceStateMap;
std::mutex ResourceStateMap::ms_ResourceStateMapMutex;
void ResourceStateMap::AddResourceState(ID3D12Resource* resource, D3D12_RESOURCE_STATES initState)
{
	lock_guard<mutex> lock(ms_ResourceStateMapMutex);

	if (ms_ResourceStateMap.find(resource) == ms_ResourceStateMap.end())
	{
		ms_ResourceStateMap[resource].State = initState;
	}
	ms_ResourceStateMap[resource].RefCount++;
}
void ResourceStateMap::TransitionResourceState(ID3D12Resource* resource, D3D12_RESOURCE_STATES state)
{
	if (!resource)
	{
		return;
	}
	lock_guard<mutex> lock(ms_ResourceStateMapMutex);
	if (ms_ResourceStateMap.find(resource) == ms_ResourceStateMap.end())
	{
		DX12_LOG_ERROR("this resource is not exsit in ResourceStateMap");
		return;
	}
	ms_ResourceStateMap[resource].State = state;
}
void ResourceStateMap::RemoveResourceState(ID3D12Resource* resource)
{
	lock_guard<mutex> lock(ms_ResourceStateMapMutex);
	if (ms_ResourceStateMap.find(resource) == ms_ResourceStateMap.end())
	{
		return;
	}
	ms_ResourceStateMap[resource].RefCount--;
	if (ms_ResourceStateMap[resource].RefCount <= 0)
	{
		ms_ResourceStateMap.erase(resource);
	}
}

D3D12_RESOURCE_STATES ResourceStateMap::GetResourceState(ID3D12Resource* resource)
{
	if(!resource)
		return D3D12_RESOURCE_STATE_COMMON;
	lock_guard<mutex> lock(ms_ResourceStateMapMutex);

	if (ms_ResourceStateMap.find(resource) != ms_ResourceStateMap.end())
	{
		return ms_ResourceStateMap[resource].State;
	}
	else
	{
		DX12_LOG_ERROR("this resource is not exsit in ResourceStateMap");
		return D3D12_RESOURCE_STATE_COMMON;
	}
}