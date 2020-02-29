

#include "pch.h"
#include "JGUIResizeBox.h"
#include "JGUIShape.h"
#include "JGUIRectTransform.h"
#include "JGUICollider.h"

#include "GUI/JGUIObject/JGUIWindow.h"


void JGUIResizeBox::PositionAdjustment()
{
	auto window_size = GetOwnerWindow()->GetTransform()->GetSize();
	auto size = GetTransform()->GetSize();
	GetTransform()->SetLocalPosition(window_size.x - size.x, window_size.y - size.y);
}

void JGUIResizeBox::Awake()
{

	RegisterCollider(GetOwnerWindow(), JGUI_Collider_Box);
	m_Rectangle = CreateJGUIComponent<JGUIRectangle>("ResizeRectangle");
	m_Rectangle->SetColor(JColor(0.0f, 0.0f, 0.0f, 1.0f));
	GetTransform()->AttachTransform(m_Rectangle->GetTransform());
	PositionAdjustment();
}

void JGUIResizeBox::Tick(const JGUITickEvent& e)
{
	if (GetCollider()->CheckInPoint(GetOwnerWindow()->GetMousePos()) || m_Resizing)
	{
		m_Rectangle->SetColor(JColor(1.0f, 0.0f, 0.0f, 1.0f));
	}
	else
	{
		m_Rectangle->SetColor(JColor(0.0f, 0.0f, 0.0f, 1.0f));
	}
}

void JGUIResizeBox::Resize(const JGUIResizeEvent& e)
{
	PositionAdjustment();
}

void JGUIResizeBox::MouseBtDown(const JGUIKeyDownEvent& e)
{
	if (m_Resizing == false)
	{
		m_Resizing = true;
		m_TempMousePos = JGUI::GetCursorPos();
		CheckCursorDirection();
		JGUIExtraEvent extra_event;
		extra_event.Bind(JGUI_EXTRAEVENT_REPEAT, this, [&](JGUIExtraEvent& e)
		{
			ResizeByDirection();
			if (m_Resizing && JGUI::GetKeyUp(GetOwnerWindow(), KeyCode::LeftMouseButton))
			{
				m_Resizing = false;
				m_Rectangle->SetColor(JColor(0.0f, 0.0f, 0.0f, 1.0f));
				e.flag = JGUI_EXTRAEVENT_EXIT;
			}
		});

		JGUI::RegisterExtraEvent(extra_event);
	}
}
void JGUIResizeBox::CheckCursorDirection()
{
	auto mouse_pos = GetOwnerWindow()->GetMousePos();
	auto pos = GetTransform()->GetPosition();
	auto size = GetTransform()->GetSize();


	int flag = 0;
	float deltaX = mouse_pos.x - pos.x;
	float deltaY = mouse_pos.y - pos.y;
	if (deltaX <= m_Thick + m_Gap && deltaX >= 0.0f)
	{
		flag |= 1; 
	}
	deltaX = (pos.x + size.x) - mouse_pos.x;
	if (deltaX <= m_Thick + m_Gap && deltaX >= 0.0f)
	{
		flag |= 2;
	}

	if (deltaY <= m_Thick + m_Gap && deltaY >= 0.0f)
	{
		flag |= 4;
	}
	deltaY = (pos.y + size.y) - mouse_pos.y;
	if (deltaY <= m_Thick + m_Gap && deltaY >= 0.0f)
	{
		flag |= 8;
	}

	m_Direction = (EResizeDirection)flag;
}

void JGUIResizeBox::ResizeByDirection()
{
	auto mouse_pos = JGUI::GetCursorPos();

	if (mouse_pos != m_TempMousePos)
	{
		float deltaX = (float)(mouse_pos.x - m_TempMousePos.x);
		float deltaY = (float)(mouse_pos.y - m_TempMousePos.y);
		GetOwnerWindow()->GetTransform()->OffsetSize(deltaX, deltaY);

		//switch (m_Direction)
		//{
		//case UP_NS:
		//	GetOwnerWindow()->GetTransform()->OffsetSize(0.0f, -deltaY);
		//	GetOwnerWindow()->GetTransform()->OffsetLocalPosition(0.0f, deltaY);
		//	break;
		//case DOWN_NS:
		//	GetOwnerWindow()->GetTransform()->OffsetSize(0.0f, deltaY);
		//	break;
		//case RIGHT_WE:
		//	GetOwnerWindow()->GetTransform()->OffsetSize(deltaX, 0.0f);
		//	break;
		//case LEFT_WE:
		//	GetOwnerWindow()->GetTransform()->OffsetSize(-deltaX, 0.0f);
		//	GetOwnerWindow()->GetTransform()->OffsetLocalPosition(deltaX, 0.0f);
		//	break;
		//case RIGHTUP_NE:
		//	GetOwnerWindow()->GetTransform()->OffsetSize(deltaX, -deltaY);
		//	GetOwnerWindow()->GetTransform()->OffsetLocalPosition(0.0f, deltaY);
		//	break;
		//case RIGHTDOWN_SE:
		//
		//	break;
		//case LEFTUP_NW:
		//	GetOwnerWindow()->GetTransform()->OffsetSize(-deltaX, -deltaY);
		//	GetOwnerWindow()->GetTransform()->OffsetLocalPosition(deltaX, deltaY);
		//	break;
		//case LEFTDOWN_SW:
		//	GetOwnerWindow()->GetTransform()->OffsetSize(-deltaX, deltaY);
		//	GetOwnerWindow()->GetTransform()->OffsetLocalPosition(deltaX, 0.0f);
		//	break;
		//}
		m_TempMousePos = mouse_pos;
	}
}
