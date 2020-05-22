#include "pch.h"
#include "TabSystem.h"





#include "GUI/JGUIObject/JGUIWindow.h"
#include "GUI/JGUIObject/JGUIElement.h"

#include "GUI/JGUIComponent/Script/Button.h"
#include "GUI/JGUIComponent/Script/TitleBar.h"
#include "GUI/JGUIComponent/Script/DockSystem.h"
#include "GUI/JGUIComponent/Script/Text.h"

#include "GUI/JGUIComponent/Element/JGUICanvas.h"
#include "GUI/JGUIComponent/Element/JGUIRenderer.h"




using namespace std;
JGUI_REGISTER_COMPONENT(TabSystem)

void TabSystem::Awake()
{
	GetOwner()->BindResizeFunc([&](const JGUIResizeEvent& e)
	{
		auto rect = GetOwnerWindow()->GetClientRect();
		for (auto& tabinfo : m_TabInfos)
		{
			if (tabinfo->win == GetOwnerWindow()) continue;
			tabinfo->win->GetTransform()->SetSize(rect.width(), rect.height());
			tabinfo->win->GetTransform()->SetLocalPosition(rect.left, rect.top);
		}
		ArrangeBtSizeByTabCount();
	});
}

void TabSystem::Start()
{
	BindWindow(GetOwnerWindow());



	JGUIExtraEvent e;
	e.Bind(JGUI_EXTRAEVENT_REPEAT, GetOwner(), [&](JGUIExtraEvent& e) {
		if (GetOwner() == nullptr) return;
		ArrangeTab();
		if (!IsActive()) return;
		auto title_bar = GetOwnerWindow()->GetTitleBar();

		if (JGUI::GetKeyUp(GetOwnerWindow(), KeyCode::LeftMouseButton))
		{
			BindWindowByCursorPos();
		}
		DraggingTabBtExtraEvent();
	});
	JGUI::RegisterExtraEvent(e, 1);
}

void TabSystem::Destroy()
{

}

void TabSystem::Tick(float tick)
{
	m_DraggingWindow = JGUI::GetCurrentDraggingWindow();
	if (m_DraggingWindow == GetOwnerWindow()) m_DraggingWindow = nullptr;

}

void TabSystem::ActiveWindow(JGUIWindow* win)
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
			info->bt->SetColor(Button::Normal, m_ActiveColor);
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
			info->bt->SetColor(Button::Normal, m_UnActiveColor);
		}
	}
}

void TabSystem::BindWindow(JGUIWindow* win)
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

	if (win->GetTabSystem())
	{
		std::vector<JGUIWindow*> wins;
		auto& infos = win->GetTabSystem()->m_TabInfos;
		int infoCount = (int)infos.size();

		for (int i = 1; i < infoCount; ++i)
		{
			wins.push_back(infos[1]->win);
			win->GetTabSystem()->DeleteTab(1);
		}

		for (int i = 0; i < (int)wins.size(); ++i)
		{
			BindWindow(wins[i]);
		}
	}



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

void TabSystem::BindWindowByCursorPos()
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


	if (!GetTransform()->GetRect().Contains(GetOwnerWindow()->GetMousePos()))
	{
		return;
	}
	else
	{
		auto mouse_pos = GetOwnerWindow()->GetMousePos();
		int idx = GetTabIndex(mouse_pos);
		if (idx == -1) return;
		bool is_last = false;
		if (m_TabInfos.size() == idx)
		{
			is_last = true;
		}
		auto titlebar = m_DraggingWindow->GetTitleBar();
		if (titlebar) titlebar->UnConnect();
		BindWindow(m_DraggingWindow);
		if (!is_last)
		{
			SwapTab(idx, m_TabInfos.size() - 1);
		}
		else
		{
			idx = m_TabInfos.size() - 1;
		}
	}

}

void TabSystem::AdjustTabWindowSize()
{
	auto rect = GetOwnerWindow()->GetClientRect();
	for (auto& tabinfo : m_TabInfos)
	{
		if (tabinfo->win == GetOwnerWindow()) continue;
		tabinfo->win->GetTransform()->SetSize(rect.width(), rect.height());
		tabinfo->win->GetTransform()->SetLocalPosition(rect.left, rect.top);
	}
}

std::vector<JGUIWindow*> TabSystem::GetBindedWindow() const
{
	vector<JGUIWindow*> result;
	for (auto& info : m_TabInfos)
	{
		result.push_back(info->win);
	}
	return result;
}

TabSystem::TabInfo* TabSystem::CreateTabInfo(JGUIWindow* win)
{
	unique_ptr<TabInfo> tabinfo = make_unique<TabInfo>();
	auto result = tabinfo.get();
	auto btHeight = GetOwner()->GetTransform()->GetSize().y;

	tabinfo->bt = GetOwner()->CreateJGUIElement("TabButton")->CreateJGUIComponent<TabButton>();
	
	tabinfo->bt->GetOwner()->CreateJGUIComponent<JGUIImageRenderer>();
	tabinfo->bt->m_BindedWindow = win;
	tabinfo->bt->m_OwnerTab = this;
	if (m_TabInfos.empty())
	{
		m_ActiveColor   = tabinfo->bt->GetColor(Button::Hightlight);
		m_UnActiveColor = tabinfo->bt->GetColor(Button::Normal);
	}
	tabinfo->win = win;
	tabinfo->origin_name = win->GetName();



	tabinfo->bt->GetTransform()->SetSize(m_TabBtWidth, btHeight - m_TabLineHeight);
	tabinfo->txt = tabinfo->bt->GetOwner()->CreateJGUIElement("Text")->CreateJGUIComponent<Text>();
	tabinfo->txt->SetColor(JColor::Black());
	tabinfo->txt->SetFontSize(16.0f);
	tabinfo->txt->SetText(tabinfo->origin_name);

	tabinfo->txt->SetHAlign(JGUI_Text_HAlignment_Center);
	tabinfo->txt->SetVAlign(JGUI_Text_VAlignment_Center);
	tabinfo->bt->GetTransform()->AttachTransform(tabinfo->txt->GetTransform());


	JVector2 btSize = tabinfo->bt->GetTransform()->GetSize();
	m_TabInfos.push_back(move(tabinfo));

	if (win != GetOwnerWindow())
	{
		win->SetParent(GetOwnerWindow());
		auto rect = GetOwnerWindow()->GetClientRect();
		win->GetTransform()->SetSize(rect.width(), rect.height());
		win->GetTransform()->SetLocalPosition(rect.left, rect.top);
	}

	return result;
}

void TabSystem::NotifyDraggingTabButton(Button* tabBt)
{
	m_CurrentDraggingTabBt = tabBt;
	//m_IsDraggingTabBtEvent = true;
}

void TabSystem::DraggingTabBtExtraEvent()
{
	if (m_CurrentDraggingTabBt == nullptr) return;
	//if (!m_IsDraggingTabBtEvent) return;

	if (m_CurrentDraggingTabBt && FindWindowByBt(m_CurrentDraggingTabBt) == GetOwnerWindow())
	{
		m_CurrentDraggingTabBt = nullptr;
		//m_IsDraggingTabBtEvent = false;
		return;
	}
	JVector2Int mouse_pos = GetOwnerWindow()->GetMousePos();
	auto bind_window = FindWindowByBt(m_CurrentDraggingTabBt);

	auto bt_Rect = m_CurrentDraggingTabBt->GetTransform()->GetRect();
	bool is_contain = mouse_pos.x >= bt_Rect.left && mouse_pos.x <= bt_Rect.right;
	bool is_win_leave = false;

	//
	int tab_index = GetTabIndex(m_CurrentDraggingTabBt);

	int top = bt_Rect.top - 20;
	int bottom = bt_Rect.bottom + 20;

	if (top > mouse_pos.y || bottom < mouse_pos.y)
	{
		auto win = bind_window;
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
		if (win->GetTitleBar())
		{
			win->GetTitleBar()->Connect({ -delta_x, -delta_y });
		}
	

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
		//m_IsDraggingTabBtEvent = false;
	}
}

void TabSystem::ArrangeTab()
{
	if (GetOwner()->IsDestroying()) return;
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

int TabSystem::GetTabIndex(Button* bt)
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

int TabSystem::GetTabIndex(const JVector2Int& p)
{
	int idx = -1;
	for (int i = 0; i < (int)m_TabInfos.size(); ++i)
	{
		auto tabInfo = m_TabInfos[i].get();
		float hw = tabInfo->bt->GetTransform()->GetSize().x * 0.5f;
		float x = tabInfo->bt->GetTransform()->GetPosition().x;

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

void TabSystem::SwapTab(int tn1, int tn2)
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

	auto tempInfo = move(m_TabInfos[tn2]);
	m_TabInfos[tn2] = move(m_TabInfos[tn1]);
	m_TabInfos[tn1] = move(tempInfo);
}

void TabSystem::DeleteTab(int idx)
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
	GetOwner()->DestroyJGUIElement(m_TabInfos[idx]->bt->GetOwner());
	auto iter = m_TabInfos.begin() + idx;
	m_TabInfos.erase(iter);
}

void TabSystem::ArrangeBtSizeByTabCount()
{
	if (m_TabInfos.empty()) return;
	float tab_width = 0.0f;
	for (auto& info : m_TabInfos)
	{
		tab_width += info->bt->GetTransform()->GetSize().x;
	}
	float accX = 0.0f;
	float btWidth = (GetTransform()->GetSize().x - m_TabGap) / m_TabInfos.size();
	float btHeight = GetTransform()->GetSize().y;
	if (btWidth < m_TabBtWidth)
	{
		for (auto& info : m_TabInfos)
		{
			info->bt->GetTransform()->SetSize(btWidth, btHeight - m_TabLineHeight);
			info->bt->GetTransform()->SetLocalPosition(accX, 0.0f);
			accX += info->bt->GetTransform()->GetSize().x;
		}
	}
	else
	{
		for (auto& info : m_TabInfos)
		{
			info->bt->GetTransform()->SetSize(
				m_TabBtWidth, btHeight - m_TabLineHeight);
			info->bt->GetTransform()->SetLocalPosition(accX, 0.0f);
			accX += info->bt->GetTransform()->GetSize().x;
		}
	}
}

JGUIWindow* TabSystem::FindWindowByBt(Button* tabBt)
{
	for (auto& info : m_TabInfos)
	{
		if (info->bt == tabBt) return info->win;
	}
	return nullptr;
}

void TabSystem::TabButton::Start()
{
	Button::Start();
	GetOwner()->BindMouseBtDownFunc([&](const JGUIKeyDownEvent& e)
	{
		if (m_OwnerTab->m_CurrentDraggingTabBt == nullptr &&
			e.Code == KeyCode::LeftMouseButton)
		{
			m_OwnerTab->ActiveWindow(m_BindedWindow);
			m_OwnerTab->NotifyDraggingTabButton(this);
		}
	});
}
