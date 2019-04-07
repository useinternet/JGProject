#include<PCH.h>
#include"IndexBuffer.h"

using namespace Dx12;
using namespace std;

IndexBuffer::IndexBuffer(const string& name) :
	Buffer(name),
	m_NumIndicies(0),
	m_IndexFormat(DXGI_FORMAT_UNKNOWN),
	m_IndexBufferView({})
{

}

IndexBuffer::~IndexBuffer()
{

}

void IndexBuffer::CreateViews(uint32_t numElements, uint32_t elementSize)
{
	if(elementSize != 2 && elementSize != 4)
		DX12_LOG_ERROR("Index's Format must be 16 or 32 bit integer");
	assert(elementSize == 2 || elementSize == 4 && "Index's Format must be 16 or 32 bit integer");


	m_NumIndicies = numElements;
	m_IndexFormat = (elementSize == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

	m_IndexBufferView.BufferLocation = m_D3D_Resource->GetGPUVirtualAddress();
	m_IndexBufferView.SizeInBytes = static_cast<UINT>(numElements * elementSize);
	m_IndexBufferView.Format = m_IndexFormat;
}

D3D12_CPU_DESCRIPTOR_HANDLE IndexBuffer::GetShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc) const
{
	DX12_LOG_ERROR("IndexBuffer::GetShaderResourceView should not be called.  name : %s", m_Name.c_str());

	return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };
}
D3D12_CPU_DESCRIPTOR_HANDLE IndexBuffer::GetUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc) const
{
	DX12_LOG_ERROR("IndexBuffer::GetUnorderedAccessView should not be called.  name : %s", m_Name.c_str());

	return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };
}