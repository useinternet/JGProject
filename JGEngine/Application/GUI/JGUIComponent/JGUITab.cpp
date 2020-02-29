#include "pch.h"
#include "JGUITab.h"
#include "JGUIShape.h"
#include "JGUIRectTransform.h"
#include "JGUIButton.h"
#include "JGUIText.h"
#include "JGUIPanel.h"
#include "GUI/JGUIObject/JGUIWindow.h"
#include "GUI/JGUIForm/JGUIForm.h"

using namespace std;

void JGUITab::Awake()
{
	RegisterCollider(GetOwnerWindow(), JGUI_Collider_Box);
	m_Background = CreateJGUIComponent<JGUIRectangle>("TabBackground");
	GetTransform()->AttachTransform(m_Background->GetTransform());


	m_ClientPanel = CreateJGUIComponent<JGUIPanel>("ClientPanel");
	m_ClientPanel->SetParent(GetOwnerWindow()->GetPanel());
}

void JGUITab::Start()
{
	auto owner_form = (JGUIForm*)GetOwnerWindow();
	auto rect = owner_form->GetClientRect();
	m_ClientPanel->GetTransform()->SetLocalPosition(rect.left, rect.top);
	m_ClientPanel->GetTransform()->SetSize(rect.width(), rect.height());
	m_ClientPanel->SetColor(JColor(0.0f, 0.0f, 0.0f, 0.0f));
}

void JGUITab::Resize(const JGUIResizeEvent& e)
{
	auto owner_form = (JGUIForm*)GetOwnerWindow();
	auto rect = owner_form->GetClientRect();
	m_ClientPanel->GetTransform()->SetLocalPosition(rect.left, rect.top);
	m_ClientPanel->GetTransform()->SetSize(rect.width(), rect.height());
	m_ClientPanel->SetColor(JColor(0.0f, 0.0f, 0.0f, 0.0f));





	for (auto& tab_info : m_TabInfo)
	{




	}

}

void JGUITab::Tick(const JGUITickEvent& e)
{
}

void JGUITab::MouseMove(const JGUIMouseMoveEvent& e)
{
	if (m_PressedTabBt)
	{
		auto mouse_pos = GetOwnerWindow()->GetMousePos();
		// 버튼 위치 바꾸기
		// 마우스 위치
		m_PressedTabBt->SetPriority(1);












	}



}

void JGUITab::MouseLeave()
{
	//ENGINE_LOG_INFO("MouseLeave from TabBar");
	if (m_PressedTabBt)
	{
		JVector2 tab_bt_pos;
		JVector2 tab_bt_size;
		JGUIWindow* win = m_PressedTabBt->m_BindedWindow;
		for (auto iter = m_TabInfo.begin(); iter < m_TabInfo.end();)
		{
			if (iter->win == win)
			{
				tab_bt_pos  = iter->bt->GetTransform()->GetPosition();
				tab_bt_size = iter->bt->GetTransform()->GetSize();
				ENGINE_LOG_INFO("Delete Bt");
				iter->bt->m_BindedWindow = nullptr;
				iter->bt->m_OwnerTab = nullptr;
				DestroyJGUIComponent(iter->bt);
				DestroyJGUIComponent(iter->txt);
				m_PressedTabBt = nullptr;
				iter = m_TabInfo.erase(iter);
				break;
			}
			else ++iter;
		}
		// origin
		win->SetParent(nullptr);
		auto flag = win->GetWindowFlags();
		flag = (flag | JGUI_WindowFlag_TitileBar);
		win->SetWindowFlags(flag);

		// testcode
		JVector2Int win_mouse_pos;
		//win_mouse_pos = GetOwnerWindow()->GetMousePos();
		JVector2 win_size = win->GetTransform()->GetSize();
		// origin
		win_mouse_pos = JGUI::GetCursorPos();

		float win_pos_x = 0.0f;
		float win_pos_y = 0.0f;
		// 여기서 탭 버튼과 마우스 위치 계산 후 윈도우 위치 조절
		{
			float width_ratio = (m_MousePosWhenPressed.x - tab_bt_pos.x) / tab_bt_size.x;
			float height_ratio = (m_MousePosWhenPressed.y - tab_bt_pos.y) / tab_bt_size.y;

			win_pos_x = win_mouse_pos.x - (win_size.x * width_ratio * 0.5f);
			win_pos_y = win_mouse_pos.y - (20 * height_ratio);
			// 
		}
		win->GetTransform()->SetLocalPosition(win_pos_x, win_pos_y);
		win->SetActive(true);
		// testcode
		//GetOwnerWindow()->SetFocusWindow(win);
		ReAdjustTabBtPos();
	}
}

void JGUITab::Destroy()
{
	DestroyJGUIComponent(m_ClientPanel);
}

void JGUITab::BindWindow(JGUIWindow* win)
{
	win->SetWindowFlags(JGUI_WindowFlag_MenuBar);
	int tab_count = (int)m_TabInfo.size();

	float bt_x_offset = 0.0f;
	for (auto& info : m_TabInfo)
	{
		bt_x_offset += info.bt->GetTransform()->GetSize().x;
	}


	TabInfo info;
	auto size = GetTransform()->GetSize();

	info.win = win;
	info.txt = CreateJGUIComponent<JGUIText>("TabText");
	info.bt  = CreateJGUIComponent<JGUITabButton>("TabBt");

	info.txt->SetParent(info.bt);


	

	float txt_width = JGUIText::GetTextWidth(info.txt->GetFontName(), win->GetName(),
		info.txt->GetTextSize()) + (4* JGUI::Gap());

	info.txt->SetColor(JColor(0.0f, 0.0f, 0.0f, 1.0f));
	info.txt->GetTransform()->SetSize(txt_width, size.y);
	info.txt->SetHAlign(JGUI_Text_HAlignment_Center);
	info.bt->m_BindedWindow = win;
	info.bt->m_OwnerTab = this;
	info.bt->GetTransform()->SetSize(txt_width, size.y);
	info.bt->GetTransform()->SetLocalPosition(bt_x_offset, 0.0F);
	
	info.txt->SetText(win->GetName());
	win->SetParent(GetOwnerWindow());

	auto client_pos = m_ClientPanel->GetTransform()->GetPosition();
	win->GetTransform()->SetLocalPosition((float)(int)client_pos.x, (float)(int)client_pos.y);
	win->GetTransform()->SetSize(m_ClientPanel->GetTransform()->GetSize());



	m_TabInfo.push_back(info);
	ActiveTab(win);
}

void JGUITab::ActiveTab(JGUIWindow* win)
{
	for (auto& info : m_TabInfo)
	{
		if (info.win == win)
		{
			info.bt->SetBtColor(JGUI_BtState_Normal, JColor(0.0f, 0.0f, 1.0f, 1.0f));
			info.win->SetActive(true);
		}
		else
		{
			info.bt->SetBtColor(JGUI_BtState_Normal, JColor(1.0f, 1.0f, 1.0f, 1.0f));
			info.win->SetActive(false);
		}
	}
}

void JGUITab::ReAdjustTabBtPos()
{
	auto size = GetTransform()->GetSize();
	float bt_x_offset = 0.0f;
	for (auto& info : m_TabInfo)
	{
		info.bt->GetTransform()->SetLocalPosition(bt_x_offset, 0.0F);
		bt_x_offset += info.bt->GetTransform()->GetSize().x;
	}
}

void JGUITabButton::Awake()
{
	JGUIButton::Awake();
	for (EJGUIBtState i = EJGUIBtState::JGUI_BtState_Normal; i < EJGUIBtState::JGUI_BtState_Count;)
	{
		switch (i)
		{
		case EJGUIBtState::JGUI_BtState_Normal:
		case EJGUIBtState::JGUI_BtState_Disabled:
			SetBtColor(i, JColor(1.0f, 1.0f, 1.0f, 1.0f));
			break;
		case EJGUIBtState::JGUI_BtState_Pressed:
		case EJGUIBtState::JGUI_BtState_Hightlight:
		case EJGUIBtState::JGUI_BtState_Selected:
			SetBtColor(i, JColor(0.0f, 0.0f, 1.0f, 1.0f));
			break;
		}
		i = (EJGUIBtState)((int)i + 1);
	}
}

void JGUITabButton::OnClick()
{
	m_OwnerTab->ActiveTab(m_BindedWindow);
}

void JGUITabButton::MouseBtUp(const JGUIKeyUpEvent& e)
{
	JGUIButton::MouseBtUp(e);
	if (m_OwnerTab && m_OwnerTab->m_PressedTabBt)
	{
		m_OwnerTab->m_PressedTabBt = nullptr;
	}
	//ENGINE_LOG_INFO("TabButton Released");
}

void JGUITabButton::MouseBtDown(const JGUIKeyDownEvent& e)
{
	JGUIButton::MouseBtDown(e);
	if (m_BindedWindow && m_OwnerTab && m_OwnerTab->m_PressedTabBt == nullptr)
	{
		ENGINE_LOG_INFO("Register Released");
		m_OwnerTab->m_PressedTabBt = this;
		m_OwnerTab->m_MousePosWhenPressed = GetOwnerWindow()->GetMousePos();
		JGUIExtraEvent extra_e;
		extra_e.Bind(JGUI_EXTRAEVENT_REPEAT, this, [&](JGUIExtraEvent& e)
		{
			if (JGUI::GetKeyUp(GetOwnerWindow(), KeyCode::LeftMouseButton) && m_OwnerTab->m_PressedTabBt)
			{
				m_OwnerTab->m_PressedTabBt = nullptr;
				e.flag = JGUI_EXTRAEVENT_EXIT;
			}
			
		});

		JGUI::RegisterExtraEvent(extra_e);
	}
	// Tab버튼을 누르고 있으면 언제 윈도우창으로 바뀌냐!
	// JGUITab 밖으로 나가면

}
