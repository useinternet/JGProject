#include"JGRCObject.h"
#include"Mesh/JGStaticMesh.h"
#include"JGMaterial.h"
#include"EngineFrameResource.h"
#include"Shader/CommonShaderRootSignature.h"
#include"Scene.h"
#include"PostProcessing/CubeMap.h"
#include"CommonData.h"
#include"Animation/JGAnimation.h"
using namespace JGRC;
using namespace std;
using namespace DirectX;
UINT64 JGRCObject::Count        = 0;
JGRCObject::JGRCObject(UINT Index, EObjType Type, const string& name)
{
	m_Name += name + to_string(Index);
	Count++;
	this->Type = Type;
	m_ObjCBIndex = Index;
	UpdateWorldMatrix();
}
void JGRCObject::Build(ID3D12GraphicsCommandList* CommandList, CommonShaderRootSignature* RoogSig)
{	
	m_bInit = true;

	//
	MaterialDesc* Desc = m_Material->GetDesc();
	if((Desc->bCubeMapStatic || Desc->bCubMapDynamic))
	{
		m_CubeMap = make_unique<CubeMap>();
		m_CubeMap->BuildCubeMap(m_Name, CommandList);
		m_CubeMap->BuildCamera(m_Location.x, m_Location.y, m_Location.z);
	}
	m_PSOPack.Macro_Merge(m_Material->GetMacroPack());
	if (m_Mesh->Type() == EMeshType::Skeletal)
	{
		Type = EObjType::Dynamic;
		m_PSOPack.Macro_Push(SHADER_MACRO_DEFINE_SKINNED, SHADER_MACRO_ONLY_DEFINE);
		m_PSOPack.CompilePSO(m_Material->GetDesc()->ShaderPath, m_Material->GetDesc()->Mode, CommonData::_Scene()->GetSkinnedRootSig());
	}
	else
	{
		m_PSOPack.CompilePSO(m_Material->GetDesc()->ShaderPath, m_Material->GetDesc()->Mode, CommonData::_Scene()->GetRootSig());
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
	m_Mesh->Update(gt, CurrentFrameResource, m_MeshName);
}
void JGRCObject::CubeMapDraw(FrameResource* CurrentFrameResource, ID3D12GraphicsCommandList* CommandList)
{
	if (!m_bVisible || m_bCulling)
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
	if (!m_bVisible || Type == EObjType::Instance || m_bCulling)
	{
		return;
	}
	auto ObjCB = CurrentFrameResource->ObjectCB->Resource();
	auto MeshData = m_Mesh->Data();
	UINT ObjCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(cbObjectConstant));

	switch (Mode)
	{
	case EObjRenderMode::Default:
		CommandList->SetPipelineState(m_PSOPack.CustomPSO.Get());
		break;
	case EObjRenderMode::Shadow:
		CommandList->SetPipelineState(m_PSOPack.ShadowPSO.Get());
		break;
	case EObjRenderMode::ViewNormal:
		CommandList->SetPipelineState(m_PSOPack.ViewNormalPSO.Get());
		break;
	}
	D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = ObjCB->GetGPUVirtualAddress();
	objCBAddress += (m_ObjCBIndex * ObjCBByteSize);
	CommandList->SetGraphicsRootConstantBufferView((UINT)ECommonShaderSlot::cbPerObject, objCBAddress);


	m_Mesh->ArgDraw(m_MeshName, CommandList, CurrentFrameResource);
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
void JGRCObject::SetMesh(JGBaseMesh* mesh, const std::string& meshname)
{
	m_Mesh = mesh;
	m_MeshName = meshname;
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