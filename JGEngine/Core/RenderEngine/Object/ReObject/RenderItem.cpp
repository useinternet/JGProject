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
		ReObject(name), m_Usage(usage)
	{ }

	RenderItem::~RenderItem()
	{
		if (m_Material)
		{
			ReMaterialManager::DeleteMaterialController(m_Material);
		}
	}

	InstanceRenderItem* RenderItem::AddInstance()
	{
		if (m_StructuredBuffer == nullptr)
		{
			m_StructuredBuffer = make_shared<SBDStructuredBuffer>(FixedGShaderModule::BindedGameObjectStructName());
		
			m_StructuredBuffer->BindStruct(FixedGShaderModule::BindedGameObjectStructName());
		
		}
		uint32_t index = (uint32_t)m_InstanceItems.size();
		auto instance = make_shared< InstanceRenderItem>(GetName() + "_Instance" + to_string(m_InstanceItems.size()));
		m_InstanceItems.push_back(instance);
		m_InstanceMapByPointer[instance.get()] = index;
		instance->m_Element = m_StructuredBuffer->Add();
 		instance->Set("World", JMatrix::Identity());
		return instance.get();
	}
	void RenderItem::RemoveInstance(InstanceRenderItem* instance)
	{
		if (m_InstanceMapByPointer.find(instance) == m_InstanceMapByPointer.end())
		{
			return;
		}
		uint32_t index = m_InstanceMapByPointer[instance];
		m_InstanceMapByPointer.erase(instance);
		auto iter = m_InstanceItems.begin() + index;
		m_StructuredBuffer->Remove((*iter)->m_Element);
		m_InstanceItems.erase(iter);
	}

	void RenderItem::SetMaterial(const std::string& mat_name)
	{
		if (m_Material)
		{
			ReMaterialManager::DeleteMaterialController(m_Material);
		}
		m_Material = ReMaterialManager::GetMaterialController(mat_name);
		if (m_Material->GetUsage() != m_Usage)
		{
			ReMaterialManager::DeleteMaterialController(m_Material);
			m_Material = nullptr;
		}
			
	}

	void InstanceRenderItem::Set(const std::string& name, const JMatrix& m)
	{
		auto element = m_Element->GetElement(name);
		if (element == nullptr)
			return;
		if (element->GetType() == JGShader::_matrix4x4)
		{
			((STMatrix*)element)->Set(m);
		}
	}

	bool InstanceRenderItem::Get(const std::string& name, JMatrix& m)
	{
		auto element = m_Element->GetElement(name);
		if (element == nullptr)
			return false;

		if (element->GetType() == JGShader::_matrix4x4)
		{
			m = ((STMatrix*)element)->Get();
			return true;
		}
		return false;
	}



	RenderItemManager::RenderItemManager(uint64_t id)
	{
		ReObject::m_RenderItemManager[id] = this;
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

	void RenderItemManager::Merge(RenderItemManager* mananger)
	{
		for (auto& ri : mananger->m_RenderItemPool)
		{
			m_RenderItemPool[ri.first] = ri.second;
			switch (ri.second->GetUsage())
			{
			case ERenderItemUsage::Static3D:
				m_StaticRIs[ri.first] = static_cast<StaticRenderItem*>(ri.first);
				break;
			case ERenderItemUsage::Skeletal3D:
				m_SkeletalRIs[ri.first] = static_cast<SkeletalRenderItem*>(ri.first);
				break;
			case ERenderItemUsage::GUI:
				m_GUIRIs[ri.first] = static_cast<GUIRenderItem*>(ri.first);
				break;
			}
		}
		mananger->m_StaticRIs.clear();
		mananger->m_SkeletalRIs.clear();
		mananger->m_GUIRIs.clear();
		mananger->m_RenderItemPool.clear();
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
	std::vector<RenderItem*> RenderItemManager::GetStaticItems()
	{
		std::vector<RenderItem*> v(m_StaticRIs.size());
		int count = 0;
		for (auto& ri : m_StaticRIs)
		{
			v[count++] = ri.second;
		}
		return move(v);
	}
	std::vector<RenderItem*> RenderItemManager::GetSkeletaltems()
	{
		std::vector<RenderItem*> v(m_SkeletalRIs.size());
		int count = 0;
		for (auto& ri : m_SkeletalRIs)
		{
			v[count++] = ri.second;
		}
		return move(v);
	}
	std::vector<RenderItem*> RenderItemManager::GetGUIItems()
	{
		std::map<uint64_t, std::vector<RenderItem*>> m;
		std::vector<RenderItem*> v;
		for (auto& ri : m_GUIRIs)
		{
			m[ri.second->GetLayer()].push_back(ri.second);
		}

		
		for (auto& ri : m)
		{
			std::sort(ri.second.begin(), ri.second.end(), [&](RenderItem* r1, RenderItem* r2)
			{
				return r1->GetPriority() < r2->GetPriority();
			});
			v.insert(v.end(), ri.second.begin(), ri.second.end());
		}

	
		//int count = 0;
		//for (auto& ri : m_GUIRIs)
		//{
		//	v[count++] = ri.second;
		//}





		//std::sort(v.begin(), v.end(), [&](RenderItem* r1, RenderItem* r2)
		//{
		//	return r1->GetPriority() < r2->GetPriority();
		//});
		return move(v);
	}
}