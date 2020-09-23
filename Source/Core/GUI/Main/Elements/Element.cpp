#include "pch.h"
#include "Element.h"
#include "Components/Transform.h"
#include "Components/NativeScriptComponent.h"
#include "Windows/Window.h"
using namespace std;



namespace JGUI
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
			if (!child->m_IsExecuteStart && child->IsActive())
			{
				child->m_IsExecuteStart = true;
				child->GUIStart();
			}
			if(child->IsActive()) child->GUITick();
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

	void Element::OnMouseButtonDown(KeyCode bt)
	{
		if (!IsActive()) return;
		for (auto& script : m_ScriptComponents)
		{
			if (!script.second->IsActive()) continue;
			if (script.second->OnMouseButtonDown) script.second->OnMouseButtonDown(bt);
		}
	}

	void Element::OnMouseButton(KeyCode bt)
	{
		if (!IsActive()) return;
		for (auto& script : m_ScriptComponents)
		{
			if (!script.second->IsActive()) continue;
			if (script.second->OnMouseButton) script.second->OnMouseButton(bt);
		}
	}

	void Element::OnMouseButtonUp(KeyCode bt)
	{
		if (!IsActive()) return;
		for (auto& script : m_ScriptComponents)
		{
			if (!script.second->IsActive()) continue;
			if (script.second->OnMouseButtonUp) script.second->OnMouseButtonUp(bt);
		}
	}

	void Element::OnMouseClick(KeyCode bt)
	{
		if (!IsActive()) return;
		for (auto& script : m_ScriptComponents)
		{
			if (!script.second->IsActive()) continue;
			if (script.second->OnMouseClick) script.second->OnMouseClick(bt);
		}
	}

	void Element::OnMouseDoubleClick(KeyCode bt)
	{
		if (!IsActive()) return;
		for (auto& script : m_ScriptComponents)
		{
			if (!script.second->IsActive()) continue;
			if (script.second->OnMouseDoubleClick) script.second->OnMouseDoubleClick(bt);
		}
	}

	void Element::OnMouseEnter()
	{
		if (!IsActive()) return;
		for (auto& script : m_ScriptComponents)
		{
			if (!script.second->IsActive()) continue;
			if (script.second->OnMouseEnter) script.second->OnMouseEnter();
		}
	}

	void Element::OnMouseMove()
	{
		if (!IsActive()) return;
		for (auto& script : m_ScriptComponents)
		{
			if (!script.second->IsActive()) continue;
			if (script.second->OnMouseEnter) script.second->OnMouseEnter();
		}
	}

	void Element::OnMouseExit()
	{
		if (!IsActive()) return;
		for (auto& script : m_ScriptComponents)
		{
			if (!script.second->IsActive()) continue;
			if (script.second->OnMouseExit) script.second->OnMouseExit();
		}
	}

	void Element::OnKeyDown(KeyCode code)
	{
		if (!IsActive()) return;
		for (auto& script : m_ScriptComponents)
		{
			if (!script.second->IsActive()) continue;
			if (script.second->OnKeyDown) script.second->OnKeyDown(code);
		}
	}

	void Element::OnKey(KeyCode code)
	{
		if (!IsActive()) return;
		for (auto& script : m_ScriptComponents)
		{
			if (!script.second->IsActive()) continue;
			if (script.second->OnKey) script.second->OnKey(code);
		}
	}

	void Element::OnKeyUp(KeyCode code)
	{
		if (!IsActive()) return;
		for (auto& script : m_ScriptComponents)
		{
			if (!script.second->IsActive()) continue;
			if (script.second->OnKeyUp) script.second->OnKeyUp(code);
		}
	}

	void Element::OnResize(float width, float height)
	{
		if (!IsActive()) return;
		for (auto& script : m_ScriptComponents)
		{
			if (!script.second->IsActive()) continue;
			if (script.second->OnResize) script.second->OnResize(width, height);
		}
	}

	void Element::OnFocusEnter()
	{
		if (!IsActive()) return;
		for (auto& script : m_ScriptComponents)
		{
			if (!script.second->IsActive()) continue;
			if (script.second->OnFocusEnter) script.second->OnFocusEnter();
		}
	}

	void Element::OnFocus()
	{
		if (!IsActive()) return;
		for (auto& script : m_ScriptComponents)
		{
			if (!script.second->IsActive()) continue;
			if (script.second->OnFocus) script.second->OnFocus();
		}
	}

	void Element::OnFocusExit()
	{
		if (!IsActive()) return;
		for (auto& script : m_ScriptComponents)
		{
			if (!script.second->IsActive()) continue;
			if (script.second->OnFocusExit) script.second->OnFocusExit();
		}
	}

	void Element::OnDropItem(const GUIDropItem& item)
	{
		if (!IsActive()) return;
		for (auto& script : m_ScriptComponents)
		{
			if (!script.second->IsActive()) continue;
			if (script.second->OnDropItem) script.second->OnDropItem(item);
		}
	}

	void Element::OnChar(wchar_t s)
	{
		if (!IsActive()) return;
		for (auto& script : m_ScriptComponents)
		{
			if (!script.second->IsActive()) continue;
			if (script.second->OnChar) script.second->OnChar(s);
		}
	}




	void Element::DestroyComponent(Component* component)
	{
		// 이것도 Tick 다돌고 제거
		component->Destroy();

		{
			auto iter = m_ScriptComponents.find(component);
			if (iter != m_ScriptComponents.end()) m_ScriptComponents.erase(iter);
		}

		{
			auto iter = std::find(m_Components.begin(), m_Components.end(), component);
			m_Components.erase(iter);
		}





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
	void Element::SetActive(bool is_active)
	{
		m_IsActiveSelf = is_active;
		SetActiveHierarchy(is_active);

	}
	void Element::SetActiveHierarchy(bool is_active)
	{
		uint32_t childCnt = GetTransform()->GetChildCount();
		for (uint32_t i = 0; i < childCnt; ++i)
		{
			GetTransform()->GetChild(i)->GetOwner()->SetActiveHierarchy(is_active);
		}
	}
}

