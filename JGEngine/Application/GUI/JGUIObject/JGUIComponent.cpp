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
	EventProcess(JGUI_ComponentEvent_Awake, nullptr);
}
void JGUIComponent::JGUIStart()
{
	for (auto& child : m_ChildComponents)
	{
		if (child->IsExecuteStartFunc()) continue;
		child->JGUIStart();
	}
	EventProcess(JGUI_ComponentEvent_Start, nullptr);
}
void JGUIComponent::JGUITick(const JGUITickEvent& e)
{
	EventProcess(JGUI_ComponentEvent_Tick, &e);

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
	EventProcess(JGUI_ComponentEvent_Destroy, nullptr);

	

	SetParent(nullptr);
	// 자식 손절
	for (int i = 0; i < m_ChildComponents.size();)
	{
		auto com = m_ChildComponents[i];
		JGUI::DestroyObject(m_ChildComponents[i]);
		if (GetOwnerWindow()->GetFocus() == com) GetOwnerWindow()->SetFocus(nullptr);
	}
}

void JGUIComponent::JGUIResize(const JGUIResizeEvent& e)
{
	EventProcess(JGUI_ComponentEvent_Resize, &e);
}

void JGUIComponent::JGUIMouseMove(const JGUIMouseMoveEvent& e)
{
	if (m_IsMouseTracking == false && GetParent())
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
		if (com->Interation(JGUI_ComponentEvent_MouseMove))
		{
			com->JGUIMouseMove(e);
		}
	}

	EventProcess(JGUI_ComponentEvent_MouseMove, &e);

}

void JGUIComponent::JGUIMouseBtDown(const JGUIKeyDownEvent& e)
{
	for (auto& com : m_ChildComponents)
	{
		if (com->Interation(JGUI_ComponentEvent_MouseBtDown))
		{
			com->JGUIMouseBtDown(e);
		}
	}

	EventProcess(JGUI_ComponentEvent_MouseBtDown, &e);
}

void JGUIComponent::JGUIMouseBtUp(const JGUIKeyUpEvent& e)
{
	for (auto& com : m_ChildComponents)
	{
		if (com->Interation(JGUI_ComponentEvent_MouseBtUp))
		{
			com->JGUIMouseBtUp(e);
		}
	}

	EventProcess(JGUI_ComponentEvent_MouseBtUp, &e);
}

void JGUIComponent::JGUIMouseLeave()
{
	if (m_IsMouseTracking)
	{
		m_IsMouseTracking = false;
		EventProcess(JGUI_ComponentEvent_MouseLeave, nullptr);
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

		EventProcess(JGUI_ComponentEvent_MouseHover, nullptr);
	}

}

void JGUIComponent::JGUIParentUpdateNotification()
{
	for (auto& child : m_ChildComponents)
	{
		child->JGUIParentUpdateNotification();
	}
	EventProcess(JGUI_ComponentEvent_ParentUpdateNotification, nullptr);
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

//bool JGUIComponent::IsEventLock(EJGUI_ComponentEvents event_type) {
//	return m_EventLockMap[event_type];
//}
//
//
//void JGUIComponent::EventLock(EJGUI_ComponentEvents event_type, EJGUI_ComponentEventLockFlags flag)
//{
//	m_EventLockMap[event_type] = true;
//	if (flag & JGUI_ComponentEventLockFlag_SendParent && GetParent())
//	{
//		GetParent()->EventLock(event_type, (flag & ~JGUI_ComponentEventLockFlag_SendChild));
//	}
//	if (flag & JGUI_ComponentEventLockFlag_SendChild)
//	{
//		for (auto& child : m_ChildComponents) child->EventLock(event_type, (flag & ~JGUI_ComponentEventLockFlag_SendParent));
//	}
//	if (flag & JGUI_ComponentEventLockFlag_Once)
//	{
//		m_OnceEventLockMap.insert(event_type);
//	}
//}
//void JGUIComponent::EventUnLock(EJGUI_ComponentEvents event_type, EJGUI_ComponentEventLockFlags flag)
//{
//	m_EventLockMap[event_type] = false;
//	if (flag & JGUI_ComponentEventLockFlag_SendParent && GetParent())
//	{
//		GetParent()->EventUnLock(event_type, (flag & ~JGUI_ComponentEventLockFlag_SendChild));
//	}
//	if (flag & JGUI_ComponentEventLockFlag_SendChild)
//	{
//		for (auto& child : m_ChildComponents) child->EventUnLock(event_type, (flag & ~JGUI_ComponentEventLockFlag_SendParent));
//	}
//}



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




void JGUIComponent::EventProcess(EJGUI_ComponentEvents event_type,const void* data)
{


	//if (IsEventLock(event_type))
	//{
	//	if (m_OnceEventLockMap.find(event_type) != m_OnceEventLockMap.end())
	//	{
	//		m_OnceEventLockMap.erase(event_type);
	//		EventUnLock(event_type);
	//	}
	//	return;
	//}

	switch (event_type)
	{
	case JGUI_ComponentEvent_Awake:
		JGUIObject::JGUIAwake();
		break;
	case JGUI_ComponentEvent_Start:
		JGUIObject::JGUIStart();
		break;
	case JGUI_ComponentEvent_Tick:
		Tick(*(JGUITickEvent*)data);
		break;
	case JGUI_ComponentEvent_Destroy:
		JGUIObject::JGUIDestroy();
		break;
	case JGUI_ComponentEvent_Resize:
		Resize(*(JGUIResizeEvent*)data);
		break;
	case JGUI_ComponentEvent_MouseMove:
		MouseMove(*(JGUIMouseMoveEvent*)data);
		break;
	case JGUI_ComponentEvent_MouseBtDown:
		MouseBtDown(*(JGUIKeyDownEvent*)data);
		break;
	case JGUI_ComponentEvent_MouseBtUp:
		MouseBtUp(*(JGUIKeyUpEvent*)data);
		break;
	case JGUI_ComponentEvent_MouseLeave:
		MouseLeave();
		break;
	case JGUI_ComponentEvent_MouseHover:
		MouseHover();
		break;
	case JGUI_ComponentEvent_ParentUpdateNotification:
		ParentUpdateNotification();
		break;
	}


}
void JGUIComponent::Init(const std::string& name, JGUIWindow* owner_window)
{
	SetName(name);
	m_OwnerWindow = owner_window;

}

bool JGUIComponent::Interation(EJGUI_ComponentEvents event_type)
{

	auto p = JGUI::GetMousePos(GetOwnerWindow()->GetRootWindowHandle());
	bool result = IsActive() && GetCollider() && GetCollider()->CheckInPoint(p);
	auto flag = m_InteractionFlag;

	return result;
}

JGUIComponent* JGUIComponent::TrackingCanInteractionComponent()
{
	JGUIComponent* com = nullptr;

	auto flag = GetInteractionFlag();
	auto p = JGUI::GetMousePos(GetOwnerWindow()->GetRootWindowHandle());
	if (flag & JGUI_ComponentInteractionFlag_Default &&
		IsActive() && GetCollider() && GetCollider()->CheckInPoint(p))
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