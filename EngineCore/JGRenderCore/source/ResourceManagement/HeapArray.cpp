#include"HeapArray.h"
#include"DxCore/DxDevice.h"
using namespace JGRC;
using namespace std;
using namespace DirectX;

template<EHeapType HeapType, typename DataType>
HeapArray<HeapType,DataType>::HeapArray(DxDevice* Device, UINT Size)
{
	m_HeapSize = Size;
	m_Device   = Device;
	ThrowIfFailed(m_Device->Get()->CreateDescriptorHeap(
		&GetDesc(m_HeapSize),
		IID_PPV_ARGS(m_Heap.GetAddressOf())));
}
template<EHeapType HeapType, typename DataType>
void HeapArray<HeapType, DataType>::Push_Back(DataType* Data)
{
	if (m_HeapDataIndex.find(Data) != m_HeapDataIndex.end())
	{
		// Error 로그
		return;
	}
	if (m_HeapOffset >= m_HeapSize)
	{
		ReSize(m_HeapSize * 2);
	}
	// Dead 힙이 존재하면 해당 Offset에 힙 메모리 할당
	UINT HeapOffset = 0;
	if(m_DeadHeapQue.empty())
	{
		HeapOffset = ++m_HeapOffset;
	}
	else
	{
		HeapOffset = m_DeadHeapQue.front();
		m_DeadHeapQue.pop();
	}
	switch (HeapType)
	{
	case EHeapType::CBV:
		m_Device->Get()->CreateConstantBufferView(
			&Data->Desc,
			GetCPUHandle(m_HeapOffset));
		break;
	case EHeapType::SRV:
		m_Device->Get()->CreateShaderResourceView(
			Data->Resource,
			&Data->Desc, 
			GetCPUHandle(m_HeapOffset));
		break;
	case EHeapType::UAV:
		m_Device->Get()->CreateUnorderedAccessView(
			Data->Resource,
			Data->CounterResource,
			&Data->Desc,
			GetCPUHandle(m_HeapOffset));
		break;
	case EHeapType::RTV:
		m_Device->Get()->CreateRenderTargetView(
			Data->Resource,
			&Data->Desc,
			GetCPUHandle(m_HeapOffset));
		break;
	case EHeapType::DSV:
		m_Device->Get()->CreateDepthStencilView(
			Data->Resource,
			&Data->Desc,
			GetCPUHandle(m_HeapOffset));
		break;
	}
	Data->SetID(m_HeapOffset);
	m_HeapDataIndex[Data] = m_HeapOffset;
}
template<EHeapType HeapType, typename DataType>
void HeapArray<HeapType, DataType>::Modify(DataType* Data)
{
	if (m_HeapDataIndex.find(Data) == m_HeapDataIndex.end())
	{
		// Error 검출
		return;
	}
	switch (HeapType)
	{
	case EHeapType::CBV:
		m_Device->Get()->CreateConstantBufferView(
			&Data->Desc,
			GetCPUHandle(Data->ID()));
		break;
	case EHeapType::SRV:
		m_Device->Get()->CreateShaderResourceView(
			Data->Resource,
			&Data->Desc,
			GetCPUHandle(Data->ID()));
		break;
	case EHeapType::UAV:
		m_Device->Get()->CreateUnorderedAccessView(
			Data->Resource,
			Data->CounterResource,
			&Data->Desc,
			GetCPUHandle(Data->ID()));
		break;
	case EHeapType::RTV:
		m_Device->Get()->CreateRenderTargetView(
			Data->Resource,
			&Data->Desc,
			GetCPUHandle(Data->ID()));
		break;
	case EHeapType::DSV:
		m_Device->Get()->CreateDepthStencilView(
			Data->Resource,
			&Data->Desc,
			GetCPUHandle(Data->ID()));
		break;
	}
}
template<EHeapType HeapType, typename DataType>
void HeapArray<HeapType, DataType>::ReSize(UINT Size)
{
	m_Heap.Reset();
	if (m_HeapSize < (UINT64)m_HeapDataIndex.size())
		m_HeapSize = (UINT64)m_HeapDataIndex.size();
	else
		m_HeapSize = Size;


	ThrowIfFailed(m_Device->Get()->CreateDescriptorHeap(
		&GetDesc(m_HeapSize),
		IID_PPV_ARGS(m_Heap.GetAddressOf())));
	for (auto& Data : m_HeapDataIndex)
	{
		Modify(Data.first);
	}
}
template<EHeapType HeapType, typename DataType>
void HeapArray<HeapType, DataType>::Remove(DataType* Data)
{
	if (m_HeapDataIndex.find(Data) == m_HeapDataIndex.end())
	{
		// Error 검출
		return;
	}
	m_HeapDataIndex.erase(Data);
	m_DeadHeapQue.push(Data->ID());
}
template<EHeapType HeapType, typename DataType>
void HeapArray<HeapType, DataType>::Clear()
{
	m_HeapDataIndex.clear();
	while (!m_DeadHeapQue.empty())
		m_DeadHeapQue.pop();
	m_HeapOffset = -1;
	m_Heap.Reset();
	ReSize(100);
}
template<EHeapType HeapType, typename DataType>
bool HeapArray<HeapType, DataType>::Empty()
{
	return m_HeapOffset == -1;
}
template<EHeapType HeapType, typename DataType>
UINT64 HeapArray<HeapType, DataType>::Size()
{
	return m_HeapOffset + 1;
}
template<EHeapType HeapType, typename DataType>
UINT64 HeapArray<HeapType, DataType>::MemSize()
{
	return m_HeapSize;
}
template<EHeapType HeapType, typename DataType>
CD3DX12_GPU_DESCRIPTOR_HANDLE HeapArray<HeapType, DataType>::GPUHandle(DataType * Data)
{
	return GetGPUHandle(Data->ID());
}
template<EHeapType HeapType, typename DataType>
CD3DX12_CPU_DESCRIPTOR_HANDLE HeapArray<HeapType, DataType>::CPUHandle(DataType * Data)
{
	return GetCPUHandle(Data->ID());
}
template<EHeapType HeapType, typename DataType>
D3D12_DESCRIPTOR_HEAP_DESC HeapArray<HeapType, DataType>::GetDesc(UINT Size)
{
	D3D12_DESCRIPTOR_HEAP_DESC Desc = {};
	Desc.NumDescriptors = m_HeapSize;
	Desc.NodeMask = 0;
	switch (HeapType)
	{
	case EHeapType::CBV:
	case EHeapType::SRV:
	case EHeapType::UAV:
		Desc.Type  = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		break;
	case EHeapType::RTV:
		Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		break;
	case EHeapType::DSV:
		Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		break;
	}
	return Desc;
}

template<EHeapType HeapType, typename DataType>
CD3DX12_GPU_DESCRIPTOR_HANDLE HeapArray<HeapType, DataType>::GetGPUHandle(UINT Index)
{
	CD3DX12_GPU_DESCRIPTOR_HANDLE Handle(m_Heap->GetGPUDescriptorHandleForHeapStart());
	switch (HeapType)
	{
	case EHeapType::CBV:
	case EHeapType::SRV:
	case EHeapType::UAV:
		Handle.Offset(Index, m_Device->CbvSrvUavDescriptorSize());
		break;
	case EHeapType::RTV:
		Handle.Offset(Index, m_Device->RtvDescriptorSize());
		break;
	case EHeapType::DSV:
		Handle.Offset(Index, m_Device->DsvDescriptorSize());
		break;
	}
	return Handle;
}

template<EHeapType HeapType, typename DataType>
CD3DX12_CPU_DESCRIPTOR_HANDLE HeapArray<HeapType, DataType>::GetCPUHandle(UINT Index)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE Handle(m_Heap->GetCPUDescriptorHandleForHeapStart());
	switch (HeapType)
	{
	case EHeapType::CBV:
	case EHeapType::SRV:
	case EHeapType::UAV:
		Handle.Offset(Index, m_Device->CbvSrvUavDescriptorSize());
		break;
	case EHeapType::RTV:
		Handle.Offset(Index, m_Device->RtvDescriptorSize());
		break;
	case EHeapType::DSV:
		Handle.Offset(Index, m_Device->DsvDescriptorSize());
		break;
	}
	return Handle;
}
