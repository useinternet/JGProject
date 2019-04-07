#include<PCH.h>
#include"ByteAddressBuffer.h"
#include"DirectXCore.h"
using namespace Dx12;
using namespace std;


ByteAddressBuffer::ByteAddressBuffer(const string& name) : Buffer(name)
{
	m_SRV = DxDevice::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_UAV = DxDevice::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}
ByteAddressBuffer::ByteAddressBuffer(
	const D3D12_RESOURCE_DESC& desc,
	uint32_t numElements, uint32_t elementSize,
	const string& name) : Buffer(desc, numElements, elementSize, name)
{

}

void ByteAddressBuffer::CreateViews(uint32_t numElements, uint32_t elementSize)
{
	auto device = DxDevice::GetDevice();

	m_BufferSize = Math::AlignUp(numElements * elementSize, 4);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.NumElements = (UINT)m_BufferSize / 4;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;

	device->CreateShaderResourceView(m_D3D_Resource.Get(), &srvDesc, m_SRV.GetDescriptorHandle());




	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	uavDesc.Buffer.NumElements = (UINT)m_BufferSize / 4;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;


	device->CreateUnorderedAccessView(m_D3D_Resource.Get(), nullptr, &uavDesc, m_UAV.GetDescriptorHandle());
}