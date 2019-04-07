#include<PCH.h>
#include"VertexBuffer.h"


using namespace Dx12;
using namespace std;


VertexBuffer::VertexBuffer(const string& name)
	: Buffer(name),
	m_NumVertices(0),
	m_VertexStride(0),
	m_VertexBufferView({})
{}
VertexBuffer::~VertexBuffer()
{}

void VertexBuffer::CreateViews(uint32_t numElements, uint32_t elementSize)
{
	m_NumVertices = numElements;
	m_VertexStride = elementSize;

	m_VertexBufferView.BufferLocation = m_D3D_Resource->GetGPUVirtualAddress();
	m_VertexBufferView.SizeInBytes = static_cast<UINT>(m_NumVertices * m_VertexStride);
	m_VertexBufferView.StrideInBytes = static_cast<UINT>(m_VertexStride);
}

D3D12_CPU_DESCRIPTOR_HANDLE VertexBuffer::GetShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc) const
{
	DX12_LOG_ERROR("VertexBuffer::GetShaderResourceView should not be called. Name : %s ", m_Name.c_str());

	return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };
}

D3D12_CPU_DESCRIPTOR_HANDLE VertexBuffer::GetUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc) const
{
	DX12_LOG_ERROR("VertexBuffer::GetUnorderedAccessView should not be called. Name : %s ", m_Name.c_str());

	return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };
}