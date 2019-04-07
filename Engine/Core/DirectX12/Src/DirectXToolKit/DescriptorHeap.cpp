#include<PCH.h>
#include"DescriptorHeap.h"
#include"DirectXCore.h"
using namespace Dx12;


DescriptorHeap::DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptor, D3D12_DESCRIPTOR_HEAP_FLAGS flag) :
	m_D3D_Type(type), m_D3D_Flag(flag)
{
	m_IncrementDescriptorHeapSize = DxDevice::GetDescriptorHandleIncrementSize(m_D3D_Type);

	auto device = DxDevice::GetDevice();
	D3D12_DESCRIPTOR_HEAP_DESC Desc;
	Desc.Flags = m_D3D_Flag;
	Desc.NodeMask = 0;
	Desc.NumDescriptors = numDescriptor;
	Desc.Type = m_D3D_Type;

	ThrowIfFailed(device->CreateDescriptorHeap(&Desc,
		IID_PPV_ARGS(&m_D3D_DescriptorHeap)));

}
void DescriptorHeap::Resize(uint32_t numDescriptor)
{
	m_D3D_DescriptorHeap.Reset();

	auto device = DxDevice::GetDevice();
	D3D12_DESCRIPTOR_HEAP_DESC Desc;
	Desc.Flags = m_D3D_Flag;
	Desc.NodeMask = 0;
	Desc.NumDescriptors = numDescriptor;
	Desc.Type = m_D3D_Type;

	ThrowIfFailed(device->CreateDescriptorHeap(&Desc,
		IID_PPV_ARGS(&m_D3D_DescriptorHeap)));
}