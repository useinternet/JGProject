#include"JGSkeletalMesh.h"
#include"Data/CommonData.h"
#include"DxCommon/ModelLoader.h"
#include"Data/Animation/JGAnimation.h"
#include"Shader/CommonShaderRootSignature.h"
#include"Data/Scene.h"
#include"ResourceManagement/ResourceReader.h"
using namespace JGRC;
using namespace std;
using namespace DirectX;
vector<unique_ptr<JGBoneNode>> JGBoneNode::BoneMemNodes;
JGSkeletalMesh::JGSkeletalMesh(const string& name)
{
	m_MeshName = name;
	m_MeshData = make_unique<MeshGeometry>();
	m_MeshType = EMeshType::Skeletal;
}
void JGSkeletalMesh::CreateMesh(ID3D12GraphicsCommandList* CommandList)
{
	UINT vbBtSize = 0;
	UINT ibBtSize = 0;
	UINT vertexSize = 0;
	vertexSize = sizeof(SkeletalVertex);
	vbBtSize = (UINT)m_Vertex.size() * vertexSize;
	ibBtSize = (UINT)m_Index.size() * sizeof(std::uint32_t);


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
JGSkeletalMesh::BoneArray&   JGSkeletalMesh::GetBoneData(const string& name)
{
	return m_BoneDatas[name];
}
JGBoneNode* JGSkeletalMesh::GetBoneHierarchy(const string& name)
{
	return m_BoneHierarchy[name];
}
void JGSkeletalMesh::AddSkeletalMeshArg(const string& path)
{
	GeometryGenerator::SkinnedMeshData Mesh;
	BoneArray   bone;
	JGBoneNode* Root = nullptr;
	ResourceReader reader(path, Mesh, bone, &Root);
	if (!reader.Success)
		return;
	m_BoneDatas[path] = bone;
	m_BoneHierarchy[path] = Root;
	AddMeshArg(path, Mesh.Vertices, Mesh.Indices32);
}
void JGSkeletalMesh::AddMeshArg(const string& name, const SkeletalMeshVertex& vertex, const SkeletalMeshIndex& index)
{
	BoundingBox bounds;
	BoundingBox::CreateFromPoints(bounds, vertex.size(), &vertex[0].Position, sizeof(SkeletalVertex));
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