#include"DxHeapManager.h"
#include"DxCore/DxDevice.h"
#include"DxCore/CommandListManager.h"
using namespace JGRC;
using namespace std;
using namespace DirectX;


void DxHeapManager::CreateManager(DxDevice* Device)
{
	m_Device = Device;
}
void DxHeapManager::RegisterHeap(CommandListManager* CmdListManager, UINT idx)
{
	ID3D12DescriptorHeap* Heaps[] = {
		m_CbvHeap.GetHeap(),
		m_SrvHeap.GetHeap(),
		m_UavHeap.GetHeap(),
		m_RtvHeap.GetHeap(),
		m_DsvHeap.GetHeap() };
	CmdListManager->GetCommandList(idx)->SetDescriptorHeaps(
		_countof(Heaps),
		Heaps);
}
void DxHeapManager::ResourceStateTransition(
	CommandListManager* CmdListManager,
	UINT CmdIdx,
	DxResource* resource,
	D3D12_RESOURCE_STATES State)
{
	auto CommandList = CmdListManager->GetCommandList(CmdIdx);
	CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		resource->Resource.Get(), resource->State, State));
	resource->State = State;
}
DxResource* DxHeapManager::BuildResource(
	D3D12_RESOURCE_DESC*  Desc,
	D3D12_CLEAR_VALUE*    ClearValue,
	D3D12_RESOURCE_STATES State,
	D3D12_HEAP_PROPERTIES Property,
	D3D12_HEAP_FLAGS      Flag     )
{
	auto dxResource = make_unique<DxResource>();
	m_Device->Get()->CreateCommittedResource(
		&Property,
		Flag,
		Desc,
		State,
		ClearValue,
		IID_PPV_ARGS(dxResource->Resource.GetAddressOf()));
	dxResource->Desc = *Desc;
	dxResource->ClearValue = *ClearValue;
	dxResource->State = State;
	dxResource->Property = Property;
	dxResource->Flag = Flag;
	DxResource* result = dxResource.get();
	m_ResourceMems.push_back(move(dxResource));
	return result;
}
void DxHeapManager::ReBuildResource(DxResource* Resource, DxResource* CounterResource)
{
	if (Resource->m_IsRegisterHeap)
	{
		EHeapType heapType = Resource->m_RegisteredHeapType;
		if (heapType == EHeapType::SRV && 
			m_SrvDataMems.find(Resource) != m_SrvDataMems.end())
		{
			auto Data = m_SrvDataMems[Resource];
			m_SrvDataMems.erase(Resource);


			Resource->Resource.Reset();
			m_Device->Get()->CreateCommittedResource(
				&Resource->Property,
				Resource->Flag,
				&Resource->Desc,
				Resource->State,
				&Resource->ClearValue,
				IID_PPV_ARGS(Resource->Resource.GetAddressOf()));
			
			Data->Resource = Resource->Resource.Get();
			m_SrvHeap.Modify(Data.get());
			m_SrvDataMems[Resource] = move(Data);
		}
		else if(heapType == EHeapType::UAV &&
			    m_UavDataMems.find(UavID(Resource, CounterResource)) != m_UavDataMems.end())
		{
			auto Data = m_UavDataMems[UavID(Resource, CounterResource)];
			m_UavDataMems.erase(UavID(Resource, CounterResource));


			Resource->Resource.Reset();
			m_Device->Get()->CreateCommittedResource(
				&Resource->Property,
				Resource->Flag,
				&Resource->Desc,
				Resource->State,
				&Resource->ClearValue,
				IID_PPV_ARGS(Resource->Resource.GetAddressOf()));
			Data->Resource = Resource->Resource.Get();
			if (CounterResource)
			{
				CounterResource->Resource.Reset();
				m_Device->Get()->CreateCommittedResource(
					&CounterResource->Property,
					CounterResource->Flag,
					&CounterResource->Desc,
					CounterResource->State,
					&CounterResource->ClearValue,
					IID_PPV_ARGS(CounterResource->Resource.GetAddressOf()));
				Data->CounterResource = CounterResource->Resource.Get();
			}
			m_UavHeap.Modify(Data.get());
			m_UavDataMems[UavID(Resource, CounterResource)] = move(Data);
		}
		else if (heapType == EHeapType::RTV &&
			     m_RtvDataMems.find(Resource) != m_RtvDataMems.end())
		{
			auto Data = m_RtvDataMems[Resource];
			m_RtvDataMems.erase(Resource);

			Resource->Resource.Reset();
			m_Device->Get()->CreateCommittedResource(
				&Resource->Property,
				Resource->Flag,
				&Resource->Desc,
				Resource->State,
				&Resource->ClearValue,
				IID_PPV_ARGS(Resource->Resource.GetAddressOf()));
			Data->Resource = Resource->Resource.Get();

			m_RtvHeap.Modify(Data.get());
			m_RtvDataMems[Resource] = move(Data);
		}
		else if (heapType == EHeapType::DSV &&
			     m_DsvDataMems.find(Resource) != m_DsvDataMems.end())
		{
			auto Data = m_DsvDataMems[Resource];
			m_DsvDataMems.erase(Resource);

			Resource->Resource.Reset();
			m_Device->Get()->CreateCommittedResource(
				&Resource->Property,
				Resource->Flag,
				&Resource->Desc,
				Resource->State,
				&Resource->ClearValue,
				IID_PPV_ARGS(Resource->Resource.GetAddressOf()));
			Data->Resource = Resource->Resource.Get();

			m_DsvHeap.Modify(Data.get());
			m_DsvDataMems[Resource] = move(Data);
		}
	}
	else
	{
		Resource->Resource.Reset();
		m_Device->Get()->CreateCommittedResource(
			&Resource->Property,
			Resource->Flag,
			&Resource->Desc,
			Resource->State,
			&Resource->ClearValue,
			IID_PPV_ARGS(Resource->Resource.GetAddressOf()));
	}
}

CbvData* DxHeapManager::CreateCbvData(const D3D12_CONSTANT_BUFFER_VIEW_DESC& Desc)
{
	auto Data = make_unique<CbvData>();
	Data->Desc = Desc;


	CbvData* result = Data.get();
	m_CbvDataMems.push_back(move(Data));
	m_CbvHeap.Push_Back(result);

	return result;
}
SrvData* DxHeapManager::CreateSrvData(
	const D3D12_SHADER_RESOURCE_VIEW_DESC& Desc, 
	DxResource* Resource)
{
	if (m_SrvDataMems.find(Resource) != m_SrvDataMems.end() ||
		Resource->IsRegisterHeap())
	{
		// error
		return nullptr;
	}
	auto Data = make_unique<SrvData>();
	Data->Desc = Desc;
	Data->Resource = Resource->Resource.Get();

	Resource->m_IsRegisterHeap    = true;

	SrvData* result = Data.get();
	m_SrvDataMems[Resource] = move(Data);
	m_SrvHeap.Push_Back(result);
	return result;
}
UavData* DxHeapManager::CreateUavData(
	const D3D12_UNORDERED_ACCESS_VIEW_DESC& Desc,
	DxResource* Resource,
	DxResource* CounterResource)
{
	if (m_UavDataMems.find(UavID(Resource,CounterResource)) != m_UavDataMems.end() ||
		Resource->IsRegisterHeap() || CounterResource->IsRegisterHeap())
	{
		// error
		return nullptr;
	}
	auto Data = make_unique<UavData>();
	Data->Desc = Desc;
	Data->Resource = Resource->Resource.Get();
	Data->CounterResource = CounterResource->Resource.Get();


	Resource->m_IsRegisterHeap = true;
	CounterResource->m_IsRegisterHeap    = true;

	UavData* result = Data.get();
	m_UavDataMems[UavID(Resource, CounterResource)] = move(Data);
	m_UavHeap.Push_Back(result);

	return result;
}
RtvData* DxHeapManager::CreateRtvData(
	const D3D12_RENDER_TARGET_VIEW_DESC& Desc,
	DxResource* Resource)
{
	if (m_RtvDataMems.find(Resource) != m_RtvDataMems.end() || Resource->IsRegisterHeap())
	{
		// error
		return nullptr;
	}
	auto Data = make_unique<RtvData>();
	Data->Desc = Desc;
	Data->Resource = Resource->Resource.Get();


	Resource->m_IsRegisterHeap = true;

	RtvData* result = Data.get();
	m_RtvDataMems[Resource] = move(Data);
	m_RtvHeap.Push_Back(result);
	return result;
}
DsvData* DxHeapManager::CreateDsvData(
	const D3D12_DEPTH_STENCIL_VIEW_DESC& Desc,
	DxResource* Resource)
{
	if (m_DsvDataMems.find(Resource) != m_DsvDataMems.end() || Resource->IsRegisterHeap())
	{
		// error
		return nullptr;
	}
	auto Data = make_unique<DsvData>();
	Data->Desc = Desc;
	Data->Resource = Resource->Resource.Get();


	Resource->m_IsRegisterHeap = true;

	DsvData* result = Data.get();
	m_DsvDataMems[Resource] = move(Data);
	m_DsvHeap.Push_Back(result);
	return result;
}