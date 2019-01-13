#include"JGMesh.h"

using namespace std;
using namespace JGRC;
using namespace DirectX;
JGMesh::JGMesh(const std::string& name)
{
	m_Mesh = make_unique<MeshGeometry>();
	m_Mesh->Name = name;
}
void JGMesh::AddMeshArg(const string& name, const MeshVertex& vertex, const MeshIndex& index)
{
	BoundingBox bounds;
	BoundingBox::CreateFromPoints(bounds, vertex.size(), &vertex[0].Position, sizeof(Vertex));

	m_Vertex.insert(m_Vertex.end(), vertex.begin(), vertex.end());
	m_Index.insert(m_Index.end(), index.begin(), index.end());

	SubmeshGeometry subMesh;
	subMesh.IndexCount = (UINT)index.size();
	subMesh.StartIndexLocation = m_CurrStartIndexLocation;
	subMesh.BaseVertexLocation = m_CurrBaseVertexLocation;
	subMesh.Bounds = bounds;
	m_Mesh->DrawArgs[name] = subMesh;

	m_CurrStartIndexLocation += (UINT)index.size();
	m_CurrBaseVertexLocation += (UINT)vertex.size();
}
void JGMesh::AddBoxArg(const string& name, float width, float height, float depth, uint32_t numSubdivision)
{
	GeometryGenerator DefaultGeo;
	GeometryGenerator::MeshData mesh = DefaultGeo.CreateBox(width, height, depth, numSubdivision);
	AddMeshArg(name, mesh.Vertices, mesh.GetIndices16());
}
void JGMesh::AddSphereArg(const string& name, float radius, UINT32 numSubdivision)
{
	GeometryGenerator DefaultGeo;
	GeometryGenerator::MeshData mesh = DefaultGeo.CreateGeosphere(radius, numSubdivision);
	AddMeshArg(name, mesh.Vertices, mesh.GetIndices16());
}
void JGMesh::AddSphereArg(const std::string& name, float radius, UINT32 sliceCount, UINT32 stackCount)
{
	GeometryGenerator DefaultGeo;
	GeometryGenerator::MeshData mesh = DefaultGeo.CreateSphere(radius, sliceCount, stackCount);
	AddMeshArg(name, mesh.Vertices, mesh.GetIndices16());
}
void JGMesh::AddCylinderArg(const string& name, float bottomRadius, float TopRadius,
	float height, UINT32 sliceCount, UINT32 stackCount)
{
	GeometryGenerator DefaultGeo;
	GeometryGenerator::MeshData mesh = DefaultGeo.CreateCylinder(bottomRadius,
		TopRadius,height,sliceCount,stackCount);
	AddMeshArg(name, mesh.Vertices, mesh.GetIndices16());
}
void JGMesh::AddGridArg(const std::string& name, float width, float depth, uint32_t m, uint32_t n)
{
	GeometryGenerator DefaultGeo;
	GeometryGenerator::MeshData mesh = DefaultGeo.CreateGrid(width, depth, m, n);
	AddMeshArg(name, mesh.Vertices, mesh.GetIndices16());
}
void JGMesh::AddQuadArg(const string& name, float  x, float y, float w, float h, float depth)
{
	GeometryGenerator DefaultGeo;
	GeometryGenerator::MeshData mesh = DefaultGeo.CreateQuad(x, y, w, h, depth);
	AddMeshArg(name, mesh.Vertices, mesh.GetIndices16());
}
void JGMesh::CreateMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList)
{
	UINT vbBtSize = (UINT)m_Vertex.size() * sizeof(Vertex);
	UINT ibBtSize = (UINT)m_Index.size() * sizeof(std::uint16_t);

	ThrowIfFailed(D3DCreateBlob(vbBtSize, &m_Mesh->VertexBufferCPU));
	CopyMemory(m_Mesh->VertexBufferCPU->GetBufferPointer(), m_Vertex.data(), vbBtSize);

	ThrowIfFailed(D3DCreateBlob(ibBtSize, &m_Mesh->IndexBufferCPU));
	CopyMemory(m_Mesh->IndexBufferCPU->GetBufferPointer(), m_Index.data(), ibBtSize);

	m_Mesh->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(Device, CommandList,
		m_Vertex.data(), vbBtSize, m_Mesh->VertexBufferUploader);
	m_Mesh->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(Device, CommandList,
		m_Index.data(), ibBtSize, m_Mesh->IndexBufferUploader);

	m_Mesh->VertexByteStride = sizeof(Vertex);
	m_Mesh->VertexBufferByteSize = vbBtSize;
	m_Mesh->IndexFormat = DXGI_FORMAT_R16_UINT;
	m_Mesh->IndexBufferByteSize = ibBtSize;
}
const SubmeshGeometry& JGMesh::GetMeshArg_c(const string& name) const
{
	if (m_Mesh->DrawArgs.end() == m_Mesh->DrawArgs.find(name))
	{
		// 抗寇 贸府
		return m_DumpSubMesh;
	}
	return m_Mesh->DrawArgs[name];
}
SubmeshGeometry& JGMesh::GetMeshArg(const string& name)
{
	if (m_Mesh->DrawArgs.end() == m_Mesh->DrawArgs.find(name))
	{
		// 抗寇 贸府
		return m_DumpSubMesh;
	}
	return m_Mesh->DrawArgs[name];
}