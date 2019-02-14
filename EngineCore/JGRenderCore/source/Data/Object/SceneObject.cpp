#include"SceneObject.h"
#include"Data/Mesh/JGSkeletalMesh.h"
#include"Data/Mesh/JGStaticMesh.h"
#include"Data/JGMaterial.h"
#include"Data/Animation/JGAnimation.h"
#include"DxCore/RootSignatureManager.h"
#include"Data/CommonData.h"
#include"Data/Scene.h"
using namespace JGRC;
using namespace std;
using namespace DirectX;

UINT SceneObject::Count = 0;

SceneObject::SceneObject(UINT index, EObjectType type, const string& name)
	: Object(type, name), m_ObjCBIndex(index) {}
void SceneObject::Build(ID3D12GraphicsCommandList* CommandList)
{
	GetState().Init = true;

	EShaderFlag flag      = m_Material->GetFlag();
	MaterialDesc* MatDesc = m_Material->GetDesc();
	if (GetMesh()->Type() == EMeshType::Skeletal && m_Animation)
	{
		SetType(EObjectType::Dynamic);
		flag = flag | Shader_Flag_Skinned;

		GetPSOPack().CompilePSO(
			MatDesc->ShaderPath,
			MatDesc->Mode,
			CommonData::_RootSigManager()->GetRootSig(ERootSigType::CommonSkinned),
			flag);

		m_SkinnedCBIndex = (UINT)SkinnedIndex++;
		m_AnimHelper = make_unique<JGAnimationHelper>();
		m_SkeletalMesh = dynamic_cast<JGSkeletalMesh*>(GetMesh());
		m_AnimHelper->BuildAnimationData(
			m_Animation,
			m_SkeletalMesh->GetBoneHierarchy(GetMeshName()),
			(UINT)m_SkeletalMesh->GetBoneData(GetMeshName()).size(),
			GetMeshName());
	}
	else
	{
		GetPSOPack().CompilePSO(
			MatDesc->ShaderPath,
			MatDesc->Mode,
			CommonData::_RootSigManager()->GetRootSig(ERootSigType::Common),
			flag);
	}
}
void SceneObject::Update(const GameTimer& gt, FrameResource* CurrFrameResource)
{
	if (!GetState_c().Active)
		return;

	auto ObjectCB = CurrFrameResource->ObjectCB.get();
	if (IsCanUpdate())
	{
		ObjectConstantData objConstants;
		/*objConstants.CubeMapIndex = CommonData::_ResourceManager()->GetCubeTextureShaderIndex(
			CommonData::_Scene()->GetMainSkyBox()->GetMaterial()->GetTexturePath(ETextureSlot::Diffuse));*/

		// 월드 매트릭스 업데이트
	    UpdateWorldMatrix();

		XMMATRIX World = XMLoadFloat4x4(&m_World);
		XMMATRIX TexTransform = XMLoadFloat4x4(&m_TexTransform);
		XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(World));
		XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(TexTransform));

		objConstants.MaterialIndex = m_MatPersonalData->CBIndex;
		ObjectCB->CopyData(m_ObjCBIndex, objConstants);
		UpdatePerFrame();
	}
	// 애니메이션
	if (m_Animation)
	{
		m_AnimHelper->UpdateAnimatoin(gt, m_Animation, m_SkeletalMesh->GetBoneHierarchy(GetMeshName()),
			(UINT)m_SkeletalMesh->GetBoneData(GetMeshName()).size());
		if (m_AnimHelper->IsPlay)
			CurrFrameResource->SkinnedCB->CopyData(m_SkinnedCBIndex, m_AnimHelper->Get());
	}
}
void SceneObject::Draw(FrameResource* CurrFrameResource, ID3D12GraphicsCommandList* CommandList, EObjectRenderMode Mode)
{
	if (!GetState_c().Visible)
		return;

	auto ObjCB = CurrFrameResource->ObjectCB->Resource();
	auto MeshData = GetMesh()->Data();
	UINT ObjCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstantData));

	switch (Mode)
	{
	case EObjectRenderMode::Default:
		CommandList->SetPipelineState(GetPSOPack_c().CustomPSO);
		break;
	case EObjectRenderMode::Shadow:
		CommandList->SetPipelineState(GetPSOPack_c().ShadowPSO);
		break;
	case EObjectRenderMode::ViewNormal:
		CommandList->SetPipelineState(GetPSOPack_c().ViewNormalPSO);
		break;
	}
	D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = ObjCB->GetGPUVirtualAddress();
	objCBAddress += (m_ObjCBIndex * ObjCBByteSize);
	CommandList->SetGraphicsRootConstantBufferView((UINT)ECommonShaderSlot::cbPerObject, objCBAddress);

	// 메시가 스켈레톤 메시 이고 애니메이션이 활성화 되어있는경우 셰이더에 본 트랜스폼 데이터 적재
	if (m_Animation && m_AnimHelper->IsPlay)
	{
		UINT skbtSize = d3dUtil::CalcConstantBufferByteSize(sizeof(SkinnedConstantData));
		D3D12_GPU_VIRTUAL_ADDRESS Address = CurrFrameResource->SkinnedCB->Resource()->GetGPUVirtualAddress();
		Address += (m_SkinnedCBIndex * skbtSize);

		CommandList->SetGraphicsRootConstantBufferView((UINT)ECommonShaderSlot::cbPerSkinned, Address);
	}
	GetMesh()->ArgDraw(GetMeshName(), CommandList, CurrFrameResource);
}
void SceneObject::AttachTo(SceneObject* obj)
{
	if (obj == this)
		return;
	obj->m_Parent = this;
	m_Child.push_back(obj);
}
SceneObject* SceneObject::GetChild(UINT idx) const
{
	if (idx >= m_Child.size())
		return nullptr;
	return m_Child[idx];
}
SceneObject* SceneObject::GetParent() const
{
	return m_Parent;
}
JGMaterial* SceneObject::GetMaterial() const
{
	return m_Material;
}
void SceneObject::SetMaterial(JGMaterial* mat, const std::string& matDataName)
{
	m_Material = mat;
	m_MatPersonalData = mat->GetData(matDataName);
	ClearNotify();
}
void SceneObject::SetAnimation(const string& name)
{
	m_Animation = CommonData::_Scene()->GetAnimation(name);
}
const XMFLOAT4X4 SceneObject::GetWorld()    const
{
	if (m_Parent == nullptr)
		return m_World;
	XMFLOAT4X4 result4x4f;
	XMMATRIX me = XMLoadFloat4x4(&m_World);
	XMMATRIX parent = XMLoadFloat4x4(&m_Parent->GetWorld());
	XMMATRIX result = XMMatrixMultiply(me, parent);
	XMStoreFloat4x4(&result4x4f, result);
	return result4x4f;
}
void SceneObject::SetLocation(float x, float y, float z)
{
	if (GetState_c().Init && GetType() == EObjectType::Static)
		return;
	m_Location = { x,y,z };
	ClearNotify();
}
void SceneObject::SetRotation(float pitch, float yaw, float roll)
{
	if (GetState_c().Init && GetType() == EObjectType::Static)
		return;
	m_Rotation = { pitch,yaw,roll };
	ClearNotify();
}
void SceneObject::SetScale(float x, float y, float z)
{
	if (GetState_c().Init && GetType() == EObjectType::Static)
		return;
	m_Scale = { x,y,z };
	ClearNotify();
}

void SceneObject::SetScale(float x)
{
	if (GetState_c().Init && GetType() == EObjectType::Static)
		return;
	m_Scale = { x,x,x };
	ClearNotify();
}

void SceneObject::OffsetLocation(float x, float y, float z)
{
	if (GetState_c().Init && GetType() == EObjectType::Static)
		return;
	m_Location = { m_Location.x + x,m_Location.y + y, m_Location.z + z };
	ClearNotify();
}
void SceneObject::OffsetRotation(float pitch, float yaw, float roll)
{
	if (GetState_c().Init && GetType() == EObjectType::Static)
		return;
	m_Rotation = { m_Rotation.x + pitch , m_Rotation.y + yaw, m_Rotation.z + roll };
	ClearNotify();
}
void SceneObject::OffsetScale(float x, float y, float z)
{
	if (GetState_c().Init && GetType() == EObjectType::Static)
		return;
	m_Scale = { m_Scale.x + x,m_Scale.y + y,m_Scale.z + z };
	ClearNotify();
}
void SceneObject::OffsetScale(float x)
{
	if (GetState_c().Init && GetType() == EObjectType::Static)
		return;
	m_Scale = { m_Scale.x + x,m_Scale.y + x,m_Scale.z + x};
	ClearNotify();
}
void SceneObject::UpdateWorldMatrix()
{
	XMMATRIX Translation = XMMatrixTranslation(m_Location.x, m_Location.y, m_Location.z);
	XMMATRIX Rotation = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(m_Rotation.x),
		XMConvertToRadians(m_Rotation.y),
		XMConvertToRadians(m_Rotation.z));
	XMMATRIX Scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
	XMMATRIX World;
	(m_Parent) ?
		World = Scale * Rotation * Translation * XMLoadFloat4x4(&m_Parent->GetWorld()) :
		World = Scale * Rotation * Translation;

	XMStoreFloat4x4(&m_World, World);
}