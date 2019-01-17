#include"JGRCObject.h"
#include"JGMesh.h"
#include"JGMaterial.h"
#include"EngineFrameResource.h"
#include"Scene.h"
#include"PostProcessing/CubeMap.h"
#include"CommonData.h"
#include"Scene.h"
using namespace JGRC;
using namespace std;
using namespace DirectX;
UINT64 JGRCObject::Count = 0;
JGRCObject::JGRCObject(UINT Index, EObjType Type, const string& name, const JGRCObjDesc& Desc)
{
	m_Name += name + to_string(Index);
	Count++;
	this->Type = Type;
	m_ObjCBIndex = Index;
	m_Location = Desc.Location;
	m_Rotation = Desc.Rotation;
	m_Scale = Desc.Scale;
	m_bVisible = Desc.bVisible;
	m_bActive = Desc.bActive;
	m_PrimitiveType = Desc.PrimitiveType;
	m_Mesh = Desc.Mesh;
	m_Material = Desc.Material;
	UpdateWorldMatrix();
}
void JGRCObject::Build(ID3D12GraphicsCommandList* CommandList)
{
	m_bInit = true;
	MaterialDesc* Desc = m_Material->GetDesc();
	if((Desc->bCubeMapStatic || Desc->bCubMapDynamic))
	{
		m_CubeMap = make_unique<CubeMap>();
		m_CubeMap->BuildCubeMap(m_Name, CommandList);
		m_CubeMap->BuildCamera(m_Location.x, m_Location.y, m_Location.z);
	}
}
void JGRCObject::Update(const GameTimer& gt,FrameResource* CurrentFrameResource)
{
	if (!m_bActive)
		return;
	auto ObjCB = CurrentFrameResource->ObjectCB.get();
	if (m_CubeMap)
	{
		m_CubeMap->Update(gt, CurrentFrameResource);
	}
	if (IsCanUpdate())
	{
		cbObjectConstant objConstants;
		if (m_CubeMap) 
		{
			m_CubeMap->BuildCamera(m_Location.x, m_Location.y, m_Location.z);
			objConstants.CubeMapIndex = m_CubeMap->GetCubeMapIndex();
		}
		else
			objConstants.CubeMapIndex = CommonData::_ResourceManager()->GetCubeTextureShaderIndex(
				CommonData::_Scene()->GetMainSkyBox()->GetMaterial()->GetTexturePath(ETextureSlot::Diffuse));
		UpdateWorldMatrix();

		XMMATRIX World = XMLoadFloat4x4(&m_World);
		XMMATRIX TexTransform = XMLoadFloat4x4(&m_TexTransform);


		XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(World));
		XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(TexTransform));
		objConstants.MaterialIndex = m_Material->CBIndex();
		ObjCB->CopyData(m_ObjCBIndex, objConstants);

		UpdatePerFrame();
	}
}
void JGRCObject::Update(const GameTimer& gt, FrameResource* CurrentFrameResource, UploadBuffer<InstanceData>* InsCB)
{
	if (m_CubeMap)
	{
		m_CubeMap->Update(gt, CurrentFrameResource);
	}
	if (IsCanUpdate())
	{
		InstanceData InsConstants;
		if (m_CubeMap)
		{
			m_CubeMap->BuildCamera(m_Location.x, m_Location.y, m_Location.z);
			InsConstants.CubeMapIndex = m_CubeMap->GetCubeMapIndex();
		}
		else
			InsConstants.CubeMapIndex = CommonData::_ResourceManager()->GetCubeTextureShaderIndex(
				CommonData::_Scene()->GetMainSkyBox()->GetMaterial()->GetTexturePath(ETextureSlot::Diffuse));
		UpdateWorldMatrix();

		XMMATRIX World = XMLoadFloat4x4(&m_World);
		XMMATRIX TexTransform = XMLoadFloat4x4(&m_TexTransform);


		XMStoreFloat4x4(&InsConstants.World, XMMatrixTranspose(World));
		XMStoreFloat4x4(&InsConstants.TexTransform, XMMatrixTranspose(TexTransform));
		InsConstants.MaterialIndex = m_Material->CBIndex();

		InsCB->CopyData(m_ObjCBIndex, InsConstants);
		UpdatePerFrame();
	}
}
void JGRCObject::CubeMapDraw(FrameResource* CurrentFrameResource, ID3D12GraphicsCommandList* CommandList)
{
	if (!m_bVisible)
		return;
	MaterialDesc* Desc = m_Material->GetDesc();

	// 큐브맵이 존재하고  동적 큐브맵도 아니고 업데이트가 다끝낫다면 그리지 않는다.
	if (m_CubeMap && !Desc->bCubMapDynamic && !IsCanUpdate())
		return;
	if (m_CubeMap)
	{
		m_CubeMap->Excute(CurrentFrameResource, CommandList);
	}
}
void JGRCObject::Draw(FrameResource* CurrentFrameResource, ID3D12GraphicsCommandList* CommandList, EObjRenderMode Mode)
{
	if (!m_bVisible || Type == EObjType::Instance)
	{
		return;
	}

	auto ObjCB = CurrentFrameResource->ObjectCB->Resource();
	auto MeshData = m_Mesh->Data();
	UINT ObjCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(cbObjectConstant));
	if(Mode != EObjRenderMode::NonePSO)
		CommandList->SetPipelineState(m_Material->GetPSO());

	CommandList->IASetVertexBuffers(0, 1, &MeshData->VertexBufferView());
	CommandList->IASetIndexBuffer(&MeshData->IndexBufferView());
	CommandList->IASetPrimitiveTopology(m_PrimitiveType);
	
	D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = ObjCB->GetGPUVirtualAddress();
	objCBAddress += (m_ObjCBIndex * ObjCBByteSize);
	CommandList->SetGraphicsRootConstantBufferView((UINT)ECommonShaderSlot::cbPerObject, objCBAddress);
	CommandList->DrawIndexedInstanced(
		MeshData->DrawArgs[m_MeshName].IndexCount,
		1,
		MeshData->DrawArgs[m_MeshName].StartIndexLocation,
		MeshData->DrawArgs[m_MeshName].BaseVertexLocation,
		0);
}
void JGRCObject::UpdateWorldMatrix()
{
	XMMATRIX Translation = XMMatrixTranslation(m_Location.x, m_Location.y, m_Location.z);
	XMMATRIX Rotation = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(m_Rotation.x),
		XMConvertToRadians(m_Rotation.y),
		XMConvertToRadians(m_Rotation.z));
	XMMATRIX Scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
	XMMATRIX World = Scale * Rotation * Translation;
	XMStoreFloat4x4(&m_World, World);
}
void JGRCObject::SetMesh(JGMesh* mesh, const string& name)
{
	m_Mesh = mesh;
	m_MeshName = name;
	ClearNotify();
}
void JGRCObject::SetMaterial(JGMaterial* material)
{
	m_Material = material;
	ClearNotify();
}
void JGRCObject::SetLocation(float x, float y, float z)
{
	if (m_bInit && Type == EObjType::Static)
		return;
	m_Location = { x,y,z };
	ClearNotify();
}
void JGRCObject::SetRotation(float pitch, float yaw, float roll)
{
	if (m_bInit && Type == EObjType::Static)
		return;
	m_Rotation = { pitch, yaw, roll };
	ClearNotify();
}
void JGRCObject::SetScale(float x, float y, float z)
{
	if (m_bInit && Type == EObjType::Static)
		return;
	m_Scale = { x,y,z };
	ClearNotify();
}
void JGRCObject::SetScale(float x)
{
	if (m_bInit && Type == EObjType::Static)
		return;
	m_Scale = { x,x,x };
	ClearNotify();
}

void JGRCObject::OffsetLocation(float x, float y, float z)
{
	if (m_bInit && Type == EObjType::Static)
		return;
	m_Location = { m_Location.x + x, m_Location.y + y, m_Location.z + z };
	ClearNotify();
}
void JGRCObject::OffsetRotation(float pitch, float yaw, float roll)
{
	if (m_bInit && Type == EObjType::Static)
		return;
	m_Rotation = { m_Rotation.x + pitch, m_Rotation.y + yaw, m_Rotation.z + roll };
	ClearNotify();
}
void JGRCObject::OffsetScale(float x, float y, float z)
{
	if (m_bInit && Type == EObjType::Static)
		return;
	m_Scale = { m_Scale.x + x, m_Scale.y + y, m_Scale.z + z };
	ClearNotify();
}
void JGRCObject::OffsetScale(float x)
{
	if (m_bInit && Type == EObjType::Static)
		return;
	m_Scale = { m_Scale.x + x, m_Scale.y + x, m_Scale.z + x };
	ClearNotify();
}