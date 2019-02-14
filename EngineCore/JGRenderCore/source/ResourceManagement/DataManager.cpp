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
	auto Obj = make_unique<SceneObject>(m_ObjectCBIndex++, type, name);
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
	m_MatMems.push_back(move(mat));
	return result;
}
MatPersonalData* DataManager::CreateMaterialData(const std::string& name)
{
	auto matData = make_unique<MatPersonalData>();
	MatPersonalData* result = matData.get();
	m_MatDataMems.push_back(move(matData));


	result->Name = name;
	result->CBIndex = m_MaterialCBIndex++;
	return result;
}
PassData*       DataManager::CreatePassData()
{
	auto passData = make_unique<PassData>(m_PassDataCBIndex++);
	PassData* result = passData.get();
	m_PassDataMems.push_back(move(passData));
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

UINT DataManager::ObjectCount()
{
	return (UINT)m_ObjectMems.size();
}
UINT DataManager::MaterialDataCount()
{
	return (UINT)m_MatDataMems.size();
}
UINT DataManager::PassDataCount()
{
	return (UINT)m_PassDataMems.size();
}