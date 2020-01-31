#include "pch.h"
#include "JGUITitleBar.h"
#include "JGUIPanel.h"
#include "JGUIButton.h"
#include "JGUICollider.h"
#include "JGUIRectTransform.h"
#include "GUI/JGUIObject/JGUIWindow.h"
using namespace std;
#define Default_Bt_Size 20


void JGUITitleBar::Awake()
{
	RegisterBoxCollider();
	m_Panel = CreateJGUIComponent<JGUIPanel>("JGUIPanel");
	m_CloseBt = CreateJGUIComponent<JGUICloseButton>("JGUICloseBt");
	m_MaxBt = CreateJGUIComponent<JGUIMaximizeButton>("JGUIMaxBt");
	m_MinBt = CreateJGUIComponent<JGUIMinimizeButton>("JGUIMinBt");
	m_MaxBt->SetParent(m_Panel);
	m_MinBt->SetParent(m_Panel);
	m_CloseBt->SetParent(m_Panel);


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
		m_IsGrap = true;
		auto mouse_pos = JGUI::GetCursorPos();
		auto window_pos = GetOwnerWindow()->GetTransform()->GetPosition();

		m_Delta.x = (int)window_pos.x - mouse_pos.x;
		m_Delta.y = (int)window_pos.y - mouse_pos.y;
		JGUIExtraEvent e;
		e.Bind(JGUI_EXTRAEVENT_REPEAT, this, [&](JGUIExtraEvent& e) {
			if (m_IsGrap)
			{
				auto mouse_pos = JGUI::GetCursorPos();
				mouse_pos.x += m_Delta.x;
				mouse_pos.y += m_Delta.y;
				JVector2 pos((float)mouse_pos.x, (float)mouse_pos.y);
				GetOwnerWindow()->GetTransform()->SetPosition(pos);
			}


			if (JGUI::GetKeyUp(GetOwnerWindow(), KeyCode::LeftMouseButton))
			{
				if (m_IsGrap)
				{
					m_IsGrap = false;
					e.flag = JGUI_EXTRAEVENT_EXIT;
				}
			}
		});
		JGUI::RegisterExtraEvent(e);
	}

}

void JGUITitleBar::MouseBtUp(const JGUIKeyUpEvent& e)
{
	if(m_IsGrap) m_IsGrap = false;
}


void JGUITitleBar::SettingElement()
{
	auto size = GetOwnerWindow()->GetTransform()->GetSize();


	GetTransform()->SetSize(size.x, 40);
	m_Panel->GetTransform()->SetSize(size.x, Default_Bt_Size);
	m_Panel->SetColor(JColor(0.3f, 0.3f, 0.3f, 1.0f));
	m_CloseBt->GetTransform()->SetSize(Default_Bt_Size, Default_Bt_Size);
	m_MaxBt->GetTransform()->SetSize(Default_Bt_Size, Default_Bt_Size);
	m_MinBt->GetTransform()->SetSize(Default_Bt_Size, Default_Bt_Size);




	float pos = GetTransform()->GetSize().x;
	float btsize = Default_Bt_Size;
	float gap = JGUI::Gap();

	pos -= (btsize + gap);
	m_CloseBt->GetTransform()->SetPosition(pos, 0);
	pos -= (btsize + gap);
	m_MaxBt->GetTransform()->SetPosition(pos, 0);
	pos -= (btsize + gap);
	m_MinBt->GetTransform()->SetPosition(pos, 0);
}