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


UINT SceneObject::Count = 0;
UINT SceneObject::SkinnedIndex = 0;
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
		cbObjectConstant objConstants;
		/*objConstants.CubeMapIndex = CommonData::_ResourceManager()->GetCubeTextureShaderIndex(
			CommonData::_Scene()->GetMainSkyBox()->GetMaterial()->GetTexturePath(ETextureSlot::Diffuse));*/

		// 월드 매트릭스 업데이트
		//  UpdateWorldMatrix();

		//XMMATRIX World = XMLoadFloat4x4(&m_World);
		//XMMATRIX TexTransform = XMLoadFloat4x4(&m_TexTransform);
		//XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(World));
		//XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(TexTransform));

		objConstants.MaterialIndex = m_Material->CBIndex();
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
	UINT ObjCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(cbObjectConstant));

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
		UINT skbtSize = d3dUtil::CalcConstantBufferByteSize(sizeof(SkinnedData));
		D3D12_GPU_VIRTUAL_ADDRESS Address = CurrFrameResource->SkinnedCB->Resource()->GetGPUVirtualAddress();
		Address += (m_SkinnedCBIndex * skbtSize);

		CommandList->SetGraphicsRootConstantBufferView((UINT)ECommonShaderSlot::cbPerSkinned, Address);
	}
	GetMesh()->ArgDraw(GetMeshName(), CommandList, CurrFrameResource);
}