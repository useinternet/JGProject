#include "pch.h"
#include "JGUICollider.h"
#include "GUI/JGUIObject/JGUIElement.h"
#include "GUI/JGUIObject/JGUIWindow.h"
#include "GUI/JGUIComponent/JGUICamera.h"
#include "GUI/JGUIElement/JGUITitleBar.h"
#include "Element/JGUICanvas.h"

bool JGUICollider::CheckInPoint(const JVector2Int& p)
{
	

	return false;
}

void JGUICollider::Awake()
{
	FindCanvas();
}

void JGUICollider::ChangeParent(JGUIObject* prev_parent)
{
	FindCanvas();
}

void JGUICollider::FindCanvas()
{
	if (m_ElementOwner)
	{
		m_OwnerCanvas = m_ElementOwner->FindComponent<JGUICanvas>();

		if (m_OwnerCanvas == nullptr)
		{
			auto parent = m_ElementOwner->GetParent();
			while (parent != nullptr)
			{
				m_OwnerCanvas = parent->FindComponent<JGUICanvas>();
				if (m_OwnerCanvas)
				{
					break;
				}
				parent = parent->GetParent();
			}
		}
		else
		{
			ENGINE_LOG_ERROR("{0} is not exsit in Canvas", GetName());
		}
	}
}

void JGUIBoxCollider::Awake()
{
	JGUICollider::Awake();
	m_ColiderType = JGUI_Collider_Box;
}
bool JGUIBoxCollider::CheckInPoint(const JVector2Int& p)
{
	if (IsActive() == false) return false;
	JVector2Int mouse_pos = p;
	JGUIRect rect;
	bool is_check = false;
	if (m_ElementOwner && m_ElementOwner->IsActive())
	{
		is_check = true;
		rect = m_ElementOwner->GetTransform()->GetRect();
	}
	if (m_WindowOwner && m_WindowOwner->IsActive())
	{
		is_check = true;
		rect = m_WindowOwner->GetTransform()->GetLocalRect();
	}

	if (m_ElementOwner && m_ElementOwner->GetOwnerWindow()->GetParent())
	{
		auto view_pos = m_ElementOwner->GetOwnerWindow()->GetParent()->GetCamera()->GetPosition();
		mouse_pos.x += view_pos.x;
		mouse_pos.y -= view_pos.y;
		ENGINE_LOG_INFO("MousePos : {0} , {1}", mouse_pos.x, mouse_pos.y);
	}
	if (
		(m_OwnerCanvas && !(m_OwnerCanvas->GetCanvasFlags() & JGUI_CanvasFlag_Overlay)) ||
		(m_OwnerCanvas == nullptr && m_WindowOwner))
	{

		JGUICamera* cam = nullptr;
		if (m_ElementOwner) cam = m_ElementOwner->GetOwnerWindow()->GetCamera();
		if (m_WindowOwner && m_WindowOwner->GetParent())  cam  = m_WindowOwner->GetParent()->GetCamera();
		if (cam)
		{
			JVector2 view_pos = cam->GetPosition();
			rect.left   -= view_pos.x;
			rect.right  -= view_pos.x;
			rect.top    += view_pos.y;
			rect.bottom += view_pos.y;
		}
	}
	if (is_check)
	{
		if (rect.left < mouse_pos.x && rect.top < mouse_pos.y &&
			rect.right > mouse_pos.x&& rect.bottom > mouse_pos.y)
		{
			return true;
		}
	}
	return false;
}
