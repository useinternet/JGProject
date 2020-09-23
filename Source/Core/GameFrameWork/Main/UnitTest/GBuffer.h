#pragma once
#include "UnitTest.h"

#include "GraphicsIF.h"


namespace GFW
{

	//class GBuffer : public UnitTest
	//{
	//public:
	//	virtual void Start()          override;
	//	virtual void Tick(float tick) override;
	//	virtual void Destroy()        override;
	//	virtual uint64_t GetID() override;
	//	virtual ~GBuffer() {}
	//private:
	//	struct MeshData
	//	{
	//		std::vector<StaticVertex> v;
	//		std::vector<uint32_t>     i;
	//	};
	//	// Test
	//	class ObjectData
	//	{
	//	public:
	//		std::wstring  name;
	//		std::wstring meshFile;
	//		std::wstring material;
	//		JVector3 position;
	//		JVector3 rotation;

	//		SDObject sdData;
	//		SceneObjectRef ptr;
	//		JBBox boundingBox;
	//	};

	//	void LoadMeshs(const std::wstring& path);
	//	void LoadTextures(const std::wstring& path);
	//	void LoadMaterial();

	//	MeshData* GetMesh(const std::wstring& name) {
	//		return &m_MeshDatas[name];
	//	}
	//	GE::Texture* GetTexture(const std::wstring& name) {
	//		return m_Textures[name].Get();
	//	}
	//	GE::Material* GetMaterial(const std::wstring& name) {
	//		return m_Materials[name].Get();
	//	}
	//	ObjectData* GetSceneObject(const std::wstring& name) {
	//		return m_Objects[name].get();

	//	}

	//	void CreateObject(const ObjectData& data);
	//	void ConvertWorldMatrix(const std::wstring& name, const JVector3& pos, const JVector3& rot);
	//private:
	//	std::map<std::wstring, MeshData>    m_MeshDatas;
	//	std::map<std::wstring, TextureRef>  m_Textures;
	//	std::map<std::wstring, MaterialRef> m_Materials;
	//	std::map<std::wstring, std::shared_ptr<ObjectData>>  m_Objects;
	//	

	//private:
	//	SceneRef        m_Scene;
	//	GE::SceneCamera m_Cam;


	//	DirectionalLightRef m_Sun;
	};
	// REGISTER_UNITTEST(GBuffer)
//}