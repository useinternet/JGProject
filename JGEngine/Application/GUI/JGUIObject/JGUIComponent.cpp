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
		RegisterTransform(this, JGUI_RectTransform_Default);
	}
	JGUIObject::JGUIAwake();
}
void JGUIComponent::JGUIStart()
{
	for (auto& child_info : m_ChildComponents)
	{
		for (auto& child : child_info.second)
		{
			if (child->IsExecuteStartFunc()) continue;
			child->JGUIStart();
		}
	}


	JGUIObject::JGUIStart();
}
void JGUIComponent::JGUITick(const JGUITickEvent& e)
{
	Tick(e);
	if (GetTransform())
	{
		if (m_PrevSize != GetTransform()->GetSize())
		{
			m_PrevSize = GetTransform()->GetSize();
			JGUIResizeEvent e;
			e.width = m_PrevSize.x;
			e.height = m_PrevSize.y;
			JGUIResize(e);
		}
	}
	uint64_t cnt = 0;
	for (auto& child_info : m_ChildComponents)
	{
		for (auto& child : child_info.second)
		{
			if (!child->IsExecuteStartFunc()) child->JGUIStart();
			if (!child->IsActive()) continue;
			child->m_RISortingOrder = m_RISortingOrder + (++cnt);
			child->JGUITick(e);
		}
	}
}
void JGUIComponent::JGUIDestroy()
{
	JGUIObject::JGUIDestroy();

	
	for (auto& child_info : m_ChildComponents)
	{
		for (auto& child : child_info.second)
		{
			auto com = child;
			JGUI::DestroyObject(child);
			if (GetOwnerWindow()->GetFocusComponent() == com) GetOwnerWindow()->SetFocusComponent(nullptr);
		}
	}
	//SetParent(nullptr);
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
	for (auto& child_info : m_ChildComponents)
	{
		for (auto& child : child_info.second)
		{
			child->JGUIMouseMove(e);
		}
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



void JGUIComponent::SetParent(JGUIComponent* parent)
{
	if (m_Flags & JGUI_ComponentFlag_NoParent) return;


	if (parent && parent->m_IsChildLock) return;

	if (m_Parent)
	{
		// »èÁ¦
		auto& childs = m_Parent->m_ChildComponents[m_Priority];
		childs.erase(std::remove_if(childs.begin(), childs.end(), [&](JGUIComponent* com)
		{
			return com == this;
		}), childs.end());

	}
	if (parent)
	{
		// »ðÀÔ
		auto& childs = parent->m_ChildComponents;
		childs[m_Priority].push_back(this);
	}
	m_Parent = parent;
}
JGUIComponent* JGUIComponent::GetParent() const
{
	return m_Parent;
}
vector<JGUIComponent*> JGUIComponent::GetChilds() const
{
	vector<JGUIComponent*> result;
	for (auto& child_info : m_ChildComponents)
	{
		for (auto& child : child_info.second)
		{
			result.push_back(child);
		}
	}
	return result;
}


void JGUIComponent::SetPriority(uint32_t priority)
{
	if (GetParent())
	{
		GetParent()->m_ChildComponents.erase(m_Priority);
		GetParent()->m_ChildComponents[priority].push_back(this);
	}
	m_Priority;
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

	
	if (Interation())
	{
		com = this;
	}
	for (auto& child_info : m_ChildComponents)
	{
		for (auto& child : child_info.second)
		{
			auto temp_com = child->TrackingCanInteractionComponent();
			if (temp_com)
			{
				com = temp_com;
				break;
			}
		}
	}
	return com;
}