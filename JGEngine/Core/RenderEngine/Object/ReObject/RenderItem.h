#pragma once
#include "Object/ReObject.h"



namespace RE
{

	class ReMesh;
	class ReMaterial;
	class SBDStructuredBuffer;
	class STStruct;
	// RenderItem은 renderengine에 저장
	// 모든 Mesh, Material 은 에셋 매니저에서 관리
	// 모든 module, pass 정보도 에셋매니저에서 관리
	// 모든 텍스쳐, 뼈대 정보, 애니메이션 정보도 에셋매니저에서 관리
	class InstanceRenderItem;
	class ReMaterialController;
	enum class RENDERENGINE_API ERenderItemUsage
	{
		GUI,
		Static3D,
		Skeletal3D
	};
	class RENDERENGINE_API RenderItem : public ReObject
	{
		friend class StaticGBufferModule;
		friend class SkeletalGBufferModule;
		friend class GUIModule;
		friend class FixedGShaderModule;
		friend class FixedGShaderModuleClone;
		static std::map<ERenderItemUsage, std::string> sm_ModuleNameByUsage;
	public:
		static const std::string& GetModuleNameByUsage(ERenderItemUsage usage);
		static void BindShaderModuleByUsage(ERenderItemUsage usage, const std::string& module_name);
	public:
		RenderItem(const std::string& name, ERenderItemUsage usage);
		~RenderItem();
		InstanceRenderItem* AddInstance();
		void RemoveInstance(InstanceRenderItem* instance);
		ReMaterialController* GetMaterial() const {
			return m_Material;
		}
		ReMesh* GetMesh() const {
			return m_Mesh.get();
		}
		void SetMesh(std::shared_ptr<ReMesh> mesh) {
			m_Mesh = mesh;
		}
		void SetMaterial(const std::string& mat_name);
		ERenderItemUsage GetUsage() const {
			return m_Usage;
		}

		void SetActive(bool active) {
			m_IsActive = active;
		}
		bool GetActive() const {
			return m_IsActive;
		}

		uint64_t GetPriority() const {
			return m_Priority;
		}
		void     SetPriority(uint64_t priority) {
			m_Priority = priority;
		}
	private:
		ReMaterialController*                  m_Material = nullptr;
		std::shared_ptr<ReMesh>                m_Mesh;
		ERenderItemUsage                       m_Usage = ERenderItemUsage::Static3D;
		//  
		std::shared_ptr<SBDStructuredBuffer>              m_StructuredBuffer;
		std::vector<std::shared_ptr<InstanceRenderItem>>  m_InstanceItems;
		std::unordered_map<InstanceRenderItem*, uint32_t> m_InstanceMapByPointer;

		bool m_IsActive = true;

		uint64_t m_Priority = 0;
	};


	class StaticRenderItem : public RenderItem
	{
	public:
		StaticRenderItem(const std::string& name) :
			RenderItem(name, ERenderItemUsage::Static3D) {}
	};
	class SkeletalRenderItem : public RenderItem
	{
	public:
		SkeletalRenderItem(const std::string& name) :
			RenderItem(name, ERenderItemUsage::Skeletal3D) {}
	};
	class GUIRenderItem : public RenderItem
	{
	public:
		GUIRenderItem(const std::string& name) :
			RenderItem(name, ERenderItemUsage::GUI) {}
	};



	class RENDERENGINE_API InstanceRenderItem : public ReObject
	{
		friend RenderItem;
	public:
		InstanceRenderItem(const std::string& name) : ReObject(name) {}
	public:
		void Set(const std::string& name, const JMatrix& m);
		bool Get(const std::string& name, JMatrix& m);
	private:
		STStruct* m_Element;
	};

	class RenderItemManager
	{
	public:
		RenderItemManager(uint64_t id);
		StaticRenderItem*   CreateStaticItem(const std::string& name);
		SkeletalRenderItem* CreateSkeletalItem(const std::string& name);
		GUIRenderItem*      CreateGUIItem(const std::string& name);
		void DeleteItem(RenderItem* item);

		void Merge(RenderItemManager* mananger);
	public:
		std::vector<RenderItem*> GetAllItems();
		std::vector<RenderItem*> GetStaticItems();
		std::vector<RenderItem*> GetSkeletaltems();
		std::vector<RenderItem*> GetGUIItems();
	private:
		std::unordered_map<RenderItem*, std::shared_ptr<RenderItem>> m_RenderItemPool;
		std::unordered_map<RenderItem*, StaticRenderItem*>   m_StaticRIs;
		std::unordered_map<RenderItem*, SkeletalRenderItem*> m_SkeletalRIs;
		std::unordered_map<RenderItem*, GUIRenderItem*>      m_GUIRIs;
	};
}