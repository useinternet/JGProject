#include"SceneObject.h"
#include"Data/Mesh/JGSkeletalMesh.h"
#include"Data/Mesh/JGStaticMesh.h"
#include"ResourceManagement/DataManager.h"
#include"Data/JGMaterial.h"
#include"Data/Animation/JGAnimation.h"
#include"DxCore/RootSignatureManager.h"
#include"Data/CommonData.h"
#include"Data/Scene.h"
using namespace JGRC;
using namespace std;
using namespace DirectX;
using namespace JGLibrary;
UINT SceneObject::Count = 0;

SceneObject::SceneObject(EObjectType type, const string& name)
	: Object(type, name) 
{
	m_Data = CommonData::_DataManager()->AddObjectData();
}
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
		m_SkinnedData = CommonData::_DataManager()->AddSkinnedData();
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
	if (m_Parent && m_Parent->IsCanUpdate() && !IsCanUpdate())
	{
		ClearNotify();
	}
	auto ObjectCB = CurrFrameResource->ObjectCB.get();
	if (IsCanUpdate())
	{
		//ObjectConstantData objConstants;
		/*objConstants.CubeMapIndex = CommonData::_ResourceManager()->GetCubeTextureShaderIndex(
			CommonData::_Scene()->GetMainSkyBox()->GetMaterial()->GetTexturePath(ETextureSlot::Diffuse));*/

		// 월드 매트릭스 업데이트
		m_WorldMatrix.Identity();
		m_WorldMatrix = GetTransform();
		m_WorldMatrix.Transpose();


		XMStoreFloat4x4(&m_Data->Get().World, m_WorldMatrix.GetSIMD());
		XMStoreFloat4x4(&m_Data->Get().TexTransform, XMMatrixTranspose(m_TexMatrix.GetSIMD()));

		m_Data->Get().MaterialIndex = m_MatPersonalData->Data->Index();
		ObjectCB->CopyData(m_Data->Index(), m_Data->Get());
		UpdatePerFrame();
	}
	// 애니메이션
	if (m_Animation)
	{
		m_AnimHelper->UpdateAnimatoin(gt, m_Animation, m_SkeletalMesh->GetBoneHierarchy(GetMeshName()),
			(UINT)m_SkeletalMesh->GetBoneData(GetMeshName()).size());
		if (m_AnimHelper->IsPlay)
		{
			m_SkinnedData->Get() = m_AnimHelper->Get();
			CurrFrameResource->SkinnedCB->CopyData(m_SkinnedData->Index(), m_SkinnedData->Get());
		}
			
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
	CommandList->SetGraphicsRootConstantBufferView(
		(UINT)ECommonShaderSlot::cbPerObject,
		CurrFrameResource->ObjectCBHeapAddress(m_Data));

	// 메시가 스켈레톤 메시 이고 애니메이션이 활성화 되어있는경우 셰이더에 본 트랜스폼 데이터 적재
	if (m_Animation && m_AnimHelper->IsPlay)
	{
		CommandList->SetGraphicsRootConstantBufferView(
			(UINT)ECommonShaderSlot::cbPerSkinned, 
			CurrFrameResource->SkinnedCBHeapAddress(m_SkinnedData));
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
Transform SceneObject::GetTransform()
{
	if (m_Parent)
	{
		Transform result;
		result = m_CoordinateSystem.GetTransform() * m_Parent->GetTransform();
		return result;
	}
	else
	{
		return m_CoordinateSystem.GetTransform();
	}
}
Transform SceneObject::GetTexTransform()
{
	return m_TexMatrix;
}