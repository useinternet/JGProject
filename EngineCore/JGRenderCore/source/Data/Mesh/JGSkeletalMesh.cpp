#include"JGSkeletalMesh.h"
#include"Data/CommonData.h"
#include"DxCommon/ModelLoader.h"
#include"Data/Animation/JGAnimation.h"
#include"Shader/CommonShaderRootSignature.h"
#include"Data/Scene.h"
using namespace JGRC;
using namespace std;
using namespace DirectX;
UINT JGSkeletalMesh::SkinnedIndex = 0;
vector<unique_ptr<JGBoneNode>> JGBoneNode::BoneMemNodes;
JGSkeletalMesh::JGSkeletalMesh(const string& name)
{
	m_MeshName = name;
	m_MeshData = make_unique<MeshGeometry>();
	m_MeshType = EMeshType::Static;
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
void JGSkeletalMesh::Update(const GameTimer& gt, FrameResource* CurrFrameResource)
{
	if (m_SkeletalAnim)
	{
		for (auto& bone : m_BoneHierarchy)
		{
			m_AnimtionHelpers[bone.first].UpdateAnimatoin(gt, m_SkeletalAnim, bone.second,
				(UINT)m_BoneDatas[bone.first].size());
			if (m_AnimtionHelpers[bone.first].IsPlay)
				CurrFrameResource->SkinnedCB->CopyData(m_SkinnedCBIndex[bone.first], m_AnimtionHelpers[bone.first].Get());
		}
	}
}
void JGSkeletalMesh::ArgDraw(
	const string& name, 
	ID3D12GraphicsCommandList* CommandList,
	FrameResource* CurrFrameResource,
	UINT Count,
	D3D12_PRIMITIVE_TOPOLOGY TopolgyType)
{
	CommandList->SetComputeRootSignature(CommonData::_Scene()->GetSkinnedRootSig()->GetRootSignature());
	CommandList->IASetVertexBuffers(0, 1, &m_MeshData->VertexBufferView());
	CommandList->IASetIndexBuffer(&m_MeshData->IndexBufferView());
	CommandList->IASetPrimitiveTopology(TopolgyType);
	//
	if (m_AnimtionHelpers[name].IsPlay)
	{
		UINT skbtSize = d3dUtil::CalcConstantBufferByteSize(sizeof(SkinnedData));
		D3D12_GPU_VIRTUAL_ADDRESS Address = CurrFrameResource->SkinnedCB->Resource()->GetGPUVirtualAddress();
		Address += (m_SkinnedCBIndex[name] * skbtSize);

		CommandList->SetGraphicsRootConstantBufferView((UINT)ECommonShaderSlot::cbPerSkinned, Address);
	}

	//
	CommandList->DrawIndexedInstanced(
		m_MeshData->DrawArgs[name].IndexCount,
		Count,
		m_MeshData->DrawArgs[name].StartIndexLocation,
		m_MeshData->DrawArgs[name].BaseVertexLocation,
		0);
	CommandList->SetComputeRootSignature(CommonData::_Scene()->GetRootSig()->GetRootSignature());
}
void JGSkeletalMesh::Draw(
	ID3D12GraphicsCommandList* CommandList,
	FrameResource* CurrFrameResource,
	UINT Count,
	D3D12_PRIMITIVE_TOPOLOGY TopolgyType)
{

	CommandList->SetComputeRootSignature(CommonData::_Scene()->GetSkinnedRootSig()->GetRootSignature());
	CommandList->IASetVertexBuffers(0, 1, &m_MeshData->VertexBufferView());
	CommandList->IASetIndexBuffer(&m_MeshData->IndexBufferView());
	CommandList->IASetPrimitiveTopology(TopolgyType);
	bool test = true;
	for (auto& Arg : m_MeshData->DrawArgs)
	{
		//if (test)
		//{
		//	test = false;
		//	continue;
		//}
		//



		if (m_AnimtionHelpers[Arg.first].IsPlay)
		{
			UINT skbtSize = d3dUtil::CalcConstantBufferByteSize(sizeof(SkinnedData));
			D3D12_GPU_VIRTUAL_ADDRESS Address = CurrFrameResource->SkinnedCB->Resource()->GetGPUVirtualAddress();
			Address += (m_SkinnedCBIndex[Arg.first] * skbtSize);
			CommandList->SetGraphicsRootConstantBufferView((UINT)ECommonShaderSlot::cbPerSkinned, Address);
		}
	
		CommandList->DrawIndexedInstanced(
			Arg.second.IndexCount,
			Count,
			Arg.second.StartIndexLocation,
			Arg.second.BaseVertexLocation,
			0);
	}
	CommandList->SetComputeRootSignature(CommonData::_Scene()->GetRootSig()->GetRootSignature());
}
void JGSkeletalMesh::SetAnimation(const std::string& name)
{
	m_MeshType = EMeshType::Skeletal;
	m_SkeletalAnim = CommonData::_Scene()->GetAnimation(name);
}

void JGSkeletalMesh::AddFbxMeshArg(const string& path)
{
	vector<GeometryGenerator::SkinnedMeshData> MeshArray;
	vector<string> NameArr;
	ModelLoader MeshLoad(path, &MeshArray, &m_BoneDatas, &m_BoneHierarchy, &NameArr);

	for (UINT i = 0; i < (UINT)MeshArray.size(); ++i)
	{
		AddMeshArg(NameArr[i], MeshArray[i].Vertices, MeshArray[i].Indices32);
		m_SkinnedCBIndex[NameArr[i]] = SkinnedIndex++;
		// 업로드 버퍼 생성
		//auto SkinnedCB = make_unique<UploadBuffer<SkinnedData>>(CommonData::_Device(), 1, true);
		//m_SkinnedCB[NameArr[i]] = move(SkinnedCB);

		// 애니메이션 헬퍼 생성
		m_AnimtionHelpers[NameArr[i]] = JGAnimationHelper();
	}
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