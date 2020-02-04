#include "pch.h"
#include "JGUITitleBar.h"
#include "JGUIPanel.h"
#include "JGUIShape.h"
#include "JGUIButton.h"
#include "JGUICollider.h"
#include "JGUIRectTransform.h"
#include "GUI/JGUIObject/JGUIWindow.h"

using namespace std;
#define Default_Bt_Size 20


void JGUITitleBar::Awake()
{
	RegisterCollider(JGUI_Component_Colider_Box);
	m_Panel = CreateJGUIComponent<JGUIPanel>("JGUIPanel");
	m_CloseBt = CreateJGUIComponent<JGUICloseButton>("JGUICloseBt");
	m_MaxBt = CreateJGUIComponent<JGUIMaximizeButton>("JGUIMaxBt");
	m_MinBt = CreateJGUIComponent<JGUIMinimizeButton>("JGUIMinBt");
	m_TitleText = CreateJGUIComponent<JGUIText>("JGUITitleText");

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
		m_IsGrap = true;
		auto mouse_pos = JGUI::GetCursorPos();
		auto window_pos = GetOwnerWindow()->GetTransform()->GetPosition();
		ENGINE_LOG_INFO("{0}, {1} wINDOW", window_pos.x, window_pos.y);
		ENGINE_LOG_INFO("{0}, {1} Cursor", mouse_pos.x, mouse_pos.y);
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


				auto window_pos = GetOwnerWindow()->GetTransform()->GetPosition();
				ENGINE_LOG_INFO("{0}, {1} move", window_pos.x, window_pos.y);
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
	// if(m_IsGrap) m_IsGrap = false;
}


void JGUITitleBar::SettingElement()
{
	auto size = GetOwnerWindow()->GetTransform()->GetSize();
	size.x -= 6.0f;


	GetTransform()->SetSize(size.x, Default_Bt_Size);


	m_TitleText->SetTextRect(size.x, Default_Bt_Size);
	m_TitleText->SetFontSize(Default_Bt_Size - 4.0f);
	m_TitleText->SetText(GetOwnerWindow()->GetName());
	m_TitleText->GetTransform()->SetPosition(JGUI::Gap(), 0.0f);
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