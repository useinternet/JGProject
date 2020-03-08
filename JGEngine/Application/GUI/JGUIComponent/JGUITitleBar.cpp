#include "pch.h"
#include "JGUITitleBar.h"
#include "JGUIPanel.h"
#include "JGUIShape.h"
#include "JGUIButton.h"
#include "JGUICollider.h"
#include "JGUIRectTransform.h"
#include "GUI/JGUIObject/JGUIWindow.h"
#include "GUI/JGUIForm/JGUIForm.h"
#include "JGUITab.h"
#include "GUI/JGUIForm/JGUIForm.h"
#include "JGUIDock.h"
using namespace std;
#define Default_Bt_Size 20


void JGUITitleBar::Awake()
{
	RegisterCollider(GetOwnerWindow(), JGUI_Collider_Box);

	m_Panel = CreateJGUIComponent<JGUIPanel>("JGUIPanel");
	m_CloseBt = CreateJGUIComponent<JGUICloseButton>("JGUICloseBt");
	m_MaxBt = CreateJGUIComponent<JGUIMaximizeButton>("JGUIMaxBt");
	m_MinBt = CreateJGUIComponent<JGUIMinimizeButton>("JGUIMinBt");
	m_TitleText = CreateJGUIComponent<JGUITextMesh>("JGUITitleText");

	m_CloseBt->SetParent(m_Panel);
	m_MaxBt->SetParent(m_Panel);
	m_MinBt->SetParent(m_Panel);
	m_TitleText->SetParent(m_Panel);
	SettingElement();
}

void JGUITitleBar::Resize(const JGUIResizeEvent& e)
{
	SettingElement();

}

void JGUITitleBar::MouseBtDown(const JGUIKeyDownEvent& e)
{
	if (!m_IsGrap)
	{
		if (GetOwnerWindow()->GetParent())
		{
			auto dock = ((JGUIForm*)(GetOwnerWindow()->GetParent()))->GetDock();
			if (dock)
			{
				dock->RequestRemoveBinedDock(GetOwnerWindow());
			}

			GetOwnerWindow()->SetParent(nullptr);
		}



		m_IsGrap = true;
		
		auto mouse_pos = JGUI::GetCursorPos();
	
		JVector2 window_pos;
		window_pos = GetOwnerWindow()->GetTransform()->GetPosition();
		m_Delta.x = (int)window_pos.x - mouse_pos.x;
		m_Delta.y = (int)window_pos.y - mouse_pos.y;
		JGUI::RegisterDraggingWindow(GetOwnerWindow());
		JGUIExtraEvent e;
		e.Bind(JGUI_EXTRAEVENT_REPEAT, this, [&](JGUIExtraEvent& e) {
			if (m_IsGrap && !m_IsUnConnect)
			{
				auto mouse_pos = JGUI::GetCursorPos();
				mouse_pos.x += m_Delta.x;
				mouse_pos.y += m_Delta.y;
				JVector2 pos((float)mouse_pos.x, (float)mouse_pos.y);
				GetOwnerWindow()->GetTransform()->SetLocalPosition(pos);
			}


	/*		if (GetOwnerWindow()->GetParent() == nullptr)
			{
				TabBinding();
			}*/


			if (JGUI::GetKeyUp(GetOwnerWindow(), KeyCode::LeftMouseButton) || m_IsUnConnect)
			{
				if (m_IsGrap)
				{
					JGUI::RegisterDraggingWindow(nullptr);
					m_IsGrap = false;
					e.flag = JGUI_EXTRAEVENT_EXIT;
				}
				if (m_IsUnConnect)
					m_IsUnConnect = false;
			}
		});
		JGUI::RegisterExtraEvent(e);
	}

}

void JGUITitleBar::MouseBtUp(const JGUIKeyUpEvent& e)
{
	// if(m_IsGrap) m_IsGrap = false;
}

//void JGUITitleBar::Connect(const JVector2Int& delta)
//{
//	if (!m_IsGrap)
//	{
//		m_IsGrap = true;
//		m_Delta = delta;
//		JGUIExtraEvent e;
//		e.Bind(JGUI_EXTRAEVENT_REPEAT, this, [&](JGUIExtraEvent& e) {
//			if (m_IsGrap)
//			{
//				auto mouse_pos = JGUI::GetCursorPos();
//				mouse_pos.x += m_Delta.x;
//				mouse_pos.y += m_Delta.y;
//				JVector2 pos((float)mouse_pos.x, (float)mouse_pos.y);
//				GetOwnerWindow()->GetTransform()->SetLocalPosition(pos);
//			}
//		/*	bool is_tab_binding = false;
//			if (GetOwnerWindow()->GetParent() == nullptr)
//			{
//				is_tab_binding = TabBinding();
//			}*/
//
//			if (JGUI::GetKeyUp(GetOwnerWindow(), KeyCode::LeftMouseButton))
//			{
//				if (m_IsGrap)
//				{
//					m_IsGrap = false;
//					e.flag = JGUI_EXTRAEVENT_EXIT;
//				}
//			}
//		});
//		JGUI::RegisterExtraEvent(e);
//	}
//}

void JGUITitleBar::UnConnect()
{
	m_IsUnConnect = true;
}

//bool JGUITitleBar::TabBinding()
//{
//	JGUIWindow* nextWin = JGUI::GetTopJGUIWindow();
//	JVector2Int cursor_pos = JGUI::GetCursorPos();
//	bool is_find = false;
//	while (nextWin != nullptr)
//	{
//		nextWin = JGUI::GetNextJGUIWindow(nextWin);
//		if (nextWin)
//		{
//			auto win_mouse_pos = nextWin->GetMousePos();
//			if (nextWin->GetPanel()->GetCollider()->CheckInPoint(win_mouse_pos))
//			{
//				if (nextWin != GetOwnerWindow())
//				{
//
//					auto tab = ((JGUIForm*)nextWin)->GetTab();
//					if (tab == nullptr) continue;
//					auto collider = ((JGUIForm*)nextWin)->GetTab()->GetCollider();
//
//					if (collider->CheckInPoint(win_mouse_pos))
//					{
//						is_find = true;
//						((JGUIForm*)nextWin)->GetTab()->BindWindow(GetOwnerWindow());
//
//						::SetFocus(nextWin->GetRootWindowHandle());
//						((JGUIForm*)nextWin)->GetTab()->Connect(nextWin);
//						break;
//					}
//				}
//
//				break;
//			}
//		}
//		
//	}
//
//
//
//	return is_find;
//
//}


void JGUITitleBar::SettingElement()
{
	auto size = GetOwnerWindow()->GetTransform()->GetSize();


	GetTransform()->SetSize(size.x, Default_Bt_Size);


	m_TitleText->SetTextRect(size.x, Default_Bt_Size);
	m_TitleText->SetFontSize(Default_Bt_Size - 4.0f);
	m_TitleText->SetText(GetOwnerWindow()->GetName());
	m_TitleText->GetTransform()->SetLocalPosition(JGUI::Gap(), 0.0f);
	m_Panel->GetTransform()->SetSize(size.x, Default_Bt_Size);
	m_Panel->SetColor(JColor(0.3f, 0.3f, 0.3f, 1.0f));
	m_CloseBt->GetTransform()->SetSize(Default_Bt_Size, Default_Bt_Size);
	m_MaxBt->GetTransform()->SetSize(Default_Bt_Size, Default_Bt_Size);
	m_MinBt->GetTransform()->SetSize(Default_Bt_Size, Default_Bt_Size);




	float pos = GetTransform()->GetSize().x;
	float btsize = Default_Bt_Size;
	float gap = JGUI::Gap();

	pos -= (btsize + gap);
	m_CloseBt->GetTransform()->SetLocalPosition(pos, 0);
	pos -= (btsize + gap);
	m_MaxBt->GetTransform()->SetLocalPosition(pos, 0);
	pos -= (btsize + gap);
	m_MinBt->GetTransform()->SetLocalPosition(pos, 0);
}