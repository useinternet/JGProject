#include<PCH.h>
#include"StructuredBuffer.h"
#include"DirectXCore.h"


using namespace Dx12;
using namespace std;


StructuredBuffer::StructuredBuffer(const string& name) :
	Buffer(name),
	m_CounterBuffer(
		CD3DX12_RESOURCE_DESC::Buffer(4, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
		1, 4, name + " Counter"),
	m_NumElements(0),
	m_ElementSize(0)
{
	m_SRV = DxDevice::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_UAV = DxDevice::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}
StructuredBuffer::StructuredBuffer(
	const D3D12_RESOURCE_DESC& desc, 
	uint32_t numElement, uint32_t elementSize,
	const string& name) :
	Buffer(desc, numElement, elementSize, name),
	m_CounterBuffer(
		CD3DX12_RESOURCE_DESC::Buffer(4, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
		1, 4, name + " Counter"),
	m_NumElements(numElement),
	m_ElementSize(elementSize)
{
	m_SRV = DxDevice::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_UAV = DxDevice::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void StructuredBuffer::CreateViews(uint32_t numElements, uint32_t elementSize)
{
	auto device = DxDevice::GetDevice();
	m_NumElements = numElements;
	m_ElementSize = elementSize;


	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.NumElements = static_cast<UINT>(m_NumElements);
	srvDesc.Buffer.StructureByteStride = static_cast<UINT>(m_ElementSize);
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	device->CreateShaderResourceView(m_D3D_Resource.Get(),
		&srvDesc,
		m_SRV.GetDescriptorHandle());

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.Buffer.CounterOffsetInBytes = 0;
	uavDesc.Buffer.NumElements = static_cast<UINT>(m_NumElements);
	uavDesc.Buffer.StructureByteStride = static_cast<UINT>(m_ElementSize);
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

	device->CreateUnorderedAccessView(m_D3D_Resource.Get(),
		m_CounterBuffer.Get(),
		&uavDesc,
		m_UAV.GetDescriptorHandle());



}