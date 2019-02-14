#include"InstanceObject.h"
#include"Data/Mesh/JGSkeletalMesh.h"
#include"Data/Mesh/JGStaticMesh.h"
#include"Data/JGMaterial.h"
#include"DxCore/RootSignatureManager.h"
#include"Data/CommonData.h"
#include"Data/Scene.h"
using namespace JGRC;
using namespace std;
using namespace DirectX;


void InstanceObject::Build(ID3D12GraphicsCommandList* CommandList)
{
	GetState().Init = true;

	EShaderFlag flag = m_Material->GetFlag();
	MaterialDesc* MatDesc = m_Material->GetDesc();

	GetPSOPack().CompilePSO(
		MatDesc->ShaderPath,
		MatDesc->Mode,
		CommonData::_RootSigManager()->GetRootSig(ERootSigType::Common),
		flag);
}
void InstanceObject::Update(const GameTimer& gt, FrameResource* CurrFrameResource)
{
	if (!GetState_c().Active)
		return;

	for (auto& unit : m_Unit)
	{
		UpdateWorldMatrix(unit.ID);
		if (unit.IsCanUpdate())
		{
			InstanceConstantData InsData;
			InsData.MaterialIndex = unit.MatData->CBIndex;

			XMMATRIX World = XMLoadFloat4x4(&unit.World);
			XMMATRIX TexTransform = XMLoadFloat4x4(&unit.TexTransform);
			XMStoreFloat4x4(&InsData.World, XMMatrixTranspose(World));
			XMStoreFloat4x4(&InsData.TexTransform, XMMatrixTranspose(TexTransform));
			// CopyData
			//
			unit.UpdatePerFrame();
		}
	}
}
void InstanceObject::Draw(
	FrameResource* CurrFrameResource,
	ID3D12GraphicsCommandList* CommandList,
	EObjectRenderMode Mode)
{
	if (!GetState_c().Visible)
		return;

	// 

	auto MeshData = GetMesh()->Data();
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
	GetMesh()->ArgDraw(GetMeshName(), CommandList, CurrFrameResource,
		GetUnitCount());
}

UINT InstanceObject::AddInstanceUnit()
{
	Unit unit;
	unit.ID = (UINT)m_Unit.size();
	m_Unit.push_back(unit);
	return unit.ID;
}
void InstanceObject::AttachTo(InstanceObject* obj)
{
	assert(m_Unit.size() <= obj->GetUnitCount());
	if (obj->GetUnitCount() != (UINT)m_Unit.size())
	{
		// warning
	}
	obj->m_Parent = this;
	m_Child.push_back(obj);

	for (UINT i = 0; i < (UINT)m_Unit.size(); ++i)
	{
		obj->m_Unit[i].Parent = &m_Unit[i];
		m_Unit[i].Child.push_back(&obj->m_Unit[i]);
	}
}

void InstanceObject::SetMaterial(JGMaterial* mat)
{
	m_Material = mat;
	ClearNotify();
}
void InstanceObject::SetMaterialData(UINT ID, const std::string& name)
{
	assert(ID < (UINT)m_Unit.size());
	if (m_Material->GetData(name) == nullptr)
	{
		return;
	}
	m_Unit[ID].MatData = m_Material->GetData(name);
	m_Unit[ID].ClearNotify();
}
void InstanceObject::SetMaterialData(UINT Start, UINT End, const std::string& name)
{
	assert(End < (UINT)m_Unit.size());
	if (m_Material->GetData(name) == nullptr)
	{
		return;
	}
	for (UINT i = Start; i < End; ++i)
	{
		m_Unit[i].MatData = m_Material->GetData(name);
		m_Unit[i].ClearNotify();
	}
}
JGMaterial* InstanceObject::GetMaterial()
{
	return m_Material;
}
MatPersonalData* InstanceObject::GetMaterialData(UINT ID)
{
	if (ID >= (UINT)m_Unit.size())
		return nullptr;
	return m_Unit[ID].MatData;
}
InstanceObject::Unit* InstanceObject::GetUnit(UINT ID)
{
	assert(ID < (UINT)m_Unit.size());
	return &m_Unit[ID];
}
const InstanceObject::Unit& InstanceObject::GetUnit_c(UINT ID) const
{
	assert(ID < (UINT)m_Unit.size());
	return m_Unit[ID];
}
UINT  InstanceObject::GetUnitCount()
{
	return (UINT)m_Unit.size();
}
const XMFLOAT3&   InstanceObject::GetLocation(UINT ID) const
{
	assert(ID < (UINT)m_Unit.size());

	return m_Unit[ID].Location;
}
const XMFLOAT3&   InstanceObject::GetRotation(UINT ID) const
{
	assert(ID < (UINT)m_Unit.size());

	return m_Unit[ID].Rotation;
}
const XMFLOAT3&   InstanceObject::GetScale(UINT ID)    const
{
	assert(ID < (UINT)m_Unit.size());

	return m_Unit[ID].Scale;
}
const XMFLOAT4X4& InstanceObject::GetWorld(UINT ID)    const
{
	assert(ID < (UINT)m_Unit.size());

	return m_Unit[ID].World;
}
const XMFLOAT4X4& InstanceObject::GetTex(UINT ID)      const
{
	assert(ID < (UINT)m_Unit.size());

	return m_Unit[ID].TexTransform;
}
void InstanceObject::SetLocation(UINT ID, float x, float y, float z)
{
	assert(ID < (UINT)m_Unit.size());
	if (GetState_c().Init && GetType() == EObjectType::Static)
		return;

	m_Unit[ID].Location = { x,y,z };
	m_Unit[ID].ClearNotify();
}
void InstanceObject::SetRotation(UINT ID, float pitch, float yaw, float roll)
{
	assert(ID < (UINT)m_Unit.size());
	if (GetState_c().Init && GetType() == EObjectType::Static)
		return;

	m_Unit[ID].Rotation = { pitch, yaw, roll };
	m_Unit[ID].ClearNotify();
}
void InstanceObject::SetScale(UINT ID, float x, float y, float z)
{
	assert(ID < (UINT)m_Unit.size());
	if (GetState_c().Init && GetType() == EObjectType::Static)
		return;

	m_Unit[ID].Scale = { x,y,z };
	m_Unit[ID].ClearNotify();
}
void InstanceObject::SetScale(UINT ID, float x)
{
	assert(ID < (UINT)m_Unit.size());
	if (GetState_c().Init && GetType() == EObjectType::Static)
		return;

	m_Unit[ID].Scale = { x,x,x };
	m_Unit[ID].ClearNotify();
}
void InstanceObject::OffsetLocation(UINT ID, float x, float y, float z)
{
	assert(ID < (UINT)m_Unit.size());
	if (GetState_c().Init && GetType() == EObjectType::Static)
		return;
	m_Unit[ID].Location = {
		m_Unit[ID].Location.x + x,
		m_Unit[ID].Location.y + y,
		m_Unit[ID].Location.z + z };
	m_Unit[ID].ClearNotify();
}
void InstanceObject::OffsetRotation(UINT ID, float pitch, float yaw, float roll)
{
	assert(ID < (UINT)m_Unit.size());
	if (GetState_c().Init && GetType() == EObjectType::Static)
		return;

	m_Unit[ID].Rotation = {
		m_Unit[ID].Rotation.x + pitch,
		m_Unit[ID].Rotation.y + yaw,
		m_Unit[ID].Rotation.z + roll };
	m_Unit[ID].ClearNotify();
}
void InstanceObject::OffsetScale(UINT ID, float x, float y, float z)
{
	assert(ID < (UINT)m_Unit.size());
	if (GetState_c().Init && GetType() == EObjectType::Static)
		return;

	m_Unit[ID].Scale = {
		m_Unit[ID].Scale.x + x,
		m_Unit[ID].Scale.y + y,
		m_Unit[ID].Scale.z + z };
	m_Unit[ID].ClearNotify();
}
void InstanceObject::OffsetScale(UINT ID, float x)
{
	assert(ID < (UINT)m_Unit.size());
	if (GetState_c().Init && GetType() == EObjectType::Static)
		return;

	m_Unit[ID].Scale = {
		m_Unit[ID].Scale.x + x,
		m_Unit[ID].Scale.y + x,
		m_Unit[ID].Scale.z + x };
	m_Unit[ID].ClearNotify();
}

void InstanceObject::UpdateWorldMatrix(UINT ID)
{
	XMMATRIX Translation = XMMatrixTranslation(
		m_Unit[ID].Location.x, m_Unit[ID].Location.y, m_Unit[ID].Location.z);
	XMMATRIX Rotation = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(m_Unit[ID].Rotation.x),
		XMConvertToRadians(m_Unit[ID].Rotation.y),
		XMConvertToRadians(m_Unit[ID].Rotation.z));
	XMMATRIX Scale = XMMatrixScaling(
		m_Unit[ID].Scale.x, m_Unit[ID].Scale.y, m_Unit[ID].Scale.z);
	XMMATRIX World;
	(m_Unit[ID].Parent) ?
		World = Scale * Rotation * Translation * XMLoadFloat4x4(&m_Unit[ID].Parent->GetWorld()) :
		World = Scale * Rotation * Translation;

	XMStoreFloat4x4(&m_Unit[ID].World, World);
}