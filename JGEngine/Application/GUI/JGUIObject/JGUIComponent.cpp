#include "pch.h"
#include "JGUIComponent.h"
#include "RenderEngine.h"
#include "GUI/JGUIComponent/JGUIRectTransform.h"
#include "GUI/JGUIComponent/JGUICollider.h"
#include "JGUIWindow.h"
using namespace std;


JGUIWindow* JGUIComponent::GetOwnerWindow() const
{
	return m_OwnerWindow;
}

void JGUIComponent::JGUIAwake()
{

	if (typeid(*this) != typeid(JGUIRectTransform))
	{
		m_RectTransform = CreateJGUIComponent<JGUIRectTransform>("JGUIRectTransform");
	}
	JGUIObject::JGUIAwake();
}
void JGUIComponent::JGUIStart()
{
	for (auto& child : m_ChildComponents)
	{
		if (child->IsExecuteStartFunc()) continue;
		child->JGUIStart();
	}
	JGUIObject::JGUIStart();
}
void JGUIComponent::JGUITick(const JGUITickEvent& e)
{

	Tick(e);
	if (m_RectTransform)
	{
		if (m_PrevSize != m_RectTransform->GetSize())
		{
			m_PrevSize = m_RectTransform->GetSize();
			JGUIResizeEvent e;
			e.width = m_PrevSize.x;
			e.height = m_PrevSize.y;
			if (m_Collider)
			{
				m_Collider->GetTransform()->SetSize(e.width, e.height);
			}

			JGUIResize(e);
		}
	}

	for (auto& child : m_ChildComponents)
	{
		if (!child->IsExecuteStartFunc()) child->JGUIStart();
		if (!child->IsActive()) continue;
		child->JGUITick(e);
	}

}
void JGUIComponent::JGUIDestroy()
{
	JGUIObject::JGUIDestroy();

	SetParent(nullptr);
	// 자식 손절
	for (int i = 0; i < m_ChildComponents.size();)
	{
		auto com = m_ChildComponents[i];
		JGUI::DestroyObject(m_ChildComponents[i]);
		if (GetOwnerWindow()->GetFocusComponent() == com) GetOwnerWindow()->SetFocusComponent(nullptr);
	}
	SetActive(false);
}

void JGUIComponent::JGUIResize(const JGUIResizeEvent& e)
{
	Resize(e);
}
void JGUIComponent::JGUIChar(const JGUICharEvent& e)
{
	Char(e);
}
void JGUIComponent::JGUIKeyDown(const JGUIKeyDownEvent& e)
{
	KeyDown(e);
}
void JGUIComponent::JGUIKeyUp(const JGUIKeyUpEvent& e)
{
	KeyUp(e);
}
void JGUIComponent::JGUIMouseMove(const JGUIMouseMoveEvent& e)
{
	bool interation_result = Interation();
	if (m_IsMouseTracking == false && GetParent() && interation_result)
	{
		m_IsMouseTracking = true;
		JGUIMouseTrack track;
		track.com = this;
		track.flag = JGUI_MOUSETRACKFLAG_MOUSEHOVER;
		track.time = JGUI_DEFAULT_HOVERTRACKTIME;
		JGUI::RegisterMouseTrack(track);
	}
	for (auto& com : m_ChildComponents)
	{
		com->JGUIMouseMove(e);
	}
	if(interation_result) MouseMove(e);
}

void JGUIComponent::JGUIMouseBtDown(const JGUIKeyDownEvent& e)
{
	MouseBtDown(e);
}

void JGUIComponent::JGUIMouseBtUp(const JGUIKeyUpEvent& e)
{
	MouseBtUp(e);
}

void JGUIComponent::JGUIMouseLeave()
{
	if (m_IsMouseTracking)
	{
		m_IsMouseTracking = false;
		MouseLeave();
	}
}

void JGUIComponent::JGUIMouseHover()
{
	if (m_IsMouseTracking)
	{
		JGUIMouseTrack track;
		track.com = this;
		track.flag = JGUI_MOUSETRACKFLAG_MOUSELEAVE;
		track.time = JGUI_DEFAULT_LEAVETRACKTIME;
		JGUI::RegisterMouseTrack(track);
		MouseHover();
	}

}

void JGUIComponent::JGUIFocusEnter(const JGUIFocusEnterEvent& e)
{
	FocusEnter(e);
}

void JGUIComponent::JGUIFocusExit(const JGUIFocusExitEvent& e)
{
	FocusExit(e);
}

void JGUIComponent::JGUIOnFocus()
{
	OnFocus();
}

void JGUIComponent::JGUIParentUpdateNotification()
{
	ParentUpdateNotification();
	for (auto& child : m_ChildComponents)
	{
		child->JGUIParentUpdateNotification();
	}
}

void JGUIComponent::SetParent(JGUIComponent* parent)
{
	if (parent && parent->m_IsChildLock) return;

	if (m_Parent)
	{
		// 삭제
		auto& childs = m_Parent->m_ChildComponents;
		childs.erase(std::remove_if(childs.begin(), childs.end(), [&](JGUIComponent* com)
		{
			return com == this;
		}), childs.end());

	}
	if (parent)
	{
		// 삽입
		auto& childs = parent->m_ChildComponents;
		childs.push_back(this);
	}
	auto temp_parent = m_Parent;
	m_Parent = parent;

	if (temp_parent != parent)
	{
		JGUIParentUpdateNotification();
	}

}
JGUIComponent* JGUIComponent::GetParent() const
{
	return m_Parent;
}
const std::vector<JGUIComponent*>& JGUIComponent::GetChilds() const
{
	return m_ChildComponents;
}
std::vector<JGUIComponent*>& JGUIComponent::GetChilds()
{
	return m_ChildComponents;

}
JGUIComponent* JGUIComponent::FindChild(uint32_t index)
{
	if (m_ChildComponents.size() <= index)
		return nullptr;
	return m_ChildComponents[index];
}
JGUIComponent* JGUIComponent::FindChild(const std::string& name)
{
	for (auto& com : m_ChildComponents)
	{
		if (com->GetName() == name)
			return com;
	}
	return nullptr;
}

void JGUIComponent::RegisterCollider(EJGUI_Component_Colider colider_type, EJGUI_ComponentInteractionFlags flags)
{
	switch (colider_type)
	{
	case JGUI_Component_Colider_Box:
		m_Collider = CreateJGUIComponent<JGUIBoxCollider>("JGUIBoxCollider");
		break;
	case JGUI_Component_Colider_EmptyBox:
		m_Collider = CreateJGUIComponent<JGUIEmptyBoxColider>("JGUIEmptyBoxCollider");
		break;
	}
	m_Collider->SetParent(this);
	m_Collider->GetTransform()->SetSize(GetTransform()->GetSize());
	m_InteractionFlag = flags;
}


void JGUIComponent::Init(const std::string& name, JGUIWindow* owner_window)
{
	SetName(name);
	m_OwnerWindow = owner_window;

}

bool JGUIComponent::Interation()
{

	auto p = GetOwnerWindow()->GetMousePos();
	return IsActive() && GetCollider() && GetCollider()->CheckInPoint(p);
}

JGUIComponent* JGUIComponent::TrackingCanInteractionComponent()
{
	JGUIComponent* com = nullptr;

	auto flag = GetInteractionFlag();
	if (flag & JGUI_ComponentInteractionFlag_Default && Interation())
	{
		com = this;
	}

	for (auto& child : m_ChildComponents)
	{
		auto temp_com = child->TrackingCanInteractionComponent();
		if (temp_com)
		{
			com = temp_com;
			break;
		}
	}
	return com;
}