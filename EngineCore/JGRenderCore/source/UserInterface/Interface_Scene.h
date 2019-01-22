#pragma once
#include"Data/Scene.h"
#include"Interface_SceneObject.h"
#include"Interface_SceneInstanceObject.h"
#include"Interface_SceneMaterial.h"
#include"Interface_SceneMesh.h"
#include"Interface_SceneDirectionLight.h"
#include"Interface_ScenePointLight.h"
#include"Interface_SceneSpotLight.h"
#include"Interface_SceneSkyBox.h"
#include"Interface_SceneCamera.h"
namespace JGRC
{
	class RCORE_EXPORT Interface_Scene
	{
	private:
		class Scene* m_Data = nullptr;

	public:
		Interface_Scene* operator->()
		{
			return this;
		}
	public:
		Interface_Scene() = default;
		Interface_Scene(Scene* scene) : m_Data(scene) {}
		Interface_Scene(const Interface_Scene& copy)
		{
			m_Data = copy.m_Data;
		}
	public:
		IF_Object         CreateObject(const IF_Material& mat, const IF_Mesh& mesh, const std::string& meshname, E_IF_ObjType type = E_IF_ObjType::Static);
		IF_InstanceObject CreateInstanceObject(const IF_Material& mat, const IF_Mesh& mesh, const std::string& meshname);
		IF_SkyBox         CreateSkyBox(const std::wstring& texturePath);
		IF_Material       AddMaterial(const struct MaterialDesc& desc);
		IF_Mesh           AddMesh();
		IF_DirectionLight AddDirLight();
		IF_PointLight     AddPointLight();
		IF_SpotLight      AddSpotLight();
		IF_Camera         AddCamera();

		void AddTexture(const std::wstring& TexturePath, ETextureType type = ETextureType::Default);
	public:
		IF_Camera GetMainCamera() const;
		IF_SkyBox GetMainSkyBox() const;
	public:
		void SetMainCamera(const IF_Camera& cam);
		void SetMainSkyBox(const IF_SkyBox& sky);
	};


	typedef Interface_Scene IF_Scene;
}