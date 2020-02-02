

#include "pch.h"
#include "JGUIResizeBox.h"
#include "JGUIShape.h"
#include "JGUIRectTransform.h"
#include "JGUICollider.h"

#include "GUI/JGUIObject/JGUIWindow.h"

void JGUIResizeBox::SetThickness(float thick)
{
	m_Thick = thick;

	m_Rectangle->SetThickness(m_Thick);

	if (GetCollider()->GetColiderType() == JGUI_Component_Colider_EmptyBox)
	{
		((JGUIEmptyBoxColider*)GetCollider())->SetThick(m_Thick);
	}
}

void JGUIResizeBox::Awake()
{
	RegisterCollider(JGUI_Component_Colider_EmptyBox);
	m_Rectangle = CreateJGUIComponent<JGUIRectangle>("ResizeRectangle");
	m_Rectangle->EmptyOn();
	m_Rectangle->SetColor(JColor(0.0f, 0.0f, 0.0f, 1.0f));
	m_Rectangle->GetTransform()->SetSize(GetTransform()->GetSize());
	SetThickness(m_Thick);


	GetTransform()->SetSize(GetOwnerWindow()->GetTransform()->GetSize());


}

void JGUIResizeBox::Resize(const JGUIResizeEvent& e)
{
	m_Rectangle->GetTransform()->SetSize(e.width, e.height);
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
void JGUIResizeBox::MouseMove(const JGUIMouseMoveEvent& e)
{
	m_Rectangle->SetColor(JColor(1.0f, 0.0f, 0.0f, 1.0f));
}


void JGUIResizeBox::MouseLeave()
{
	if (m_Resizing == false)
	{
		m_Rectangle->SetColor(JColor(0.0f, 0.0f, 0.0f, 1.0f));
	}
}

void JGUIResizeBox::CheckCursorDirection()
{
	auto mouse_pos = JGUI::GetMousePos(GetOwnerWindow()->GetRootWindowHandle());
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


		switch (m_Direction)
		{
		case UP_NS:
			GetOwnerWindow()->GetTransform()->OffsetSize(0.0f, -deltaY);
			GetOwnerWindow()->GetTransform()->OffsetPosition(0.0f, deltaY);
			break;
		case DOWN_NS:
			GetOwnerWindow()->GetTransform()->OffsetSize(0.0f, deltaY);
			break;
		case RIGHT_WE:
			GetOwnerWindow()->GetTransform()->OffsetSize(deltaX, 0.0f);
			break;
		case LEFT_WE:
			GetOwnerWindow()->GetTransform()->OffsetSize(-deltaX, 0.0f);
			GetOwnerWindow()->GetTransform()->OffsetPosition(deltaX, 0.0f);
			break;
		case RIGHTUP_NE:
			GetOwnerWindow()->GetTransform()->OffsetSize(deltaX, -deltaY);
			GetOwnerWindow()->GetTransform()->OffsetPosition(0.0f, deltaY);
			break;
		case RIGHTDOWN_SE:
			GetOwnerWindow()->GetTransform()->OffsetSize(deltaX, deltaY);
			break;
		case LEFTUP_NW:
			GetOwnerWindow()->GetTransform()->OffsetSize(-deltaX, -deltaY);
			GetOwnerWindow()->GetTransform()->OffsetPosition(deltaX, deltaY);
			break;
		case LEFTDOWN_SW:
			GetOwnerWindow()->GetTransform()->OffsetSize(-deltaX, deltaY);
			GetOwnerWindow()->GetTransform()->OffsetPosition(deltaX, 0.0f);
			break;
		}
		m_TempMousePos = mouse_pos;
	}
}
