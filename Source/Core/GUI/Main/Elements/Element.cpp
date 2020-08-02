#include "pch.h"
#include "Element.h"
#include "Components/Transform.h"
#include "Windows/Window.h"
using namespace std;



namespace GUI
{
	void Element::GUIAwake()
	{
		m_Transform = CreateComponent<Transform>(TT("Transform"));
		m_PrevSize = m_Transform->GetSize();
		Awake();
	}

	void Element::GUIStart()
	{
		Start();
	}

	void Element::GUITick()
	{
		if (m_PrevSize != GetTransform()->GetSize())
		{
			auto size = GetTransform()->GetSize();
			OnResize(size.x, size.y);
			m_PrevSize = size;
		}

		Tick();
		// ComponentTick
		for (auto& com : m_Components)
		{
			if (!com->m_IsExecuteStart)
			{
				com->m_IsExecuteStart = true;
				com->Start();
			}
			com->Tick();
		}


		// Element Tick
		uint32_t childCnt = m_Transform->GetChildCount();

		for (uint32_t i = 0; i < childCnt; ++i)
		{
			auto child = m_Transform->GetChild(i)->GetOwner();
			if (!child->m_IsExecuteStart)
			{
				child->m_IsExecuteStart = true;
				child->GUIStart();
			}
			child->GUITick();
		}
	}
	
	void Element::GUIDestroy()
	{
		Destroy();
		if (GetOwnerWindow() && GetOwnerWindow()->GetFocusElement() == this)
		{
			GetOwnerWindow()->SetFocusElement(nullptr);
		}

		uint32_t childCnt = m_Transform->GetChildCount();

		for (uint32_t i = 0; i < childCnt; ++i)
		{
			auto child = m_Transform->GetChild(i)->GetOwner();
			child->GUIDestroy();
		}

		for (auto& com : m_Components)
		{
			com->Destroy();
			GUIIF::UnAllocateGUIObject(com);
		}
		m_Components.clear();


		// 자기자신도 소멸
		m_OwnerWindow = nullptr;
		GUIIF::UnAllocateGUIObject(this);
	}




	void Element::DestroyComponent(Component* component)
	{
		// 이것도 Tick 다돌고 제거
		component->Destroy();
		auto iter = std::find(m_Components.begin(), m_Components.end(), component);
		m_Components.erase(iter);
		GUIIF::UnAllocateGUIObject(component);
	}
	void Element::DestroyElement(Element* element)
	{
		GUIIF::ReserveDestroyElement(element);
	}
	Component* Element::FindComponent(const std::wstring& name)
	{
		auto iter = std::find_if(m_Components.begin(), m_Components.end(), [&](Component* com)
		{
			return com->GetName() == name;
		});


		return *iter;
	}

	Component* Element::FindComponent(uint32_t index)
	{
		if (m_Components.size() <= index) return nullptr;


		return m_Components[index];
	}
}

