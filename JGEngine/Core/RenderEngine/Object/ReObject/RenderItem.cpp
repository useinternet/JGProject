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
	RenderItem::RenderItem(const std::string& name) : ReObject(name)
	{
		Material = make_shared<ReMaterial>();
		Mesh = make_shared<ReStaticMesh>();
	}

	InstanceRenderItem* RenderItem::AddInstance()
	{
		if (StructuredBuffer == nullptr)
		{
			StructuredBuffer = make_shared<SBDStructuredBuffer>();
			StructuredBuffer->BindStruct("GameObject");
		
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












	RenderItem* RenderItemManager::CreateItem(const std::string& name)
	{
		auto item = make_shared<RenderItem>(name);
		m_RenderItemPool[item.get()] = item;
		return item.get();
	}





	void RenderItemManager::DeleteItem(RenderItem* item)
	{
		if (m_RenderItemPool.find(item) == m_RenderItemPool.end())
			return;
		m_RenderItemPool.erase(item);
	}


	RenderItemManager::RenderItemManager()
	{
		ReObject::m_RenderItemManager = this;
	}
	std::vector<RenderItem*> RenderItemManager::GetAllItem()
	{
		std::vector<RenderItem*> v;
		for (auto& iter : m_RenderItemPool)
		{
			v.push_back(iter.first);
		}
		return move(v);
	}
	std::vector<RenderItem*> RenderItemManager::GetItemByMesh(EReMeshType type)
	{
		std::vector<RenderItem*> v;
		for (auto& iter : m_RenderItemPool)
		{
			if (iter.second->Mesh->GetType() == type)
			{
				v.push_back(iter.first);
			}
		}
		return move(v);
	}

}