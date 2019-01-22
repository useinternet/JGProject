#include"Interface_Scene.h"
#include"Data/Scene.h"

using namespace JGRC;
using namespace std;

IF_Object Interface_Scene::CreateObject(const IF_Material& mat, const IF_Mesh& mesh, const string& meshname, E_IF_ObjType type)
{
	JGRCObject* obj = nullptr;
	switch (type)
	{
	case E_IF_ObjType::Static:
		break;
	case E_IF_ObjType::Dynamic:
		break;
	}
	return IF_Object(obj);
}
IF_InstanceObject Interface_Scene::CreateInstanceObject(const IF_Material& mat, const IF_Mesh& mesh, const string& meshname)
{
	return IF_InstanceObject();
}

IF_SkyBox         Interface_Scene::CreateSkyBox(const wstring& texturePath)
{
	return IF_SkyBox();
}
IF_Material       Interface_Scene::AddMaterial(const MaterialDesc& desc)
{
	return IF_Material();
}
IF_Mesh           Interface_Scene::AddMesh()
{
	return IF_Mesh();
}
IF_DirectionLight Interface_Scene::AddDirLight()
{
	return IF_DirectionLight();
}
IF_PointLight     Interface_Scene::AddPointLight()
{
	return IF_PointLight();
}
IF_SpotLight      Interface_Scene::AddSpotLight()
{
	return IF_SpotLight();
}
IF_Camera         Interface_Scene::AddCamera()
{
	return IF_Camera();
}
void Interface_Scene::AddTexture(const std::wstring& TexturePath, ETextureType type)
{

}
IF_Camera Interface_Scene::GetMainCamera() const
{
	return IF_Camera();
}
IF_SkyBox Interface_Scene::GetMainSkyBox() const
{
	return IF_SkyBox();
}

void Interface_Scene::SetMainCamera(const IF_Camera& cam)
{

}
void Interface_Scene::SetMainSkyBox(const IF_SkyBox& sky)
{

}