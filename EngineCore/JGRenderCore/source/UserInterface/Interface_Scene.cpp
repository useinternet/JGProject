#include"Interface_Scene.h"
#include"Data/Scene.h"
#include"Data/JGRCObject.h"
#include"Data/JGMaterial.h"
#include"Data/JGLight.h"
#include"Data/Mesh/JGSkeletalMesh.h"
#include"Data/Mesh/JGStaticMesh.h"
#include"ResourceManagement/ResourceExtracter.h"
using namespace JGRC;
using namespace std;

IF_Object Interface_Scene::CreateObject(const IF_Material& mat, const IF_Mesh& mesh, const string& meshname, E_IF_ObjType type)
{
	JGRCObject* obj = nullptr;
	switch (type)
	{
	case E_IF_ObjType::Static:
		obj = m_Data->CreateObject(mat.m_Data, mesh.m_Data, meshname, EObjType::Static);
		break;
	case E_IF_ObjType::Dynamic:
		obj = m_Data->CreateObject(mat.m_Data, mesh.m_Data, meshname, EObjType::Dynamic);
		break;
	}
	return IF_Object(obj);
}
IF_SkyBox         Interface_Scene::CreateSkyBox(const wstring& texturePath)
{
	return IF_SkyBox(m_Data->CreateSkyBox(texturePath));
}
void              Interface_Scene::DebugBox(IF_Object obj, const DirectX::XMFLOAT3& color, float thickness)
{
	m_Data->AddDebugBox(obj.m_ObjectData, color, thickness);
}
IF_Material       Interface_Scene::AddMaterial(const MaterialDesc& desc)
{
	return IF_Material(m_Data->AddMaterial(desc));
}
IF_StaticMesh     Interface_Scene::AddStaticMesh()
{
	return IF_StaticMesh(m_Data->AddStaticMesh());
}
IF_SkeletalMesh   Interface_Scene::AddSkeletalMesh() 
{
	return IF_SkeletalMesh(m_Data->AddSkeletalMesh());
}
IF_DirectionLight Interface_Scene::AddDirLight()
{
	return IF_DirectionLight(m_Data->AddLight(ELightType::Direction, ELightExercise::Static));
}
IF_PointLight     Interface_Scene::AddPointLight()
{
	return IF_PointLight(m_Data->AddLight(ELightType::Point, ELightExercise::Static));
}
IF_SpotLight      Interface_Scene::AddSpotLight()
{
	return IF_SpotLight(m_Data->AddLight(ELightType::Spot, ELightExercise::Static));
}
IF_Camera         Interface_Scene::AddCamera()
{
	return IF_Camera(m_Data->AddCamera());
}
void       Interface_Scene::ExtracterResource(UINT flag, const string& OutputPath, const string& path)
{
	ResourceExtracter ex((EExtracter_ResourceType)flag, OutputPath, path);
}
string       Interface_Scene::AddAnimation(const string& path)
{
	return m_Data->AddAnimation(path);
}
void Interface_Scene::AddTexture(const wstring& TexturePath, ETextureType type)
{
	m_Data->AddTexture(TexturePath, type);
}
IF_Camera Interface_Scene::GetMainCamera() const
{
	return IF_Camera(m_Data->GetMainCamera());
}
IF_SkyBox Interface_Scene::GetMainSkyBox() const
{
	return IF_SkyBox(m_Data->GetMainSkyBox());
}
void Interface_Scene::SetMainCamera(const IF_Camera& cam)
{
	m_Data->SetMainCamera(cam.m_Data);
}
void Interface_Scene::SetMainSkyBox(const IF_SkyBox& sky)
{
	m_Data->SetMainSkyBox(sky.m_Data);
}