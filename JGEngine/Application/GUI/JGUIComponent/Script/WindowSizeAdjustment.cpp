#include "pch.h"
#include "WindowSizeAdjustment.h"
#include "GUI/JGUIComponent/Element/JGUIRenderer.h"
#include "GUI/JGUIObject/JGUIWindow.h"
using namespace std;

void WindowSizeAdjustment::Start()
{
	m_Image = GetOwner()->FindComponent<JGUIImageRenderer>();

	if (m_Image)
	{
		Setting();
	}
}

void WindowSizeAdjustment::Destroy()
{

}

void WindowSizeAdjustment::Tick(float tick)
{
	if (m_Image == nullptr)
	{
		m_Image = GetOwner()->FindComponent<JGUIImageRenderer>();
		if (m_Image) Setting();
		else return;
	}

	if (GetOwner()->GetCollider() == nullptr) return;
	if (GetOwner()->GetCollider()->CheckInPoint(GetOwner()->GetOwnerWindow()->GetMousePos()) || m_IsResizing)
	{
		m_Image->SetColor(JColor::Red());
	}
	else
	{
		m_Image->SetColor(JColor::White());
	}


}

void WindowSizeAdjustment::PositionAdjustment()
{
	auto window_size = GetOwner()->GetOwnerWindow()->GetTransform()->GetSize();
	auto size = GetOwner()->GetTransform()->GetSize();
	float offset = 0.0f;
	if (GetOwner()->GetOwnerWindow()->GetWindowFlags() & JGUI_WindowFlag_Border)
	{
		offset = 1.0f;
	}
	GetOwner()->GetTransform()->SetLocalPosition(window_size.x - size.x - offset, window_size.y - size.y - offset);
}

void WindowSizeAdjustment::Setting()
{

	if (GetOwner()->GetCollider() == nullptr)
	{
		GetOwner()->RegisterCollider(JGUI_Collider_Box);
	}
	m_Image->SetColor(JColor::White());
	GetOwner()->BindResizeFunc([&](const JGUIResizeEvent& e)
	{
		PositionAdjustment();
	});


	GetOwner()->BindMouseBtDownFunc([&](const JGUIKeyDownEvent& e)
	{
		if (m_IsResizing == false && e.Code == KeyCode::LeftMouseButton)
		{
			m_IsResizing = true;
			m_PrevCurosrPos = JGUI::GetCursorPos();
			JGUIExtraEvent extra_event;
			extra_event.Bind(JGUI_EXTRAEVENT_REPEAT, GetOwner(), [&](JGUIExtraEvent& e)
			{
				auto cursor_pos = JGUI::GetCursorPos();

				if (cursor_pos != m_PrevCurosrPos)
				{
					float deltaX = (float)(cursor_pos.x - m_PrevCurosrPos.x);
					float deltaY = (float)(cursor_pos.y - m_PrevCurosrPos.y);
					GetOwner()->GetOwnerWindow()->GetTransform()->OffsetSize(deltaX, deltaY);
					m_PrevCurosrPos = cursor_pos;
				}
				if (m_IsResizing && JGUI::GetKeyUp(GetOwner()->GetOwnerWindow(), KeyCode::LeftMouseButton))
				{
					m_IsResizing = false;
					m_Image->SetColor(JColor::White());
					e.flag = JGUI_EXTRAEVENT_EXIT;
				}
			});

			JGUI::RegisterExtraEvent(extra_event);
		}


	});
}
