#include"DataManager.h"
#include"Data/Object/SceneObject.h"
#include"Data/Object/InstanceObject.h"
#include"Data/Mesh/JGSkeletalMesh.h"
#include"Data/Mesh/JGStaticMesh.h"
#include"Data/JGMaterial.h"
#include"DxCore/CommandListManager.h"
#include"Data/CommonData.h"
using namespace JGRC;
using namespace std;


SceneObject*    DataManager::CreateSceneObject(
	const std::string& name,
	JGMaterial* mat,
	const string& matDataName,
	JGBaseMesh* mesh,
	const string& meshName,
	EObjectType type)
{
	auto Obj = make_unique<SceneObject>(type, name);
	Obj->SetMaterial(mat, matDataName);
	Obj->SetMesh(mesh, meshName);
	

	SceneObject* result = Obj.get();
	m_ObjectMems.push_back(move(Obj));
	return result;
}
JGMaterial*     DataManager::CreateMaterial(const MaterialDesc& desc)
{
	auto mat = make_unique<JGMaterial>(desc);
	JGMaterial* result = mat.get();
	m_MaterialMems.push_back(move(mat));
	return result;
}
JGStaticMesh*   DataManager::CreateStaticMesh(const std::string& name)
{
	auto mesh = make_unique<JGStaticMesh>(name);
	JGStaticMesh* result = mesh.get();

	m_MeshMems.push_back(move(mesh));
	return result;
}
JGSkeletalMesh* DataManager::CreateSkeletalMesh(const std::string& name)
{
	auto mesh = make_unique<JGSkeletalMesh>(name);
	JGSkeletalMesh* result = mesh.get();

	m_MeshMems.push_back(move(mesh));
	return result;
}
void DataManager::Build()
{
	auto CommandList = CommonData::_CmdListManager()->GetCommandList(0);
	for (auto& obj : m_ObjectMems)
	{
		obj->Build(CommandList);
	}
	for (auto& mesh : m_MeshMems)
	{
		mesh->CreateMesh(CommandList);
	}
}
ObjectData*   DataManager::AddObjectData()
{
	auto Data = make_unique<ObjectData>(m_ObjectCBIndex++);
	ObjectData* result = Data.get();
	m_ObjectDataMems.push_back(move(Data));
	return result;
}
PassData*     DataManager::AddPassData()
{
	auto Data = make_unique<PassData>(m_PassDataCBIndex++);
	PassData* result = Data.get();
	m_PassDataMems.push_back(move(Data));
	return result;
}
SkinnedData*  DataManager::AddSkinnedData()
{
	auto Data = make_unique<SkinnedData>(m_SkinnedCBIndex++);
	SkinnedData* result = Data.get();
	m_SkinnedDataMems.push_back(move(Data));
	return result;
}
InstanceData* DataManager::AddInstanceData()
{
	auto Data = make_unique<InstanceData>(m_InstanceCBIndex++);
	InstanceData* result = Data.get();
	m_InstanceDataMems.push_back(move(Data));
	return result;
}
MaterialData* DataManager::AddMaterialData()
{
	auto Data = make_unique<MaterialData>(m_MaterialCBIndex++);
	MaterialData* result = Data.get();
	m_MaterialDataMems.push_back(move(Data));
	return result;
}
UINT DataManager::ObjectCount()
{
	return (UINT)m_ObjectDataMems.size();
}
UINT DataManager::MaterialDataCount()
{
	return (UINT)m_MaterialDataMems.size();
}
UINT DataManager::PassDataCount()
{
	return (UINT)m_PassDataMems.size();
}
UINT DataManager::SkinnedDataCount()
{
	return (UINT)m_SkinnedDataMems.size();
}
UINT DataManager::InstanceDataCount()
{
	return (UINT)m_InstanceDataMems.size();
}
