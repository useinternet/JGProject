#include "pch.h"
#include "JGUITitleBar.h"
#include "JGUIPanel.h"
#include "JGUIShape.h"
#include "JGUIButton.h"
#include "JGUIText.h"
#include "GUI/JGUIObject/JGUIWindow.h"
#include "GUI/JGUIForm/JGUIForm.h"
#include "GUI/JGUIForm/JGUIForm.h"
#include "JGUIDock.h"
using namespace std;
#define Default_Bt_Size 20


void JGUITitleBar::Awake()
{
	JGUIElement::Awake();
	RegisterCollider(JGUI_Collider_Box);

	m_Panel = CreateJGUIElement<JGUIPanel>("JGUIPanel");
	SettingTitleElementByWindowFlag();


	JGUIExtraEvent e;
	e.Bind(JGUI_EXTRAEVENT_REPEAT, this, [&](JGUIExtraEvent& e) {
		ArrangeTab();
		if (!IsActive()) return;
		if (JGUI::GetKeyUp(GetOwnerWindow(), KeyCode::LeftMouseButton))
		{
			BindWindowByCursorPos();
		}
		DraggingTabBtExtraEvent();
	});
	JGUI::RegisterExtraEvent(e);

	BindWindow((JGUIWindow*)GetOwnerWindow());
}

void JGUITitleBar::Resize(const JGUIResizeEvent& e)
{
	SettingTitleElement();
	auto rect = GetOwnerWindow()->GetClientRect();
	for (auto& tabinfo : m_TabInfos)
	{
		if (tabinfo->win == GetOwnerWindow()) continue;
		tabinfo->win->GetTransform()->SetSize(rect.width(), rect.height());
		tabinfo->win->GetTransform()->SetLocalPosition(rect.left, rect.top);
	}
	ArrangeBtSizeByTabCount();
}

void JGUITitleBar::MouseBtDown(const JGUIKeyDownEvent& e)
{
	TitleMouseBtDown(e);

}

void JGUITitleBar::MouseBtUp(const JGUIKeyUpEvent& e)
{

}

void JGUITitleBar::Tick(const JGUITickEvent& e)
{
	m_DraggingWindow = JGUI::GetCurrentDraggingWindow();
	TabTick(e);
}


void JGUITitleBar::UnConnect()
{
	m_IsUnConnect = true;
}

void JGUITitleBar::Connect(const JVector2Int& delta)
{
	JGUIKeyDownEvent e;
	e.Code = KeyCode::LeftMouseButton;
	TitleMouseBtDown(e);
	m_Delta = delta;
}



void JGUITitleBar::SettingTitleElementByWindowFlag()
{
	auto win_flags = GetOwnerWindow()->GetWindowFlags();

	//닫기 버튼 활성화
	if (win_flags & JGUI_WindowFlag_EnableClose)
	{
		if (m_CloseBt == nullptr)
		{
			m_CloseBt = CreateJGUIElement<JGUICloseButton>("JGUICloseBt");
			m_CloseBt->SetParent(m_Panel);
		}
		 
	}
	else
	{
		if (m_CloseBt)
		{
			DestroyJGUIElement(m_CloseBt);
			m_CloseBt = nullptr;
		}
	}
	// 최대화 버튼 활성화
	if (win_flags & JGUI_WindowFlag_EnableMaximize)
	{
		if (m_MaxBt == nullptr)
		{
			m_MaxBt = CreateJGUIElement<JGUIMaximizeButton>("JGUIMaxBt");
			m_MaxBt->SetParent(m_Panel);
		}
	}
	else
	{
		if (m_MaxBt)
		{
			DestroyJGUIElement(m_MaxBt);
			m_MaxBt = nullptr;
		}
	}
	// 최소화 버튼 활성화
	if (win_flags & JGUI_WindowFlag_EnableMinimize)
	{
		if (m_MinBt == nullptr)
		{
			m_MinBt = CreateJGUIElement<JGUIMinimizeButton>("JGUIMinBt");
			m_MinBt->SetParent(m_Panel);
		}
	}
	else
	{
		if (m_MinBt)
		{
			DestroyJGUIElement(m_MinBt);
			m_MinBt = nullptr;
		}
	}
	SettingTitleElement();
}

void JGUITitleBar::SettingTitleElement()
{
	auto size = GetOwnerWindow()->GetTransform()->GetSize();
	GetTransform()->SetSize(size.x, Default_Bt_Size);

	m_TitleSpareWidth = 0.0f;
	m_Panel->GetTransform()->SetSize(size.x, Default_Bt_Size);
	m_Panel->SetColor(JColor(0.3f, 0.3f, 0.3f, 1.0f));
	float pos = GetTransform()->GetSize().x;
	float btsize = Default_Bt_Size;
	float gap = JGUI::Gap();
	if (m_CloseBt)
	{
		m_CloseBt->GetTransform()->SetSize(Default_Bt_Size, Default_Bt_Size);
		pos -= (btsize + gap);
		m_CloseBt->GetTransform()->SetLocalPosition(pos, 0);
		m_TitleSpareWidth += Default_Bt_Size;
	}
	if (m_MaxBt)
	{
		m_MaxBt->GetTransform()->SetSize(Default_Bt_Size, Default_Bt_Size);
		pos -= (btsize + gap);
		m_MaxBt->GetTransform()->SetLocalPosition(pos, 0);
		m_TitleSpareWidth += Default_Bt_Size;
	}
	if (m_MinBt)
	{
		m_MinBt->GetTransform()->SetSize(Default_Bt_Size, Default_Bt_Size);
		pos -= (btsize + gap);
		m_MinBt->GetTransform()->SetLocalPosition(pos, 0);
		m_TitleSpareWidth += Default_Bt_Size;
	}
	m_TitleSpareWidth = size.x - m_TitleSpareWidth;
}


void JGUITitleBar::TitleMouseBtDown(const JGUIKeyDownEvent& e)
{
	auto win_flag = GetOwnerWindow()->GetWindowFlags();
	if (!m_IsGrap && e.Code == KeyCode::LeftMouseButton && (win_flag & JGUI_WindowFlag_NoMove) == false)
	{
		if (GetOwnerWindow()->GetParent())
		{
			// TESTCODE
			/*auto dock = GetOwnerWindow()->GetParent()->GetDock();
			if (dock)
			{
				dock->RequestRemoveBinedDock(GetOwnerWindow());
			}*/

			GetOwnerWindow()->SetParent(nullptr);
			if (GetOwnerWindow()->GetParent())
			{
				GetOwnerWindow()->GetParent()->SetFocusWindow(GetOwnerWindow());
			}
		}
		m_IsGrap = true;

		auto mouse_pos = JGUI::GetCursorPos();

		JVector2 window_pos;
		window_pos = GetOwnerWindow()->GetTransform()->GetPosition();
		m_Delta.x = (int)window_pos.x - mouse_pos.x;
		m_Delta.y = (int)window_pos.y - mouse_pos.y;

		JGUI::RegisterDraggingWindow(GetOwnerWindow());

		
		JGUIExtraEvent extra_e;
		extra_e.Bind(JGUI_EXTRAEVENT_REPEAT, this, [&](JGUIExtraEvent& e)
		{
			if (m_IsGrap && !m_IsUnConnect)
			{
				auto mouse_pos = JGUI::GetCursorPos();
				mouse_pos.x += m_Delta.x;
				mouse_pos.y += m_Delta.y;
				JVector2 pos((float)mouse_pos.x, (float)mouse_pos.y);

				if (GetOwnerWindow()->GetParent())
				{
					auto parent_pos = GetOwnerWindow()->GetParent()->GetTransform()->GetPosition();
				
					pos.x -= parent_pos.x;
					pos.y -= parent_pos.y;
				}
				GetOwnerWindow()->GetTransform()->SetLocalPosition(pos);
			}

			if (JGUI::GetKeyUp(GetOwnerWindow(), KeyCode::LeftMouseButton) || m_IsUnConnect)
			{
				if (m_IsGrap)
				{
					JGUI::RegisterDraggingWindow(nullptr);
					m_IsGrap = false;
					e.flag = JGUI_EXTRAEVENT_EXIT;
				}
				if (m_IsUnConnect) m_IsUnConnect = false;
			}
		});
		JGUI::RegisterExtraEvent(extra_e, 1);
	}
}



void JGUITitleBar::ActiveWindow(JGUIWindow* win)
{
	for (auto& info : m_TabInfos)
	{
		if (info->win == win)
		{
			if (info->win == GetOwnerWindow())
			{
				info->win->SetActiveExceptEssentials(true);
				GetOwnerWindow()->SetFocusWindow(nullptr);
			}
			else
			{
				info->win->SetActive(true);
				GetOwnerWindow()->SetFocusWindow(info->win);
			}
			info->bt->SetBtColor(JGUI_BtState_Normal, m_ActiveColor);
		}
		else
		{
			if (info->win == GetOwnerWindow())
			{
				info->win->SetActiveExceptEssentials(false);
			}
			else
			{
				info->win->SetActive(false);
			}
			info->bt->SetBtColor(JGUI_BtState_Normal, m_UnActiveColor);
		}
	}
}

void JGUITitleBar::BindWindow(JGUIWindow* win)
{
	int tabCount = (int)m_TabInfos.size();
	float offsetX = 0.0f;

	for (auto& info : m_TabInfos)
	{
		if (info->win == win) return;
		auto size = info->bt->GetTransform()->GetSize();
		offsetX += size.x;
	}

	TabInfo* tabInfo = CreateTabInfo(win);
	tabInfo->bt->GetTransform()->SetLocalPosition(offsetX, 0.0f);


	//if (win->GetTitleBar())
	//{
	//	std::vector<JGUIWindow*> wins;
	//	auto& infos = win->GetTitleBar()->m_TabInfos;
	//	int infoCount = (int)infos.size();


	//	for (int i = 1; i < infoCount; ++i)
	//	{
	//		wins.push_back(infos[1]->win);
	//		win->GetTitleBar()->DeleteTab(1);
	//	}
	//	for (int i = 0; i < (int)wins.size(); ++i)
	//	{
	//		BindWindow(wins[i]);
	//	}
	//}

	if (GetOwnerWindow() != win)
	{
		if (GetOwnerWindow()->GetWindowFlags() & JGUI_WindowFlag_EnableDock &&
			win->GetWindowFlags() & JGUI_WindowFlag_EnableDock)
		{
			win->AddWindowFlags(JGUI_WindowFlag_MainTabWindow);
			win->RemoveWindowFlags(JGUI_WindowFlag_EnableResize | JGUI_WindowFlag_TitleBar);
		}
		else
		{
			win->AddWindowFlags(JGUI_WindowFlag_ChildTabWindow);
			win->RemoveWindowFlags(JGUI_WindowFlag_EnableResize | JGUI_WindowFlag_TitleBar);
		}

	}
	ActiveWindow(win);
	ArrangeBtSizeByTabCount();
}

void JGUITitleBar::BindWindowByCursorPos()
{
	if (m_DraggingWindow == nullptr || m_DraggingWindow == GetOwnerWindow())
	{
		return;
	}
	if ((GetOwnerWindow()->GetWindowFlags() & JGUI_WindowFlag_EnableDock) !=
		(m_DraggingWindow->GetWindowFlags() & JGUI_WindowFlag_EnableDock))
	{
		return;
	}

	// testcode
	//if (!GetTransform()->GetRect().Contains(GetOwnerWindow()->GetMousePos()))
	//{
	//	return;
	//}




	//else
	//{
	//	auto mouse_pos = GetOwnerWindow()->GetMousePos();
	//	int idx = GetTabIndex(mouse_pos);
	//	if (idx == -1) return;
	//	bool is_last = false;
	//	if (m_TabInfos.size() == idx)
	//	{
	//		is_last = true;
	//	}
	//	auto titlebar = m_DraggingWindow->GetTitleBar();
	//	if (titlebar) titlebar->UnConnect();
	//	BindWindow(m_DraggingWindow);
	//	if (!is_last)
	//	{
	//		SwapTab(idx, m_TabInfos.size() - 1);
	//	}
	//	else
	//	{
	//		idx = m_TabInfos.size() - 1;
	//	}
	//}
}

void JGUITitleBar::AdjustTabWindowSize()
{
	auto rect = GetOwnerWindow()->GetClientRect();
	for (auto& tabinfo : m_TabInfos)
	{
		if (tabinfo->win == GetOwnerWindow()) continue;
		tabinfo->win->GetTransform()->SetSize(rect.width(), rect.height());
		tabinfo->win->GetTransform()->SetLocalPosition(rect.left, rect.top);
	}
}



JGUITitleBar::TabInfo* JGUITitleBar::CreateTabInfo(JGUIWindow* win)
{
	unique_ptr<TabInfo> tabinfo = make_unique<TabInfo>();
	auto result = tabinfo.get();


	tabinfo->bt = CreateJGUIElement<JGUITabButton>("TabButton");
	if (m_TabInfos.empty())
	{
		m_ActiveColor = tabinfo->bt->GetBtColor(JGUI_BtState_Hightlight);
		m_UnActiveColor = tabinfo->bt->GetBtColor(JGUI_BtState_Normal);
	}
	tabinfo->bt->m_BindedWindow = win;
	tabinfo->bt->m_Owner = this;
	tabinfo->win = win;
	tabinfo->txt = CreateJGUIElement<JGUIText>("TabText");
	tabinfo->txt->SetParent(tabinfo->bt);
	tabinfo->origin_name = win->GetName();
	tabinfo->txt->SetText(tabinfo->win->GetName());
	tabinfo->txt->SetVAlign(JGUI_Text_VAlignment_Center);
	tabinfo->txt->SetHAlign(JGUI_Text_HAlignment_Left);
	tabinfo->txt->SetTextSize(m_BtTextSize);
	tabinfo->txt->SetColor(JColor::Black());

	
	tabinfo->bt->GetTransform()->SetSize(m_TabBtWidth, Default_Bt_Size - m_TabLineHeight);
	JVector2 btSize = tabinfo->bt->GetTransform()->GetSize();
	tabinfo->txt->GetTransform()->SetLocalPosition(JGUI::Gap(), 0.0f);
	tabinfo->txt->GetTransform()->SetSize(btSize.x - (JGUI::Gap() * 2), btSize.y);
	m_TabInfos.push_back(move(tabinfo));

	if (win != GetOwnerWindow())
	{
		win->SetParent(GetOwnerWindow());
		auto rect = ((JGUIForm*)GetOwnerWindow())->GetClientRect();
		win->GetTransform()->SetSize(rect.width(), rect.height());
		win->GetTransform()->SetLocalPosition(rect.left, rect.top);
	}

	return result;
}

void JGUITitleBar::TabTick(const JGUITickEvent& e)
{
	
}

void JGUITitleBar::NotifyDraggingTabButton(JGUITabButton* tabBt)
{
	m_CurrentDraggingTabBt = tabBt;
	m_IsDraggingTabBtEvent = true;
}

void JGUITitleBar::DraggingTabBtExtraEvent()
{
	if (!m_IsDraggingTabBtEvent) return;
	if (m_CurrentDraggingTabBt && m_CurrentDraggingTabBt->m_BindedWindow == GetOwnerWindow())
	{
		m_CurrentDraggingTabBt = nullptr;
		m_IsDraggingTabBtEvent = false;
		return;
	}
	JVector2Int mouse_pos = GetOwnerWindow()->GetMousePos();


	auto bt_Rect = m_CurrentDraggingTabBt->GetTransform()->GetRect();
	bool is_contain = mouse_pos.x >= bt_Rect.left && mouse_pos.x <= bt_Rect.right;
	bool is_win_leave = false;

	//
	int tab_index = GetTabIndex(m_CurrentDraggingTabBt);

	int top = bt_Rect.top - 20;
	int bottom = bt_Rect.bottom + 20;

	if (top > mouse_pos.y || bottom < mouse_pos.y)
	{
		auto win = m_CurrentDraggingTabBt->m_BindedWindow;
		auto win_size = win->GetTransform()->GetSize();
		DeleteTab(tab_index);

		int delta_x = (int)(win_size.x * 0.15f);
		int delta_y = 10;




		win->SetParent(nullptr);
		if (win->GetWindowFlags() & JGUI_WindowFlag_EnableDock)
		{
			win->AddWindowFlags(JGUI_WindowFlag_MainWindow);
			win->RemoveWindowFlags(JGUI_WindowFlag_NoMove | JGUI_WindowFlag_BottomMost);
		}
		else
		{
			win->SetWindowFlags(JGUI_WindowFlag_ChildWindow);
			win->RemoveWindowFlags(JGUI_WindowFlag_NoMove | JGUI_WindowFlag_BottomMost);
		}
		
		// testcode
		//((JGUIWindow*)win)->GetTitleBar()->Connect({ -delta_x, -delta_y });

		m_SwapLock = false;
		m_DeltaXDistance = 0;
		is_win_leave = true;

		tab_index = std::max<int>(0, tab_index - 1);
		ActiveWindow(m_TabInfos[tab_index]->win);
	}
	if (!is_contain)
	{
		if (m_SwapLock)
		{
			int current_x_distance = mouse_pos.x - m_CurrentDraggingTabBt->GetTransform()->GetPosition().x;
			// 오른쪽
			if (m_DeltaXDistance < 0)
			{
				if (m_DeltaXDistance - current_x_distance > 5.0f)
				{
					SwapTab(tab_index, tab_index - 1);
				}
			}
			// 왼쪽
			else
			{
				if (current_x_distance - m_DeltaXDistance > 5.0f)
				{
					SwapTab(tab_index, tab_index + 1);
				}
			}
		}
		else
		{
			// 왼쪽으로 이동( 왼쪽으로 이동 후 Rect범위 밖에 있다면 마우스 다시 오른쪽으로 간다면 제자리)
			if (bt_Rect.left > mouse_pos.x)
			{
				// 바뀔때 마우스 좌표를 저장해놨다가 이것보다 delta발생 이면 스왑
				SwapTab(tab_index, tab_index - 1);
				m_SwapLock = true;
			}
			// 오른쪽으로 이동( 오른쪽으로 이동 후 Rect범위 밖에 있다면 마우스 다시 왼쪽으로 간다면 제자리)
			if (bt_Rect.right < mouse_pos.x)
			{
				SwapTab(tab_index, tab_index + 1);
				m_SwapLock = true;
			}
			m_DeltaXDistance = mouse_pos.x - m_CurrentDraggingTabBt->GetTransform()->GetPosition().x;
		}
	}
	else
	{
		m_SwapLock = false;
		m_DeltaXDistance = 0;
	}

	if (JGUI::GetKeyUp(GetOwnerWindow(), KeyCode::LeftMouseButton) || is_win_leave)
	{
		m_CurrentDraggingTabBt = nullptr;
		m_IsDraggingTabBtEvent = false;
	}
}

void JGUITitleBar::ArrangeTab()
{
	if (IsDestroying()) return;
	int cnt = 0;
	vector<int> delete_tab_indexs;
	for (auto& tabinfo : m_TabInfos)
	{
		if (tabinfo->win->IsDestroying() && tabinfo->win != GetOwnerWindow())
		{

			delete_tab_indexs.push_back(cnt);



		}
		++cnt;
	}

	for (auto& idx : delete_tab_indexs)
	{
		DeleteTab(idx);
	}
}

int JGUITitleBar::GetTabIndex(JGUITabButton* bt)
{
	int cnt = 0;
	for (auto& info : m_TabInfos)
	{
		if (info->bt == bt)
			return cnt;
		++cnt;
	}
	return cnt;
}

int JGUITitleBar::GetTabIndex(const JVector2Int& p)
{
	int idx = -1;
	for (int i = 0; i < (int)m_TabInfos.size(); ++i)
	{
		auto tabInfo = m_TabInfos[i].get();
		float hw = tabInfo->bt->GetTransform()->GetSize().x * 0.5f;
		float x  = tabInfo->bt->GetTransform()->GetPosition().x;

		if (x + hw >= p.x) idx = i;
		if (x + hw < p.x && x + hw + hw >= p.x) idx = i + 1;


		if (idx == -1 && i == m_TabInfos.size() - 1 && 
			x + hw + hw <= p.x) idx = m_TabInfos.size();
		if (idx != -1) break;
	}
	if (m_TabInfos.empty())
	{
		if (p.x >= 0) return 0;
		else return -1;
	}
	return idx;
}

void JGUITitleBar::SwapTab(int tn1, int tn2)
{
	if (tn1 < 0 || tn2 < 0) return;
	if (tn1 == tn2) return;
	int tabCount = (int)m_TabInfos.size();
	if (tn1 >= tabCount || tn2 >= tabCount) return;

	if (tn1 > tn2)
	{
		int temp = tn1;
		tn1 = tn2;
		tn2 = temp;
	}


	auto Info1 = m_TabInfos[tn1].get();
	auto Info2 = m_TabInfos[tn2].get();



	auto info2_size = Info2->bt->GetTransform()->GetSize();
	auto info1_pos = Info1->bt->GetTransform()->GetLocalPosition();


	Info2->bt->GetTransform()->SetLocalPosition(info1_pos.x, info1_pos.y);
	Info1->bt->GetTransform()->SetLocalPosition(info1_pos.x + info2_size.x, info1_pos.y);

	auto tempInfo   = move(m_TabInfos[tn2]);
	m_TabInfos[tn2] = move(m_TabInfos[tn1]);
	m_TabInfos[tn1] = move(tempInfo);
}

void JGUITitleBar::DeleteTab(int idx)
{
	if (m_TabInfos.empty() || m_TabInfos.size() == 1) return;
	float offsetX = 0.0f;
	int cnt = 0;
	for (auto& info : m_TabInfos)
	{
		if (cnt != idx)
		{
			info->bt->GetTransform()->SetLocalPosition(offsetX, 0.0f);
			offsetX += info->bt->GetTransform()->GetSize().x;
		}
		++cnt;
	}
	DestroyJGUIElement(m_TabInfos[idx]->txt);
	DestroyJGUIElement(m_TabInfos[idx]->bt);
	auto iter = m_TabInfos.begin() + idx;
	m_TabInfos.erase(iter);
}

void JGUITitleBar::ArrangeBtSizeByTabCount()
{
	if (m_TabInfos.empty()) return;
	float tab_width = 0.0f;
	for (auto& info : m_TabInfos)
	{
		tab_width += info->bt->GetTransform()->GetSize().x;
	}
	float accX = 0.0f;
	float btWidth = (m_TitleSpareWidth - m_TabGap) / m_TabInfos.size();

	if (btWidth < m_TabBtWidth)
	{
		for (auto& info : m_TabInfos)
		{
			info->bt->GetTransform()->SetSize(btWidth, Default_Bt_Size - m_TabLineHeight);
			info->txt->GetTransform()->SetSize(btWidth - (JGUI::Gap() * 2), Default_Bt_Size - m_TabLineHeight);
			info->bt->GetTransform()->SetLocalPosition(accX, 0.0f);
			accX += info->bt->GetTransform()->GetSize().x;
		}
	}
	else
	{
		for (auto& info : m_TabInfos)
		{
			info->bt->GetTransform()->SetSize(
				m_TabBtWidth, Default_Bt_Size - m_TabLineHeight);
			info->txt->GetTransform()->SetSize(
				m_TabBtWidth - (JGUI::Gap() * 2),
				Default_Bt_Size - m_TabLineHeight);
			info->bt->GetTransform()->SetLocalPosition(accX, 0.0f);
			accX += info->bt->GetTransform()->GetSize().x;
		}
	}
}



void JGUITitleBar::JGUITabButton::MouseBtDown(const JGUIKeyDownEvent& e)
{
	JGUIButton::MouseBtDown(e);

	if (m_Owner->m_CurrentDraggingTabBt == nullptr &&
		e.Code == KeyCode::LeftMouseButton)
	{
		m_Owner->ActiveWindow(m_BindedWindow);
		m_Owner->NotifyDraggingTabButton(this);
	}

}


