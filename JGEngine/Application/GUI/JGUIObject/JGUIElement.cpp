#include "pch.h"
#include "JGUIElement.h"
#include "RenderEngine.h"
#include "JGUIWindow.h"
using namespace std;

uint64_t JGUIElement::m_RIOffset = 0;
JGUIWindow* JGUIElement::GetOwnerWindow() const
{
	return m_OwnerWindow;
}

void JGUIElement::SetActive(bool active, bool is_hierarchy)
{
	JGUIObject::SetActive(active, is_hierarchy);
	for (auto& child : m_Childs)
	{
		child->SetActive(IsActive(), true);
	}
}
void JGUIElement::JGUIAwake()
{
	m_Transform = CreateJGUIComponent<JGUITransform>();
	JGUIObject::JGUIAwake();
}
void JGUIElement::JGUIStart()
{
	JGUIObject::JGUIStart();
	for (auto& child : m_Childs)
	{
		if (child->IsExecuteStartFunc()) continue;
		child->JGUIStart();
	}
}
void JGUIElement::JGUIDestroy()
{
	JGUIObject::JGUIDestroy();
	if (GetParent())
	{
		if (GetOwnerWindow()->GetFocusElement() == this) GetOwnerWindow()->SetFocusElement(nullptr);
		m_Parent->m_Childs.erase(std::remove(
			m_Parent->m_Childs.begin(),
			m_Parent->m_Childs.end(), this), m_Parent->m_Childs.end());
	}

	auto childs = m_Childs;
	for (auto& child : childs)
	{
		auto com = child;
		JGUI::DestroyObject(child);
		if (GetOwnerWindow()->GetFocusElement() == com) GetOwnerWindow()->SetFocusElement(nullptr);
	}
	SetActive(false);
}
void JGUIElement::JGUITick(const JGUITickEvent& e)
{
	JGUIObject::JGUITick(e);
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
	for (int i = 0; i < m_Childs.size(); ++i)
	{
		auto child = m_Childs[i];
		if (!child->IsExecuteStartFunc()) child->JGUIStart();
		if (!child->IsActive()) continue;
		if (child->IsDestroying()) continue;
		child->m_RISortingOrder = ++m_RIOffset;
		child->JGUITick(e);
	}
}


void JGUIElement::JGUIResize(const JGUIResizeEvent& e)
{
	Resize(e);
	for (auto& func_pair : m_ResizeFuncs)
	{
		for (auto& func : func_pair.second) func(e);
	}
}
void JGUIElement::JGUIMouseWheel(const JGUIMouseWheelEvent& e)
{
	MouseWheel(e);
	for (auto& func_pair : m_MouseWheelFuncs)
	{
		for (auto& func : func_pair.second) func(e);
	}
}
void JGUIElement::JGUIChar(const JGUICharEvent& e)
{
	Char(e);
	for (auto& func_pair : m_CharFuncs)
	{
		for (auto& func : func_pair.second) func(e);
	}
}
void JGUIElement::JGUIKeyDown(const JGUIKeyDownEvent& e)
{
	KeyDown(e);
	for (auto& func_pair : m_KeyDownFuncs)
	{
		for (auto& func : func_pair.second) func(e);
	}
}
void JGUIElement::JGUIKeyUp(const JGUIKeyUpEvent& e)
{
	KeyUp(e);
	for (auto& func_pair : m_KeyUpFuncs)
	{
		for (auto& func : func_pair.second) func(e);
	}
}
void JGUIElement::JGUIMouseMove(const JGUIMouseMoveEvent& e)
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
	for (int i = 0; i < m_Childs.size(); ++i)
	{
		m_Childs[i]->JGUIMouseMove(e);
	}

	if (interation_result)
	{
		MouseMove(e);
		for (auto& func_pair : m_MouseMoveFuncs)
		{
			for (auto& func : func_pair.second) func(e);
		}
	}
}

void JGUIElement::JGUIMouseBtDown(const JGUIKeyDownEvent& e)
{
	MouseBtDown(e);
	for (auto& func_pair : m_MouseBtDownFuncs)
	{
		for (auto& func : func_pair.second) func(e);
	}
}

void JGUIElement::JGUIMouseBtUp(const JGUIKeyUpEvent& e)
{
	MouseBtUp(e);
	for (auto& func_pair : m_MouseBtUpFuncs)
	{
		for (auto& func : func_pair.second) func(e);
	}
}

void JGUIElement::JGUIMouseLeave()
{
	if (m_IsMouseTracking)
	{
		m_IsMouseTracking = false;
		MouseLeave();
		for (auto& func_pair : m_MouseLeaveFuncs)
		{
			for (auto& func : func_pair.second) func();
		}
	}
}

void JGUIElement::JGUIMouseHover()
{
	if (m_IsMouseTracking)
	{
		JGUIMouseTrack track;
		track.com = this;
		track.flag = JGUI_MOUSETRACKFLAG_MOUSELEAVE;
		track.time = JGUI_DEFAULT_LEAVETRACKTIME;
		JGUI::RegisterMouseTrack(track);
		MouseHover();
		for (auto& func_pair : m_MouseHoverFuncs)
		{
			for (auto& func : func_pair.second) func();
		}
	}

}

void JGUIElement::JGUIFocusEnter(const JGUIFocusEnterEvent& e)
{
	FocusEnter(e);
	for (auto& func_pair : m_FocusEnterFuncs)
	{
		for (auto& func : func_pair.second) func(e);
	}
}

void JGUIElement::JGUIFocusExit(const JGUIFocusExitEvent& e)
{
	FocusExit(e);
	for (auto& func_pair : m_FocusExitFuncs)
	{
		for (auto& func : func_pair.second) func(e);
	}
}

void JGUIElement::JGUIOnFocus()
{
	OnFocus();
	for (auto& func_pair : m_OnFocusFuncs)
	{
		for (auto& func : func_pair.second) func();
	}
}



void JGUIElement::SetParent(JGUIElement* parent)
{
	auto prev_parent = m_Parent;
	if (m_Parent)
	{
		m_Parent->m_Childs.erase(std::remove_if(
			m_Parent->m_Childs.begin(),
			m_Parent->m_Childs.end(), [&](JGUIElement* com)
		{
			return com == this;
		}), m_Parent->m_Childs.end());
	}
	if (parent)
	{
		parent->m_Childs.push_back(this);
	}
	m_Parent = parent;

	NotifyChangeParentToComponents(prev_parent);

}
JGUIElement* JGUIElement::GetParent() const
{
	return m_Parent;
}
void JGUIElement::AddElementFlags(EJGUI_ComponentFlags flags)
{
	m_Flags = m_Flags | flags;
}
void JGUIElement::RemoveElementFlags(EJGUI_ComponentFlags flags)
{
	m_Flags = m_Flags & (~flags);

}
void JGUIElement::SetElementFlags(EJGUI_ComponentFlags flags)
{
	m_Flags = flags;
}

void JGUIElement::DestroyJGUIElement(JGUIElement* com)
{
	GetOwnerWindow()->DestroyJGUIElement(com);
}



bool JGUIElement::Interation()
{
	auto p = GetOwnerWindow()->GetMousePos();
	return IsActive() && GetCollider() && GetCollider()->CheckInPoint(p);
}

JGUIElement* JGUIElement::TrackingCanInteractionComponent()
{
	JGUIElement* com = nullptr;

	if (Interation())
	{
		com = this;
	}
	for (auto& child : m_Childs)
	{

		auto temp_com = child->TrackingCanInteractionComponent();

		if (temp_com)
		{
			com = temp_com;
		}
	}
	return com;
}

void JGUIElement::NotifyChangeParentToComponents(JGUIObject* prevParent)
{
	for (size_t i = 0; i < m_Coms.size(); ++i)
	{
		m_Coms[i]->JGUIChangeParent(prevParent);
	}
	for (size_t i = 0; i < m_Childs.size(); ++i)
	{
		m_Childs[i]->NotifyChangeParentToComponents(prevParent);
	}

}


