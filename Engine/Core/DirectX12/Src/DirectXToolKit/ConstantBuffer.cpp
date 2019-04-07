#include<PCH.h>
#include"ConstantBuffer.h"
#include"DirectXCore.h"

using namespace std;
using namespace Dx12;

ConstantBuffer::ConstantBuffer(const std::string& name) :
	Buffer(name),
	m_SizeInBytes(0)
{
	m_ConstantBufferView = DxDevice::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

ConstantBuffer::~ConstantBuffer()
{

}
void ConstantBuffer::CreateViews(uint32_t numElements, uint32_t elementSize)
{
	m_SizeInBytes = numElements * elementSize;


	D3D12_CONSTANT_BUFFER_VIEW_DESC constantDesc;
	constantDesc.BufferLocation = m_D3D_Resource->GetGPUVirtualAddress();
	constantDesc.SizeInBytes = static_cast<UINT>(Math::AlignUp(m_SizeInBytes, 16));


	auto device = DxDevice::GetDevice();

	device->CreateConstantBufferView(&constantDesc, m_ConstantBufferView.GetDescriptorHandle());
}