#include "pch.h"
#include "GameObject.h"
#include "Components/HierarchicalComponent.h"
#include "Components/NonHierarchicalComponent.h"

using namespace std;


namespace GFW
{
	void GameObject::Awake()
	{
		m_RootComponent = CreateComponent<HierarchicalComponent>(TT("RootComponent"));

	}
	void GameObject::Start()
	{
	}
	void GameObject::Tick()
	{
		if (!m_IsExecuteStart) {
			m_IsExecuteStart = true;
			Start();
		}




		m_RootComponent->Tick();
		auto Coms = m_Components;

		for (auto& com : Coms)
		{
			com->Tick();
		}


		while (!m_DestroyHComQueue.empty())
		{
			auto com = m_DestroyHComQueue.front(); m_DestroyHComQueue.pop();
			com->Destroy();
			if (com->m_Parent)
			{
				auto iter = std::find(com->m_Parent->m_Childs.begin(), com->m_Parent->m_Childs.end(), com);
				com->m_Parent->m_Childs.erase(iter);
				com->m_Parent = nullptr;
			}
			m_ComponentPool.erase(com);
		}
		while (!m_DestroyNHComQueue.empty())
		{
			auto com = m_DestroyNHComQueue.front(); m_DestroyNHComQueue.pop();
			com->Destroy();
			m_ComponentPool.erase(com);
		}
	}
	void GameObject::Destroy()
	{
		
		m_RootComponent->Destroy();
		m_RootComponent = nullptr;


		auto Coms = m_Components;

		for (auto& com : Coms)
		{
			com->Destroy();
		}

		m_Components.clear();
		m_ComponentPool.clear();
	}
	HierarchicalComponent* GameObject::GetRootComponent() const
	{
		return m_RootComponent;
	}
	void GameObject::SetRootComponent(HierarchicalComponent* root)
	{
		m_RootComponent = root;
		m_RootComponent->SetParent(nullptr);
	}

	void GameObject::DestroyComponent(HierarchicalComponent* com)
	{

		m_DestroyHComQueue.push(com);
	}
	void GameObject::DestroyComponent(NonHierarchicalComponent* com)
	{
		m_DestroyNHComQueue.push(com);
	}
}