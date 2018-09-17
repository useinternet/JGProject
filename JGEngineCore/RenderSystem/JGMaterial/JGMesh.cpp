#include"JGMesh.h"
#include"../JGHLSLShaderDevice/JGBufferManager.h"
#include"../JGHLSLShaderDevice/JGBuffer.h"
#include"../JGDeviceD.h"
using namespace std;
JGMesh::JGMesh()
{
}

JGMesh::~JGMesh()
{
	for (auto& iter : m_vMeshName)
	{
		m_BufferManager->DestroyBuffer(iter + TT("_VertexBuffer"));
		m_BufferManager->DestroyBuffer(iter + TT("_IndexBuffer"));
	}
}

bool JGMesh::Create_Vertex_Index_Buffer(JGBufferManager* BufferManager, 
	const std::wstring& MeshName, EJGUsageType VertexUsageType, EJGCPUType VertexCPUType,
	EJGUsageType IndexUsageType, EJGCPUType IndexCPUType, void* VertexData,
	size_t DataCount, size_t DataTypeSize)
{
	m_BufferManager = BufferManager;
	bool result = true;

	// 버텍스 버퍼 생성
	m_vMeshName.push_back(MeshName);
	result = m_BufferManager->AddBuffer(MeshName + TT("_VertexBuffer"),
		EJGBufferType::VertexBuffer, VertexUsageType, VertexCPUType,
		VertexData, DataCount * DataTypeSize);
	if (!result)
	{
		return false;
	}
	m_VertexBuffer.push_back(m_BufferManager->GetBuffer(MeshName + TT("_VertexBuffer"))->Get());

	
	// 인덱스 버퍼 생성
	vector<UINT> vIndex;
	for (UINT i = 0; i < (UINT)DataCount; ++i)
	{
		vIndex.push_back(i);
	}
	result = m_BufferManager->AddBuffer(MeshName + TT("_IndexBuffer"),
		EJGBufferType::IndexBuffer, IndexUsageType, IndexCPUType,
		&vIndex[0], sizeof(UINT) * vIndex.size());
	if (!result)
	{
		return false;
	}
	m_IndexBuffer.push_back(m_BufferManager->GetBuffer(MeshName + TT("_IndexBuffer"))->Get());


	// 렌더링할 정보를 저장한다.
	m_Stride.push_back((UINT) DataTypeSize );
	m_Offset.push_back(0);
	return true;
}

bool JGMesh::Create_Vertex_Instance_Buffer(JGBufferManager* BufferManager)
{

	bUseInstanceBuffer = true;
	return true;
}

bool JGMesh::MeshWrite(const wstring& MeshName, const EJGMapType type, void* VertexData)
{
	for (auto& iter : m_vMeshName)
	{
		if (iter == MeshName)
		{
			m_BufferManager->Write(MeshName + TT("_VertexBuffer"), type,VertexData);
			return true;
		}
	}
	return false;
}

void JGMesh::Render(JGDeviceD* Device, ERenderingType RenderingType)
{
	Device->GetContext()->IASetVertexBuffers(0, (UINT)m_VertexBuffer.size(),
		&m_VertexBuffer[0], &m_Stride[0], &m_Offset[0]);
	if (m_IndexBuffer.size() != 0)
	{
		Device->GetContext()->IASetIndexBuffer(m_IndexBuffer[0],
			DXGI_FORMAT_R32_UINT, m_Offset[0]);
	}
	switch (RenderingType)
	{
	case ERenderingType::PointList:
		Device->GetContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		break;
	case ERenderingType::LineList:
		Device->GetContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
		break;
	case ERenderingType::LineStrip:
		Device->GetContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
		break;
	case ERenderingType::TriangleList:
		Device->GetContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		break;
	case ERenderingType::TriangleStrip:
		Device->GetContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		break;
	}
}
