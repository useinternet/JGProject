#include"JGStaticMesh.h"
#include"Data/CommonData.h"
#include"ResourceManagement/ResourceReader.h"
using namespace std;
using namespace JGRC;
using namespace DirectX;
unordered_map<string, GeometryGenerator::MeshData> JGStaticMesh::m_StaticMeshs;
JGStaticMesh::JGStaticMesh(const std::string& name)
{
	m_MeshName = name;
	m_MeshData = make_unique<MeshGeometry>();
	m_MeshType = EMeshType::Static;
}
void JGStaticMesh::CreateMesh(ID3D12GraphicsCommandList* CommandList)
{
	UINT vbBtSize = 0;
	UINT ibBtSize = 0;
	UINT vertexSize = 0;
	vertexSize = sizeof(Vertex);
	vbBtSize = (UINT)m_Vertex.size() * vertexSize;
	ibBtSize = (UINT)m_Index.size()  * sizeof(std::uint32_t);


	ThrowIfFailed(D3DCreateBlob(vbBtSize, &m_MeshData->VertexBufferCPU));
	CopyMemory(m_MeshData->VertexBufferCPU->GetBufferPointer(), m_Vertex.data(), vbBtSize);
	ThrowIfFailed(D3DCreateBlob(ibBtSize, &m_MeshData->IndexBufferCPU));
	CopyMemory(m_MeshData->IndexBufferCPU->GetBufferPointer(), m_Index.data(), ibBtSize);


	m_MeshData->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(CommonData::_Device(), CommandList,
		m_Vertex.data(), vbBtSize, m_MeshData->VertexBufferUploader);
	m_MeshData->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(CommonData::_Device(), CommandList,
		m_Index.data(), ibBtSize, m_MeshData->IndexBufferUploader);
	m_MeshData->VertexByteStride = vertexSize;
	m_MeshData->VertexBufferByteSize = vbBtSize;
	m_MeshData->IndexFormat = DXGI_FORMAT_R32_UINT;
	m_MeshData->IndexBufferByteSize = ibBtSize;
}

void JGStaticMesh::AddStaticMeshArg(const string& path)
{
	GeometryGenerator::MeshData mesh;
	if (m_StaticMeshs.find(path) != m_StaticMeshs.end())
	{
		mesh = m_StaticMeshs[path];
		return;
	}

	ResourceReader reader(path, mesh);
	if (!reader.Success)
		return;


	AddMeshArg(path, mesh.Vertices, mesh.Indices32);
}

void JGStaticMesh::AddBoxArg(const string& name, float width, float height, float depth, uint32_t numSubdivision)
{
	GeometryGenerator DefaultGeo;
	GeometryGenerator::MeshData mesh = DefaultGeo.CreateBox(width, height, depth, numSubdivision);
	AddMeshArg(name, mesh.Vertices, mesh.Indices32);
}
void JGStaticMesh::AddSphereArg(const string& name, float radius, UINT32 numSubdivision)
{
	GeometryGenerator DefaultGeo;
	GeometryGenerator::MeshData mesh = DefaultGeo.CreateGeosphere(radius, numSubdivision);
	AddMeshArg(name, mesh.Vertices, mesh.Indices32);
}
void JGStaticMesh::AddSphereArg(const string& name, float radius, UINT32 sliceCount, UINT32 stackCount)
{
	GeometryGenerator DefaultGeo;
	GeometryGenerator::MeshData mesh = DefaultGeo.CreateSphere(radius, sliceCount, stackCount);
	AddMeshArg(name, mesh.Vertices, mesh.Indices32);
}
void JGStaticMesh::AddCylinderArg(const string& name, float bottomRadius, float TopRadius,
	float height, UINT32 sliceCount, UINT32 stackCount)
{
	GeometryGenerator DefaultGeo;
	GeometryGenerator::MeshData mesh = DefaultGeo.CreateCylinder(bottomRadius,
		TopRadius, height, sliceCount, stackCount);
	AddMeshArg(name, mesh.Vertices, mesh.Indices32);
}
void JGStaticMesh::AddGridArg(const string& name, float width, float depth, uint32_t m, uint32_t n)
{
	GeometryGenerator DefaultGeo;
	GeometryGenerator::MeshData mesh = DefaultGeo.CreateGrid(width, depth, m, n);
	AddMeshArg(name, mesh.Vertices, mesh.Indices32);
}
void JGStaticMesh::AddQuadArg(const string& name, float  x, float y, float w, float h, float depth)
{
	GeometryGenerator DefaultGeo;
	GeometryGenerator::MeshData mesh = DefaultGeo.CreateQuad(x, y, w, h, depth);
	AddMeshArg(name, mesh.Vertices, mesh.Indices32);
}

void JGStaticMesh::AddMeshArg(const string& name, const MeshVertex& vertex, const MeshIndex& index)
{
	BoundingBox bounds;
	BoundingBox::CreateFromPoints(bounds, vertex.size(), &vertex[0].Position, sizeof(Vertex));
	m_MeshArgNames.push_back(name);

	m_Vertex.insert(m_Vertex.end(), vertex.begin(), vertex.end());
	m_Index.insert(m_Index.end(), index.begin(), index.end());

	SubmeshGeometry subMesh;
	subMesh.IndexCount = (UINT)index.size();
	subMesh.StartIndexLocation = m_CurrStartIndexLocation;
	subMesh.BaseVertexLocation = m_CurrBaseVertexLocation;
	subMesh.Bounds = bounds;
	m_MeshData->DrawArgs[name] = subMesh;

	m_CurrStartIndexLocation += (UINT)index.size();
	m_CurrBaseVertexLocation += (UINT)vertex.size();
}