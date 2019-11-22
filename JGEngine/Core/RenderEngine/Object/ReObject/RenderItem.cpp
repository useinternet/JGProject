#include "pch.h"
#include "RenderItem.h"
#include "Object/Shader/ShaderModule.h"
#include "Object/DxObject/PipelineState.h"
#include "ReMaterial.h"
#include "ReMesh.h"
using namespace std;


namespace RE
{
	RenderItem::RenderItem(const std::string& name) : ReObject(name)
	{
		Material = make_shared<ReMaterial>();
		Mesh = make_shared<ReStaticMesh>();
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