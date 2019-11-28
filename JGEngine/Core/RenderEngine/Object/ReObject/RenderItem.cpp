#include "pch.h"
#include "RenderItem.h"
#include "Object/Shader/ShaderModule.h"
#include "Object/DxObject/PipelineState.h"
#include "ReMaterial.h"
#include "ReMesh.h"
#include "Object/Shader/ShaderData.h"
#include "Object/Shader/ShaderLib.h"
#include "Object/Shader/ShaderDataType.h"
using namespace std;


namespace RE
{
	std::map<ERenderItemUsage, std::string> RenderItem::sm_ModuleNameByUsage;

	const string& RenderItem::GetModuleNameByUsage(ERenderItemUsage usage)
	{
		return sm_ModuleNameByUsage[usage];
		// TODO: 여기에 반환 구문을 삽입합니다.
	}

	void RenderItem::BindShaderModuleByUsage(ERenderItemUsage usage, const std::string& module_name)
	{
		sm_ModuleNameByUsage[usage] = module_name;
	}

	RenderItem::RenderItem(const std::string& name, ERenderItemUsage usage) : 
		ReObject(name), Usage(usage)
	{
		switch (usage)
		{
		case ERenderItemUsage::Static3D:
			Mesh = make_shared<ReStaticMesh>(name + "'s ReStaticMesh");
			break;
		case ERenderItemUsage::Skeletal3D:
			Mesh = make_shared<ReSkeletalMesh>(name + "'s ReStaticMesh");
			break;
		case ERenderItemUsage::GUI:
			Mesh = make_shared<ReGuiMesh>(name + "'s ReGUIMesh");
			break;
		}
	}

	RenderItem::~RenderItem()
	{
		if (Material)
		{
			ReMaterialManager::DeleteMaterialController(Material);
		}
	}

	InstanceRenderItem* RenderItem::AddInstance()
	{
		if (StructuredBuffer == nullptr)
		{
			StructuredBuffer = make_shared<SBDStructuredBuffer>(EntryShaderModule::BindedGameObjectStructName());
		
			StructuredBuffer->BindStruct(EntryShaderModule::BindedGameObjectStructName());
		
		}
		uint32_t index = (uint32_t)InstanceItems.size();
		auto instance = make_shared< InstanceRenderItem>(GetName() + "_Instance" + to_string(InstanceItems.size()));
		InstanceItems.push_back(instance);
		InstanceMapByPointer[instance.get()] = index;
		instance->Element = StructuredBuffer->Add();
		instance->Set("World", JMatrix::Identity());
		return instance.get();
	}
	void RenderItem::RemoveInstance(InstanceRenderItem* instance)
	{
		if (InstanceMapByPointer.find(instance) == InstanceMapByPointer.end())
		{
			return;
		}
		uint32_t index = InstanceMapByPointer[instance];
		InstanceMapByPointer.erase(instance);
		auto iter = InstanceItems.begin() + index;
		StructuredBuffer->Remove((*iter)->Element);
		InstanceItems.erase(iter);
	}

	void RenderItem::SetMaterial(const std::string& mat_name)
	{
		if (Material)
		{
			ReMaterialManager::DeleteMaterialController(Material);
		}
		Material = ReMaterialManager::GetMaterialController(mat_name);
		if (Material->GetUsage() != Usage)
		{
			ReMaterialManager::DeleteMaterialController(Material);
			Material = nullptr;
		}
			
	}

	void InstanceRenderItem::Set(const std::string& name, const JMatrix& m)
	{
		auto element = Element->GetElement(name);
		if (element == nullptr)
			return;
		if (element->GetType() == JGShader::_matrix4x4)
		{
			((STMatrix*)element)->Set(m);
		}
	}

	bool InstanceRenderItem::Get(const std::string& name, JMatrix& m)
	{
		auto element = Element->GetElement(name);
		if (element == nullptr)
			return false;

		if (element->GetType() == JGShader::_matrix4x4)
		{
			m = ((STMatrix*)element)->Get();
			return true;
		}
		return false;
	}



	RenderItemManager::RenderItemManager()
	{
		ReObject::m_RenderItemManager = this;
	}

	StaticRenderItem* RenderItemManager::CreateStaticItem(const std::string& name)
	{
		auto item = make_shared<StaticRenderItem>(name);
		m_RenderItemPool[item.get()] = item;
		m_StaticRIs[item.get()] = item.get();
		return item.get();
	}
	SkeletalRenderItem* RenderItemManager::CreateSkeletalItem(const std::string& name)
	{
		auto item = make_shared<SkeletalRenderItem>(name);
		m_RenderItemPool[item.get()] = item;
		m_SkeletalRIs[item.get()] = item.get();
		return item.get();
	}
	GUIRenderItem* RenderItemManager::CreateGUIItem(const std::string& name)
	{
		auto item = make_shared<GUIRenderItem>(name);
		m_RenderItemPool[item.get()] = item;
		m_GUIRIs[item.get()] = item.get();
		return item.get();
	}

	void RenderItemManager::DeleteItem(RenderItem* item)
	{
		if (m_RenderItemPool.find(item) == m_RenderItemPool.end())
			return;
		if (m_StaticRIs.find(item) != m_StaticRIs.end())
			m_StaticRIs.erase(item);
		else if (m_SkeletalRIs.find(item) != m_SkeletalRIs.end())
			m_SkeletalRIs.erase(item);
		else
			m_GUIRIs.erase(item);

		m_RenderItemPool.erase(item);
	}

	std::vector<RenderItem*> RenderItemManager::GetAllItems()
	{
		std::vector<RenderItem*> v;
		for (auto& iter : m_RenderItemPool)
		{
			v.push_back(iter.first);
		}
		return move(v);
	}
	std::vector<StaticRenderItem*> RenderItemManager::GetStaticItems()
	{
		std::vector<StaticRenderItem*> v(m_StaticRIs.size());
		int count = 0;
		for (auto& ri : m_StaticRIs)
		{
			v[count++] = ri.second;
		}
		return move(v);
	}
	std::vector<SkeletalRenderItem*> RenderItemManager::GetSkeletaltems()
	{
		std::vector<SkeletalRenderItem*> v(m_SkeletalRIs.size());
		int count = 0;
		for (auto& ri : m_SkeletalRIs)
		{
			v[count++] = ri.second;
		}
		return move(v);
	}
	std::vector<GUIRenderItem*> RenderItemManager::GetGUIItems()
	{
		std::vector<GUIRenderItem*> v(m_GUIRIs.size());
		int count = 0;
		for (auto& ri : m_GUIRIs)
		{
			v[count++] = ri.second;
		}
		return move(v);
	}


}