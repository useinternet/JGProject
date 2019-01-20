#include"InstanceObject.h"
#include"JGMaterial.h"
#include"JGMesh.h"
#include"CommonData.h"
#include"Scene.h"
using namespace JGRC;
using namespace std;
UINT InstanceObject::Count = 0;

InstanceObject::InstanceObject()
{
	m_Name = "InstanceObject" + Count;
}
void InstanceObject::Build(ID3D12GraphicsCommandList* CommandList)
{
	InstanceCB = make_unique<UploadBuffer<InstanceData>>(
		CommonData::_Device(),
		(UINT)m_InsObjects.size(),
		false);
	for (auto& obj : m_InsObjects)
	{
		obj->Build(CommandList);
	}
}
void InstanceObject::Update(const GameTimer& gt, FrameResource* CurrFrameResource)
{
	m_NotCullingObjCount = 0;
	for (UINT i = 0; i < m_InsObjects.size(); ++i)
	{
		if (!CommonData::_Scene()->ObjCulling(m_InsObjects[i].get()))
			m_NotCullingObjCount++;
		m_InsObjects[i]->Update(gt, CurrFrameResource, InstanceCB.get(),i);
	}
}
void InstanceObject::CubeDraw(FrameResource* CurrentFrameResource, ID3D12GraphicsCommandList* CommandList)
{
	for (auto& obj : m_InsObjects)
	{
		obj->CubeMapDraw(CurrentFrameResource, CommandList);
	}
}
void InstanceObject::Draw(FrameResource* CurrentFrameResource, ID3D12GraphicsCommandList* CommandList, EObjRenderMode mode)
{
	auto InsCB = InstanceCB->Resource();
	auto MeshData = m_Mesh->Data();


	if (mode != EObjRenderMode::NonePSO)
		CommandList->SetPipelineState(m_Material->GetPSO());

	CommandList->IASetVertexBuffers(0, 1, &MeshData->VertexBufferView());
	CommandList->IASetIndexBuffer(&MeshData->IndexBufferView());
	CommandList->IASetPrimitiveTopology(m_Desc.PrimitiveType);
	CommandList->SetGraphicsRootShaderResourceView((UINT)ECommonShaderSlot::sbInstanceData,
		InsCB->GetGPUVirtualAddress());
	CommandList->DrawIndexedInstanced(
		MeshData->DrawArgs[m_MeshName].IndexCount,
		m_NotCullingObjCount,
		MeshData->DrawArgs[m_MeshName].StartIndexLocation,
		MeshData->DrawArgs[m_MeshName].BaseVertexLocation,
		0);
}
void InstanceObject::SetMesh(class JGMesh* mesh, const std::string& name)
{
	m_Mesh = mesh; m_MeshName = name;
}
void InstanceObject::SetMaterial(class JGMaterial* mat)
{
	m_Material = mat;
}
JGRCObject* InstanceObject::AddObject()
{
	auto Obj = make_unique<JGRCObject>(m_CBIndex++, EObjType::Instance, m_Name + "_");
	JGRCObject* result = Obj.get();
	m_InsObjects.push_back(move(Obj));


	result->SetMaterial(m_Material);
	result->SetMesh(m_Mesh, m_MeshName);
	return result;
}