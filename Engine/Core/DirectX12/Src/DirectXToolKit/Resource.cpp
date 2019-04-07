#include<PCH.h>
#include"Resource.h"
#include<Util/StringUtil.h>
#include"DirectXCore.h"
#include"CommandList.h"
#include"ResourceStateMap.h"
using namespace Dx12;
using namespace std;


Resource::Resource(const string& name) :
	m_Name(name)
{
	
}

Resource::Resource(
	const D3D12_RESOURCE_DESC& desc,
	const D3D12_CLEAR_VALUE* clearValue,
	const string& name,
	D3D12_RESOURCE_STATES initState,
	D3D12_HEAP_TYPE heapType,
	D3D12_HEAP_FLAGS heapFlag) 
{
	if (clearValue)
	{
		m_ClearValue = make_unique<D3D12_CLEAR_VALUE>(*clearValue);
	}
	auto device = DxDevice::GetDevice();

	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(heapType),
		heapFlag,
		&desc,
		initState,
		m_ClearValue.get(),
		IID_PPV_ARGS(&m_D3D_Resource)));
	SetName(name);

	ResourceStateMap::AddResourceState(m_D3D_Resource.Get(), initState);
}
Resource::Resource(const Resource& copy) :
	m_Name(copy.m_Name)
{
	ResourceStateMap::RemoveResourceState(m_D3D_Resource.Get());

	m_D3D_Resource = copy.m_D3D_Resource;

	ResourceStateMap::AddResourceState(
		m_D3D_Resource.Get(), 
		ResourceStateMap::GetResourceState(m_D3D_Resource.Get()));

	if (copy.m_ClearValue)
	{
		m_ClearValue.reset();
		m_ClearValue = make_unique<D3D12_CLEAR_VALUE>(*copy.m_ClearValue);
	}
}
Resource::Resource(Resource&& rhs) :
	m_Name(move(rhs.m_Name)),
	m_ClearValue(move(rhs.m_ClearValue))
{
	ResourceStateMap::RemoveResourceState(m_D3D_Resource.Get());
	m_D3D_Resource = move(rhs.m_D3D_Resource);
}

Resource& Resource::operator=(const Resource& rhs)
{
	m_Name = rhs.m_Name;

	ResourceStateMap::RemoveResourceState(m_D3D_Resource.Get());

	m_D3D_Resource = rhs.m_D3D_Resource;

	ResourceStateMap::AddResourceState(
		m_D3D_Resource.Get(),
		ResourceStateMap::GetResourceState(m_D3D_Resource.Get()));


	if (rhs.m_ClearValue)
	{
		m_ClearValue = make_unique<D3D12_CLEAR_VALUE>(*rhs.m_ClearValue);
	}

	return *this;
}
Resource& Resource::operator=(Resource&& rhs)
{
	m_Name = move(rhs.m_Name);
	ResourceStateMap::RemoveResourceState(m_D3D_Resource.Get());
	m_D3D_Resource = move(rhs.m_D3D_Resource);
	m_ClearValue = move(rhs.m_ClearValue);
	
	return *this;
}
Resource::~Resource() 
{
	ResourceStateMap::RemoveResourceState(m_D3D_Resource.Get());
}
void Resource::Reset()
{
	ResourceStateMap::RemoveResourceState(m_D3D_Resource.Get());
	m_D3D_Resource.Reset();
	m_ClearValue.reset();
}
void Resource::Set(const Resource& resource)
{
	this->operator=(resource);
}
void Resource::SetName(const std::string& name)
{
	if (m_D3D_Resource)
	{
		m_Name = name;
		m_D3D_Resource->SetName(Common::Util::AnsiToWString(name).c_str());
	}
}

void Resource::SetD3DResource(
	ComPtr<ID3D12Resource> d3dResource,
	D3D12_RESOURCE_STATES initState,
	D3D12_CLEAR_VALUE* clearValue)
{

	ResourceStateMap::RemoveResourceState(m_D3D_Resource.Get());

	m_D3D_Resource = d3dResource;

	ResourceStateMap::AddResourceState(m_D3D_Resource.Get(), initState);

	if (clearValue)
	{
		m_ClearValue = make_unique<D3D12_CLEAR_VALUE>(*clearValue);
	}
	else
	{
		m_ClearValue.reset();
	}
	SetName(m_Name);
}

D3D12_RESOURCE_STATES Resource::GetState() const
{
	return ResourceStateMap::GetResourceState(m_D3D_Resource.Get());
}