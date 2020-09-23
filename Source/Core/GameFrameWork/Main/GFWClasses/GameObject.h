#pragma once
#include "GFWObject.h"




namespace GFW
{
	class SubGameWorld;
	class HierarchicalComponent;
	class NonHierarchicalComponent;
	class GameObjectComponent;
	class GAMEFRAMEWORK_API GameObject : public GFWObject
	{
		friend SubGameWorld;
	public:
		GameObject(const std::wstring& name) : GFWObject(name) {}
		virtual ~GameObject() = default;


	protected:
		virtual void Awake()   override;
		virtual void Start()   override;
		virtual void Tick()    override;
		virtual void Destroy() override;
	public:
		HierarchicalComponent* GetRootComponent() const;
		void SetRootComponent(HierarchicalComponent* root);

		template<typename ComponentType>
		ComponentType* CreateComponent(const std::wstring& name) {
			auto com = std::make_shared<ComponentType>();
			com->SetName(name);
			com->m_OwnerWorld = m_OwnerWorld;
			com->m_Owner      = this;

			if (std::is_base_of<HierarchicalComponent, ComponentType>::value)
			{
				if (m_RootComponent != nullptr) com->SetParent(m_RootComponent);
			}
			else
			{
				m_Components.push_back(com.get());
			}

			com->Awake();
			m_ComponentPool[com.get()] = com;
			return com.get();

		}

		void DestroyComponent(HierarchicalComponent* com);
		void DestroyComponent(NonHierarchicalComponent* com);

	private:
		SubGameWorld* m_OwnerWorld = nullptr;


		HierarchicalComponent* m_RootComponent = nullptr;
		std::vector<GameObjectComponent*> m_Components;

		using ComponentPool = std::unordered_map<GameObjectComponent*, std::shared_ptr<GameObjectComponent>>;
		ComponentPool m_ComponentPool;

		std::queue<HierarchicalComponent*>    m_DestroyHComQueue;
		std::queue<NonHierarchicalComponent*> m_DestroyNHComQueue;
		

		bool m_IsExecuteStart = false;


	};
}