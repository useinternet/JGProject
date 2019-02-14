#pragma once
#include"DxCommon/DxCommon.h"
namespace JGRC
{
	class CommandListManager;
	class Object;
	class SceneObject;
	class JGMaterial;
	class JGBaseMesh;
	class JGSkeletalMesh;
	class JGStaticMesh;
	struct MatPersonalData;
	struct MaterialDesc;
	enum class EObjectType;
	class DataManager
	{
	private:
		std::vector<std::unique_ptr<ObjectData>>   m_ObjectDataMems;
		std::vector<std::unique_ptr<PassData>>     m_PassDataMems;
		std::vector<std::unique_ptr<SkinnedData>>  m_SkinnedDataMems;
		std::vector<std::unique_ptr<InstanceData>> m_InstanceDataMems;
		std::vector<std::unique_ptr<MaterialData>> m_MaterialDataMems;
	private:
		std::vector<std::shared_ptr<Object>>     m_ObjectMems;
		std::vector<std::shared_ptr<JGMaterial>> m_MatMems;
		std::vector<std::shared_ptr<MatPersonalData>> m_MatDataMems;
		std::vector<std::shared_ptr<JGBaseMesh>>  m_MeshMems;
	private:
		UINT m_ObjectCBIndex   = 0;
		UINT m_MaterialCBIndex = 0;
		UINT m_PassDataCBIndex = 0;
		UINT m_SkinnedCBIndex  = 0;
	public:
		DataManager() = default;
		~DataManager() = default;
		DataManager(const DataManager& rhs) = delete;
		DataManager& operator=(const DataManager& rhs) = delete;
	public:
		SceneObject*    CreateSceneObject(
			const std::string& name,
		    JGMaterial* mat,
			const std::string& matDataName,
			JGBaseMesh* mesh,
			const std::string& meshName,
			EObjectType type);
		JGMaterial*      CreateMaterial(const MaterialDesc& desc);
		MatPersonalData* CreateMaterialData(const std::string& name);
		PassData*        CreatePassData();
		JGStaticMesh*    CreateStaticMesh(const std::string& name);
		JGSkeletalMesh*  CreateSkeletalMesh(const std::string& name);

		void Build();
	public:
		UINT ObjectCount();
		UINT MaterialDataCount();
		UINT PassDataCount();
	};



}