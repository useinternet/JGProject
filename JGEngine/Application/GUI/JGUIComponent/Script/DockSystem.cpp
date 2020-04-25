#include "pch.h"
#include "DockSystem.h"

#include "GUI/JGUIObject/JGUIElement.h"
#include "GUI/JGUIObject/JGUIWindow.h"

#include "GUI/JGUIComponent/Element/JGUICanvas.h"
#include "GUI/JGUIComponent/Element/JGUIRenderer.h"

#include "GUI/JGUIComponent/Script/TitleBar.h"
#include "GUI/JGUIComponent/Script/TabSystem.h"

using namespace std;

void DockSystem::Awake()
{
	m_LineCanvas = GetOwner()->CreateJGUIElement("ResizeLineCanvas")->CreateJGUIComponent<JGUICanvas>();
	GetOwner()->GetTransform()->AttachTransform(m_LineCanvas->GetTransform());


	m_DockTarget = GetOwner()->CreateJGUIElement("DockTarget");
	m_DockTarget->CreateJGUIComponent<JGUICanvas>();
	m_DockTarget->GetTransform()->SetPivot(0.5f, 0.5f);
	for (int i = 0; i < DD_Count; ++i)
	{
		m_Docks[i] = m_DockTarget->CreateJGUIElement("Dock_" + to_string(i))->CreateJGUIComponent<JGUIImageRenderer>();
		m_Docks[i]->SetColor(JColor(1, 1, 1, 0.5F));
		m_Docks[i]->GetOwner()->RegisterCollider(JGUI_Collider_Box);
		m_Docks[i]->GetTransform()->SetPivot(0.5f, 0.5f);
	}
	m_PrevRect = GetTransform()->GetRect();
	m_PrevRect.Demical();
	Setting();
	JGUIExtraEvent extra_e;
	extra_e.Bind(JGUI_EXTRAEVENT_REPEAT, GetOwner(),
		[&](JGUIExtraEvent& e)
	{
		vector<DockInfo*> selected_removeInfo;
		for (auto& info_pair : m_DockInfoMap)
		{
			if (info_pair.first && info_pair.first->IsDestroying())
			{
				selected_removeInfo.push_back(info_pair.second);
			}
		}
		for (auto& info : selected_removeInfo)
		{
			ArrangeDockInfo(info);
		}
	});
	JGUI::RegisterExtraEvent(extra_e);
}

void DockSystem::Start()
{
	m_DockTarget->SetActive(false);


	GetOwner()->BindResizeFunc([&](const JGUIResizeEvent& e)
	{
		//auto root_win = JGUI::FindRootJGUIWindow(GetOwnerWindow()->GetRootWindowHandle());
		//auto window_size = root_win->GetTransform()->GetSize();
		//if (window_size.x < 800 || window_size.y < 600)
		//{
		//	m_PrevRect = GetTransform()->GetRect();
		//	m_PrevRect.Demical();
		//	m_DockTarget->GetTransform()->SetLocalPosition(m_PrevRect.width() * 0.5f,
		//		m_PrevRect.height() * 0.5f);
		//	return;
		//}
		Setting();
	});

}

void DockSystem::Destroy()
{
}

void DockSystem::Tick(float tick)
{
	auto dragging_win = JGUI::GetCurrentDraggingWindow();
	auto dock_pos = GetTransform()->GetPosition();
	if (dragging_win && dragging_win != GetOwnerWindow())
	{
		if (!m_DockTarget->IsActive())
		{
			m_DockTarget->SetActive(true);
		}

		// 마우스 위치 알아내고
		// 존재하는 사각형에 Dock 아이콘을 위치시킨다.
		DockInfo* contained_dock = nullptr;
		auto mouse_pos = GetOwnerWindow()->GetMousePos();
		for (auto& info_pair : m_DockInfoPool)
		{
			auto rect = info_pair.second->rect;
			auto center = rect.center();
			if (rect.Contains(mouse_pos))
			{
				m_DockTarget->GetTransform()->SetLocalPosition(center.x, center.y - dock_pos.y);
				contained_dock = info_pair.second.get();
			}
		}


		DockDirection dir = DD_Count;
		for (int i = 0; i < DD_Count; ++i)
		{
			if (m_Docks[i]->GetOwner()->GetCollider()->CheckInPoint(GetOwnerWindow()->GetMousePos()))
			{
				dir = (DockDirection)i;
				m_Docks[i]->SetColor(m_InteractionColor);
			}
			else
			{
				m_Docks[i]->SetColor(m_DefaultColor);
			}
		}
		if (dir == DD_Count)
		{
			dragging_win->SetParent(nullptr);

		}
		else
		{
			dragging_win->SetParent(GetOwnerWindow());
		}

		if (JGUI::GetKeyUp(GetOwnerWindow(), KeyCode::LeftMouseButton))
		{
			DockExecuteByDirection(dir, dragging_win, contained_dock);
			m_DockTarget->SetActive(false);
		}
	}
	else 	m_DockTarget->SetActive(false);
}

void DockSystem::SetActive(bool active)
{
	JGUIScriptComponent::SetActive(active);
	for (auto& info : m_DockInfoMap)
	{
		if (info.first == GetOwnerWindow()) continue;
		info.first->SetActive(active);
	}
}

void DockSystem::RequestRemoveBinedDock(JGUIWindow* win)
{
	if (m_DockInfoMap.find(win) == m_DockInfoMap.end()) return;
	ArrangeDockInfo(m_DockInfoMap[win]);
	win->AddWindowFlags(JGUI_WindowFlag_ChildWindow);
	win->RemoveWindowFlags(JGUI_WindowFlag_BottomMost);
}

void DockSystem::Setting()
{
	auto dock_size = GetTransform()->GetSize();
	float mid_x = dock_size.x * 0.5f;
	float mid_y = dock_size.y * 0.5f;


	m_Docks[DD_Left]->GetTransform()->SetSize(m_DockRectSize, m_DockRectSize);
	m_Docks[DD_Left]->GetTransform()->SetLocalPosition(-m_DockDistance, 0.0F);

	m_Docks[DD_Right]->GetTransform()->SetSize(m_DockRectSize, m_DockRectSize);
	m_Docks[DD_Right]->GetTransform()->SetLocalPosition(m_DockDistance, 0.0F);

	m_Docks[DD_Center]->GetTransform()->SetSize(m_DockRectSize, m_DockRectSize);
	m_Docks[DD_Center]->GetTransform()->SetLocalPosition(0.0f, 0.0f);

	m_Docks[DD_Top]->GetTransform()->SetSize(m_DockRectSize, m_DockRectSize);
	m_Docks[DD_Top]->GetTransform()->SetLocalPosition(0.0F, -m_DockDistance);

	m_Docks[DD_Bottom]->GetTransform()->SetSize(m_DockRectSize, m_DockRectSize);
	m_Docks[DD_Bottom]->GetTransform()->SetLocalPosition(0.0f, m_DockDistance);



	if (m_DockInfoMap.find(GetOwnerWindow()) == m_DockInfoMap.end() && m_DockInfoMap.empty())
	{
		unique_ptr<DockInfo> dock_info = make_unique<DockInfo>();
		m_DockInfoMap[GetOwnerWindow()] = dock_info.get();
		m_DockInfoPool[dock_info.get()] = move(dock_info);
		m_DockInfoMap[GetOwnerWindow()]->win = GetOwnerWindow();
	}
	if (m_DockInfoPool.size() == 1 && m_DockInfoMap.find(GetOwnerWindow()) != m_DockInfoMap.end())
	{
		m_DockInfoMap[GetOwnerWindow()]->rect = GetTransform()->GetRect();
	}
	else
	{
		SettingDockInfo();
	}

	m_PrevRect = GetTransform()->GetRect();
	m_PrevRect.Demical();
}

void DockSystem::SettingDockInfo()
{
	auto  resize_rect = GetTransform()->GetRect();
	auto  prev_rect = m_PrevRect;
	resize_rect.Demical();
	std::vector<DockResizeLine*> vEntryResizeLines;
	std::vector<DockResizeLine*> hEntryResizeLines;
	for (auto& line : m_ResizeLinePool)
	{
		JVector2 size = line->GetTransform()->GetSize();
		if (line->GetType() == 0)
		{
			if (size.x == prev_rect.width())
			{
				hEntryResizeLines.push_back(line);
			}
		}
		else
		{
			if (size.y == prev_rect.height())
			{
				vEntryResizeLines.push_back(line);
			}
		}
	}
	std::map<DockResizeLine*, bool> check_map;
	std::map<JGUIWindow*, bool>         check_win;


	std::sort(vEntryResizeLines.begin(), vEntryResizeLines.end(), [&](DockResizeLine* l1, DockResizeLine* l2)
	{
		return l1->GetTransform()->GetLocalPosition().x < l2->GetTransform()->GetLocalPosition().x;
	});
	std::sort(hEntryResizeLines.begin(), hEntryResizeLines.end(), [&](DockResizeLine* l1, DockResizeLine* l2)
	{
		return l1->GetTransform()->GetLocalPosition().y < l2->GetTransform()->GetLocalPosition().y;
	});

	JGUIRect next_resize_rect;

	if (!vEntryResizeLines.empty())
	{
		for (auto& line : vEntryResizeLines)
		{
			SettingDockInfo(1,
				resize_rect, prev_rect, next_resize_rect,
				line, check_map, check_win);
		}

	}

	if (!hEntryResizeLines.empty())
	{
		for (auto& line : hEntryResizeLines)
		{
			SettingDockInfo(0,
				resize_rect, prev_rect, next_resize_rect,
				line, check_map, check_win);
		}

	}
	if (vEntryResizeLines.empty() && hEntryResizeLines.empty())
	{
		auto rect = (GetOwnerWindow())->GetClientRect();
		rect.Demical();
		m_DockInfoMap.begin()->second->rect = rect;
		m_DockInfoMap.begin()->second->win->GetTransform()->SetSize(rect.width(), rect.height());
		m_DockInfoMap.begin()->second->win->GetTransform()->SetLocalPosition(rect.left, rect.top);
	}

}

void DockSystem::SettingDockInfo(int type, JGUIRect resize_rect, JGUIRect prev_rect, JGUIRect& next_resize_rect, DockResizeLine* owner, std::map<DockResizeLine*, bool>& check_line, std::map<JGUIWindow*, bool>& check_win)
{
	if (check_line[owner]) return;

	check_line[owner] = true;

	auto dock_pos = GetTransform()->GetPosition();
	dock_pos.x = (float)(int)dock_pos.x;
	dock_pos.y = (float)(int)dock_pos.y;
	// HLine
	if (type == 0)
	{
		JGUIRect prevTopRect;
		JGUIRect prevBottomRect;
		JGUIRect topRect;
		JGUIRect bottomRect;
		JVector2 line_world_pos = owner->GetTransform()->GetPosition();
		JVector2 line_size = owner->GetTransform()->GetSize();
		float pos_ratio = (line_world_pos.y - prev_rect.top) / prev_rect.height();
		auto top_area = owner->TopArea();
		auto bottom_area = owner->BottomArea();


		prevTopRect = top_area;
		prevBottomRect = bottom_area;

		float topRect_top = 0.0f;
		float line_pos = 0.0f;
		float bottomRect_bottom = 0.0f;

		bool is_first = false;
		bool is_last = false;

		if (top_area.top == prev_rect.top)          is_first = true;
		if (bottom_area.bottom == prev_rect.bottom) is_last = true;

		if (is_first && is_last)
		{
			topRect_top = resize_rect.top;
			line_pos = pos_ratio * resize_rect.height();
			bottomRect_bottom = resize_rect.bottom;
		}
		else if (is_first)
		{
			topRect_top = resize_rect.top;
			line_pos = pos_ratio * resize_rect.height();
			bottomRect_bottom = (bottom_area.bottom / prev_rect.height()) * resize_rect.height();
		}
		else if (is_last)
		{
			topRect_top = next_resize_rect.bottom + line_size.y;
			line_pos = pos_ratio * resize_rect.height();
			bottomRect_bottom = resize_rect.bottom;
		}
		else
		{
			topRect_top = next_resize_rect.bottom + line_size.y;
			line_pos = pos_ratio * resize_rect.height();
			bottomRect_bottom = (bottom_area.bottom / prev_rect.height()) * resize_rect.height();
		}
		line_pos += prev_rect.top;
		topRect = resize_rect;
		topRect.top = topRect_top;
		topRect.bottom = line_pos;

		bottomRect = resize_rect;
		bottomRect.top = line_pos + line_size.y;
		bottomRect.bottom = bottomRect_bottom;

		prevTopRect.Demical();
		prevBottomRect.Demical();
		topRect.Demical();
		bottomRect.Demical();


		owner->GetTransform()->SetLocalPosition(
			(int)(topRect.left - dock_pos.x),
			(int)(topRect.bottom - dock_pos.y));
		owner->GetTransform()->SetSize(topRect.width(), line_size.y);


		next_resize_rect = topRect;

		// 위쪽 업데이트
		if (owner->m_ConnectedResizeLines[DD_Top].empty())
		{
			for (auto dock_info_pair : owner->m_TopDock)
			{

				auto dock_info = dock_info_pair.second;
				if (check_win[dock_info->win]) continue;
				dock_info->rect = topRect;
				if (dock_info->win && dock_info->win != GetOwnerWindow())
				{
					dock_info->win->GetTransform()->SetSize(topRect.width(), topRect.height());
					dock_info->win->GetTransform()->SetLocalPosition(topRect.left, topRect.top);
				}
				check_win[dock_info->win] = true;

			}
		}
		vector<DockResizeLine*> l_t_line_array;
		l_t_line_array.assign(
			owner->m_ConnectedResizeLines[DD_Top].begin(),
			owner->m_ConnectedResizeLines[DD_Top].end());
		sort(l_t_line_array.begin(), l_t_line_array.end(), [&](DockResizeLine* l1, DockResizeLine* l2) -> bool
		{
			return l1->GetTransform()->GetLocalPosition().x < l2->GetTransform()->GetLocalPosition().x;
		});
		JGUIRect lt_next_reszie_rect;
		// 위
		for (auto& line : l_t_line_array)
		{
			SettingDockInfo(line->GetType(),
				topRect, prevTopRect, lt_next_reszie_rect,
				line,
				check_line, check_win);
		}



		// 아래쪽 업데이트
		if (is_last)
		{
			if (owner->m_ConnectedResizeLines[DD_Bottom].empty())
			{
				for (auto dock_info_pair : owner->m_BottomDock)
				{
					auto dock_info = dock_info_pair.second;
					if (check_win[dock_info->win]) continue;
					dock_info->rect = bottomRect;
					if (dock_info->win && dock_info->win != GetOwnerWindow())
					{
						dock_info->win->GetTransform()->SetSize(bottomRect.width(), bottomRect.height());
						dock_info->win->GetTransform()->SetLocalPosition(bottomRect.left, bottomRect.top);
					}
					check_win[dock_info->win] = true;

				}
			}
			vector<DockResizeLine*> r_b_line_array;
			r_b_line_array.assign(
				owner->m_ConnectedResizeLines[DD_Bottom].begin(),
				owner->m_ConnectedResizeLines[DD_Bottom].end());
			sort(r_b_line_array.begin(), r_b_line_array.end(), [&](DockResizeLine* l1, DockResizeLine* l2) -> bool
			{
				return l1->GetTransform()->GetLocalPosition().x < l2->GetTransform()->GetLocalPosition().x;
			});
			JGUIRect rb_next_reszie_rect;

			for (auto& line : r_b_line_array)
			{
				SettingDockInfo(line->GetType(),
					bottomRect, prevBottomRect, rb_next_reszie_rect,
					line,
					check_line, check_win);
			}
		}
	}


	// VLine
	if (type == 1)
	{
		JGUIRect prevLeftRect;
		JGUIRect prevRightRect;
		JGUIRect leftRect;
		JGUIRect rightRect;
		JVector2 line_world_pos = owner->GetTransform()->GetPosition();
		JVector2 line_size = owner->GetTransform()->GetSize();

		auto left_area = owner->LeftArea();
		auto right_area = owner->RightArea();
		float pos_ratio = (line_world_pos.x - prev_rect.left) / prev_rect.width();

		prevLeftRect = left_area;
		prevRightRect = right_area;

		float leftRect_left = 0.0f;
		float line_pos = 0.0f;
		float rightRect_right = 0.0f;

		bool is_first = false;
		bool is_last = false;

		if (left_area.left == prev_rect.left)     is_first = true;
		if (right_area.right == prev_rect.right) is_last = true;

		if (is_first && is_last)
		{
			leftRect_left = resize_rect.left;
			line_pos = pos_ratio * resize_rect.width();
			rightRect_right = resize_rect.right;
		}
		else if (is_first)
		{
			leftRect_left = resize_rect.left;
			line_pos = pos_ratio * resize_rect.width();
			rightRect_right = (right_area.right / prev_rect.width()) * resize_rect.width();
		}
		else if (is_last)
		{
			leftRect_left = next_resize_rect.right + line_size.x;
			line_pos = pos_ratio * resize_rect.width();
			rightRect_right = resize_rect.right;
		}
		else
		{
			leftRect_left = next_resize_rect.right + line_size.x;
			line_pos = pos_ratio * resize_rect.width();
			rightRect_right = (right_area.right / prev_rect.width()) * resize_rect.width();
		}
		line_pos += prev_rect.left;
		leftRect = resize_rect;
		leftRect.left = leftRect_left;
		leftRect.right = line_pos;

		rightRect = resize_rect;
		rightRect.left = line_pos + line_size.x;
		rightRect.right = rightRect_right;

		prevLeftRect.Demical();
		prevRightRect.Demical();
		leftRect.Demical();
		rightRect.Demical();


		owner->GetTransform()->SetLocalPosition(
			(int)(leftRect.right - dock_pos.x),
			(int)(leftRect.top - dock_pos.y));
		owner->GetTransform()->SetSize(line_size.x, leftRect.height());


		next_resize_rect = leftRect;

		// 위쪽 업데이트
		if (owner->m_ConnectedResizeLines[DD_Left].empty())
		{
			for (auto dock_info_pair : owner->m_LeftDock)
			{

				auto dock_info = dock_info_pair.second;
				if (check_win[dock_info->win]) continue;
				dock_info->rect = leftRect;
				if (dock_info->win && dock_info->win != GetOwnerWindow())
				{
					dock_info->win->GetTransform()->SetSize(leftRect.width(), leftRect.height());
					dock_info->win->GetTransform()->SetLocalPosition(leftRect.left, leftRect.top);
				}
				check_win[dock_info->win] = true;

			}
		}
		vector<DockResizeLine*> l_t_line_array;
		l_t_line_array.assign(
			owner->m_ConnectedResizeLines[DD_Left].begin(),
			owner->m_ConnectedResizeLines[DD_Left].end());
		sort(l_t_line_array.begin(), l_t_line_array.end(), [&](DockResizeLine* l1, DockResizeLine* l2) -> bool
		{
			return l1->GetTransform()->GetLocalPosition().y < l2->GetTransform()->GetLocalPosition().y;
		});
		JGUIRect lt_next_reszie_rect;
		// 위
		for (auto& line : l_t_line_array)
		{
			SettingDockInfo(line->GetType(),
				leftRect, prevLeftRect, lt_next_reszie_rect,
				line,
				check_line, check_win);
		}



		// 아래쪽 업데이트
		if (is_last)
		{
			if (owner->m_ConnectedResizeLines[DD_Right].empty())
			{
				for (auto dock_info_pair : owner->m_RightDock)
				{
					auto dock_info = dock_info_pair.second;
					if (check_win[dock_info->win]) continue;
					dock_info->rect = rightRect;
					if (dock_info->win && dock_info->win != GetOwnerWindow())
					{
						dock_info->win->GetTransform()->SetSize(rightRect.width(), rightRect.height());
						dock_info->win->GetTransform()->SetLocalPosition(rightRect.left, rightRect.top);
					}
					check_win[dock_info->win] = true;

				}
			}
			vector<DockResizeLine*> r_b_line_array;
			r_b_line_array.assign(
				owner->m_ConnectedResizeLines[DD_Right].begin(),
				owner->m_ConnectedResizeLines[DD_Right].end());
			sort(r_b_line_array.begin(), r_b_line_array.end(), [&](DockResizeLine* l1, DockResizeLine* l2) -> bool
			{
				return l1->GetTransform()->GetLocalPosition().y < l2->GetTransform()->GetLocalPosition().y;
			});
			JGUIRect rb_next_reszie_rect;

			for (auto& line : r_b_line_array)
			{
				SettingDockInfo(line->GetType(),
					rightRect, prevRightRect, rb_next_reszie_rect,
					line,
					check_line, check_win);
			}
		}
	}
}

void DockSystem::DockExecuteByDirection(DockDirection dir, JGUIWindow* dragging_win, DockInfo* containing_dock)
{
	if (dir >= DD_Count || dir < 0) return;
	if (dragging_win->GetWindowFlags() & JGUI_WindowFlag_EnableDock) return;

	dragging_win->SetParent(GetOwnerWindow());
	dragging_win->AddWindowFlags(JGUI_WindowFlag_DockWindow);
	dragging_win->RemoveWindowFlags(JGUI_WindowFlag_EnableResize | JGUI_WindowFlag_NoMove);
	auto dock_info = make_unique<DockInfo>();
	containing_dock->rect.Demical();
	JGUIRect containing_rect = containing_dock->rect;

	float half_thick = m_DefaultResizeThick * 0.5f;

	JGUIRect dock_rect;




	switch (dir)
	{
	case DD_Left:

		dock_rect.left = containing_rect.left;
		dock_rect.top = containing_rect.top;
		dock_rect.right = dock_rect.left + (containing_rect.width() * 0.5f - half_thick);
		dock_rect.bottom = dock_rect.top + containing_rect.height();
		containing_rect.left += (containing_rect.width() * 0.5f + half_thick);
		break;
	case DD_Right:
		dock_rect.left = containing_rect.left + (containing_rect.width() * 0.5f) + half_thick;
		dock_rect.top = containing_rect.top;
		dock_rect.right = dock_rect.left + (containing_rect.width() * 0.5f - half_thick);
		dock_rect.bottom = dock_rect.top + containing_rect.height();
		containing_rect.right -= (containing_rect.width() * 0.5f + half_thick);
		;		break;

	case DD_Top:
		dock_rect.left = containing_rect.left;
		dock_rect.top = containing_rect.top;
		dock_rect.right = dock_rect.left + containing_rect.width();
		dock_rect.bottom = dock_rect.top + (containing_rect.height() * 0.5f - half_thick);

		containing_rect.top += (containing_rect.height() * 0.5f + half_thick);
		break;
	case DD_Bottom:
		dock_rect.left = containing_rect.left;
		dock_rect.right = dock_rect.left + containing_rect.width();
		dock_rect.top = containing_rect.top + (containing_rect.height() * 0.5f) + half_thick;
		dock_rect.bottom = dock_rect.top + (containing_rect.height() * 0.5f - half_thick);
		containing_rect.bottom -= (containing_rect.height() * 0.5f + half_thick);
		break;
	case DD_Center:
		if (containing_dock->win == GetOwnerWindow())
		{
			dragging_win->GetTransform()->SetSize(containing_rect.width(), containing_rect.height());
			dragging_win->GetTransform()->SetLocalPosition(containing_rect.left, containing_rect.top);
			dock_info->win = dragging_win;
			dock_info->rect = containing_rect;
			for (auto& line_pair : containing_dock->resizeLines)
			{
				if (line_pair.second == nullptr) continue;
				switch (line_pair.first)
				{
				case DD_Left:
					line_pair.second->AddBindDock(DD_Right, dock_info.get());
					break;
				case DD_Right:
					line_pair.second->AddBindDock(DD_Left, dock_info.get());
					break;
				case DD_Top:
					line_pair.second->AddBindDock(DD_Bottom, dock_info.get());
					break;
				case DD_Bottom:
					line_pair.second->AddBindDock(DD_Top, dock_info.get());
					break;
				}
				line_pair.second->UnBindDock(containing_dock);
			}


			m_DockInfoMap.erase(containing_dock->win);
			m_DockInfoPool.erase(containing_dock);
			containing_dock = nullptr;
		}
		else
		{
			auto titlebar = dragging_win->GetTitleBar();
			if (titlebar) titlebar->UnConnect();

			auto tabSys = containing_dock->win->GetTabSystem();
			if (tabSys) tabSys->BindWindow(dragging_win);
			return;
		}

		break;
	}
	dock_info->win = dragging_win;
	if (dir != DD_Center)
	{
		dock_rect.Demical();
		dock_info->rect = dock_rect;
		dock_info->win->GetTransform()->SetSize(dock_rect.width(), dock_rect.height());
		dock_info->win->GetTransform()->SetLocalPosition(dock_rect.left, dock_rect.top);

	}
	m_DockInfoMap[dragging_win] = dock_info.get();
	m_DockInfoPool[dock_info.get()] = move(dock_info);


	if (containing_dock)
	{
		if (containing_dock->win && containing_dock->win != GetOwnerWindow())
		{
			containing_dock->win->GetTransform()->SetSize(containing_rect.width(), containing_rect.height());
			containing_dock->win->GetTransform()->SetLocalPosition(containing_rect.left, containing_rect.top);
		}

		containing_dock->rect = containing_rect;
		switch (dir)
		{
		case DD_Left:
			// Old 기준 : left  resizeLine에 Unbind, right는 그대로
			if (containing_dock->resizeLines[DD_Top])
			{
				containing_dock->resizeLines[DD_Top]->AddBindDock(DD_Bottom, m_DockInfoMap[dragging_win]);
			}
			if (containing_dock->resizeLines[DD_Bottom])
			{
				containing_dock->resizeLines[DD_Bottom]->AddBindDock(DD_Top, m_DockInfoMap[dragging_win]);
			}
			if (containing_dock->resizeLines[DD_Left])
			{
				containing_dock->resizeLines[DD_Left]->AddBindDock(DD_Right, m_DockInfoMap[dragging_win]);
				containing_dock->resizeLines[DD_Left]->UnBindDock(containing_dock);

			}
			break;
		case DD_Right:
			// Old 기준 : right resizeLine 에 Unbind, left는 그대로
			if (containing_dock->resizeLines[DD_Top])
			{
				containing_dock->resizeLines[DD_Top]->AddBindDock(DD_Bottom, m_DockInfoMap[dragging_win]);
			}
			if (containing_dock->resizeLines[DD_Bottom])
			{
				containing_dock->resizeLines[DD_Bottom]->AddBindDock(DD_Top, m_DockInfoMap[dragging_win]);
			}
			if (containing_dock->resizeLines[DD_Right])
			{
				containing_dock->resizeLines[DD_Right]->AddBindDock(DD_Left, m_DockInfoMap[dragging_win]);
				containing_dock->resizeLines[DD_Right]->UnBindDock(containing_dock);

			}
			break;
		case DD_Top:
			if (containing_dock->resizeLines[DD_Left])
			{
				containing_dock->resizeLines[DD_Left]->AddBindDock(DD_Right, m_DockInfoMap[dragging_win]);
			}
			if (containing_dock->resizeLines[DD_Right])
			{
				containing_dock->resizeLines[DD_Right]->AddBindDock(DD_Left, m_DockInfoMap[dragging_win]);
			}
			if (containing_dock->resizeLines[DD_Top])
			{
				containing_dock->resizeLines[DD_Top]->AddBindDock(DD_Bottom, m_DockInfoMap[dragging_win]);
				containing_dock->resizeLines[DD_Top]->UnBindDock(containing_dock);

			}
			break;
		case DD_Bottom:
			if (containing_dock->resizeLines[DD_Left])
			{
				containing_dock->resizeLines[DD_Left]->AddBindDock(DD_Right, m_DockInfoMap[dragging_win]);
			}
			if (containing_dock->resizeLines[DD_Right])
			{
				containing_dock->resizeLines[DD_Right]->AddBindDock(DD_Left, m_DockInfoMap[dragging_win]);
			}
			if (containing_dock->resizeLines[DD_Bottom])
			{
				containing_dock->resizeLines[DD_Bottom]->AddBindDock(DD_Top, m_DockInfoMap[dragging_win]);
				containing_dock->resizeLines[DD_Bottom]->UnBindDock(containing_dock);

			}
			break;
		}
		// resizeLine 생성 및 재조정
		switch (dir)
		{
		case DD_Left:
		case DD_Top:
			// 양쪽 reizeLine에 추가
			CreateResizeLineByDirection(dir, m_DockInfoMap[dragging_win], containing_dock);
			break;
		case DD_Right:
		case DD_Bottom:
			CreateResizeLineByDirection(dir, containing_dock, m_DockInfoMap[dragging_win]);
			break;
		}
	}






	auto titlebar = dragging_win->GetTitleBar();
	if (titlebar) titlebar->UnConnect();
	m_Docks[dir]->SetColor(m_DefaultColor);
}

void DockSystem::CreateResizeLineByDirection(DockDirection dir, DockInfo* l_t_info, DockInfo* r_b_info)
{
	DockResizeLine* resize_line = nullptr;
	auto lise_obj = m_LineCanvas->GetOwner()->CreateJGUIElement("DockResizeLine");
	lise_obj->CreateJGUIComponent<JGUIImageRenderer>();
	resize_line = lise_obj->CreateJGUIComponent<DockResizeLine>();


	switch (dir)
	{
	case DD_Left:
	case DD_Right:
		resize_line->BindDock(1, l_t_info, r_b_info);
		break;
	case DD_Top:
	case DD_Bottom:
		resize_line->BindDock(0, l_t_info, r_b_info);
		break;
	}
	m_ResizeLinePool.insert(resize_line);
}

void DockSystem::ArrangeDockInfo(DockInfo* remove_info)
{
	JGUIRect rect1 = remove_info->rect;
	rect1.Demical();
	DockDirection expected_remove_line_dir = DD_Count;

	int remove_width = (int)rect1.width();
	int remove_height = (int)rect1.height();

	// 병합할 방향 찾기
	for (auto& line : remove_info->resizeLines)
	{
		if (line.second == nullptr) continue;
		int lineLength = 0;
		JVector2 lineSize = line.second->GetTransform()->GetSize();
		if (line.second->GetType() == 0)
		{
			lineLength = (int)lineSize.x;
			if (remove_width == lineLength)
			{
				expected_remove_line_dir = line.first;
				break;
			}
		}
		else
		{
			lineLength = (int)lineSize.y;
			if (lineLength == remove_height)
			{
				expected_remove_line_dir = line.first;
				break;
			}
		}
	}

	// 윈도우 병합
	if (expected_remove_line_dir != DD_Count)
	{
		DockResizeLine* line = remove_info->resizeLines[expected_remove_line_dir];
		if (line)
		{
			switch (expected_remove_line_dir)
			{
			case DD_Left:
				for (auto& dock_pair : line->m_LeftDock)
				{
					auto dock_info = dock_pair.second;
					dock_info->rect.right = rect1.right;
					if (dock_info->win && dock_info->win != GetOwnerWindow())
					{
						dock_info->win->GetTransform()->SetSize(dock_info->rect.width(), dock_info->rect.height());
						dock_info->win->GetTransform()->SetLocalPosition(dock_info->rect.left, dock_info->rect.top);
					}
				}
				break;
			case DD_Right:
				for (auto& dock_pair : line->m_RightDock)
				{
					auto dock_info = dock_pair.second;
					dock_info->rect.left = rect1.left;
					if (dock_info->win && dock_info->win != GetOwnerWindow())
					{
						dock_info->win->GetTransform()->SetSize(dock_info->rect.width(), dock_info->rect.height());
						dock_info->win->GetTransform()->SetLocalPosition(dock_info->rect.left, dock_info->rect.top);
					}
				}
				break;
			case DD_Bottom:
				for (auto& dock_pair : line->m_BottomDock)
				{
					auto dock_info = dock_pair.second;
					dock_info->rect.top = rect1.top;
					if (dock_info->win && dock_info->win != GetOwnerWindow())
					{
						dock_info->win->GetTransform()->SetSize(dock_info->rect.width(), dock_info->rect.height());
						dock_info->win->GetTransform()->SetLocalPosition(dock_info->rect.left, dock_info->rect.top);
					}
				}
				break;
			case DD_Top:
				for (auto& dock_pair : line->m_TopDock)
				{
					auto dock_info = dock_pair.second;
					dock_info->rect.bottom = rect1.bottom;
					if (dock_info->win && dock_info->win != GetOwnerWindow())
					{
						dock_info->win->GetTransform()->SetSize(dock_info->rect.width(), dock_info->rect.height());
						dock_info->win->GetTransform()->SetLocalPosition(dock_info->rect.left, dock_info->rect.top);
					}
				}
				break;
			}


		}
	}



	// 윈도우 간의 연결 해제
	if (expected_remove_line_dir != DD_Count)
	{
		DockResizeLine* delete_line = remove_info->resizeLines[expected_remove_line_dir];
		if (delete_line)
		{
			DockResizeLine* line = delete_line;
			switch (expected_remove_line_dir)
			{
			case DD_Left:

				if (remove_info->resizeLines[DD_Right])
				{
					for (auto& dock_info_pair : line->m_LeftDock)
					{
						auto dock_info = dock_info_pair.second;
						remove_info->resizeLines[DD_Right]->AddBindDock(DD_Left, dock_info);
						dock_info->resizeLines[DD_Right] = remove_info->resizeLines[DD_Right];
					}
					for (auto& connect_line : line->m_ConnectedResizeLines[DD_Left])
					{
						connect_line->m_ConnectedResizeLines[DD_Right].erase(line);
						connect_line->m_ConnectedResizeLines[DD_Right].insert(remove_info->resizeLines[DD_Right]);
						remove_info->resizeLines[DD_Right]->m_ConnectedResizeLines[DD_Left].insert(connect_line);

					}
				}
				else
				{
					for (auto& dock_info_pair : line->m_LeftDock)
					{
						auto dock_info = dock_info_pair.second;
						dock_info->resizeLines[DD_Right] = nullptr;
					}
				}
				break;
			case DD_Right:
				if (remove_info->resizeLines[DD_Left])
				{
					for (auto& dock_info_pair : line->m_RightDock)
					{
						auto dock_info = dock_info_pair.second;
						remove_info->resizeLines[DD_Left]->AddBindDock(DD_Right, dock_info);
						dock_info->resizeLines[DD_Left] = remove_info->resizeLines[DD_Left];
					}
					for (auto& connect_line : line->m_ConnectedResizeLines[DD_Right])
					{
						connect_line->m_ConnectedResizeLines[DD_Left].erase(line);
						connect_line->m_ConnectedResizeLines[DD_Left].insert(remove_info->resizeLines[DD_Left]);
						remove_info->resizeLines[DD_Left]->m_ConnectedResizeLines[DD_Right].insert(connect_line);
					}
				}
				else
				{
					for (auto& dock_info_pair : line->m_RightDock)
					{
						auto dock_info = dock_info_pair.second;
						dock_info->resizeLines[DD_Left] = nullptr;
					}
				}
				break;
			case DD_Top:

				if (remove_info->resizeLines[DD_Bottom])
				{
					for (auto& dock_info_pair : line->m_TopDock)
					{
						auto dock_info = dock_info_pair.second;
						remove_info->resizeLines[DD_Bottom]->AddBindDock(DD_Top, dock_info);
						dock_info->resizeLines[DD_Bottom] = remove_info->resizeLines[DD_Bottom];
					}
					for (auto& connect_line : line->m_ConnectedResizeLines[DD_Top])
					{
						connect_line->m_ConnectedResizeLines[DD_Bottom].erase(line);
						connect_line->m_ConnectedResizeLines[DD_Bottom].insert(remove_info->resizeLines[DD_Bottom]);
						remove_info->resizeLines[DD_Bottom]->m_ConnectedResizeLines[DD_Top].insert(connect_line);
					}

				}
				else
				{
					for (auto& dock_info_pair : line->m_TopDock)
					{
						auto dock_info = dock_info_pair.second;
						dock_info->resizeLines[DD_Bottom] = nullptr;
					}
				}
				break;
			case DD_Bottom:

				if (remove_info->resizeLines[DD_Top])
				{
					for (auto& dock_info_pair : line->m_BottomDock)
					{
						auto dock_info = dock_info_pair.second;
						remove_info->resizeLines[DD_Top]->AddBindDock(DD_Bottom, dock_info);
						dock_info->resizeLines[DD_Top] = remove_info->resizeLines[DD_Top];
					}
					for (auto& connect_line : line->m_ConnectedResizeLines[DD_Bottom])
					{
						connect_line->m_ConnectedResizeLines[DD_Top].erase(line);
						connect_line->m_ConnectedResizeLines[DD_Top].insert(remove_info->resizeLines[DD_Top]);
						remove_info->resizeLines[DD_Top]->m_ConnectedResizeLines[DD_Bottom].insert(connect_line);
					}

				}
				else
				{
					for (auto& dock_info_pair : line->m_BottomDock)
					{
						auto dock_info = dock_info_pair.second;
						dock_info->resizeLines[DD_Top] = nullptr;
					}
				}
				break;
			}


			for (auto& connect_line : line->m_ConnectedResizeLines[expected_remove_line_dir])
			{
				connect_line->SizeUpdateByBindedWindow();
			}
			line->UnBindDock(remove_info);

		}


		for (auto& line_pair : remove_info->resizeLines)
		{
			if (line_pair.second == nullptr) continue;
			DockResizeLine* line = line_pair.second;


			if (delete_line)
			{
				for (auto& line_set_pair : line->m_ConnectedResizeLines)
				{
					for (auto& line : line_set_pair.second)
					{
						if (line == delete_line)
						{
							line_set_pair.second.erase(delete_line);
							break;
						}
					}
				}
			}

			line->UnBindDock(remove_info);
		}

		if (delete_line)
		{
			delete_line->Reset();
			GetOwner()->DestroyJGUIElement(delete_line->GetOwner());
			m_ResizeLinePool.erase(delete_line);
		}
	}


	if (remove_info->win)
	{
		m_DockInfoMap.erase(remove_info->win);
	}
	m_DockInfoPool.erase(remove_info);


	if (m_DockInfoPool.empty())
	{
		unique_ptr<DockInfo> dock_info = make_unique<DockInfo>();
		m_DockInfoMap[GetOwnerWindow()] = dock_info.get();
		m_DockInfoPool[dock_info.get()] = move(dock_info);
		m_DockInfoMap[GetOwnerWindow()]->win = GetOwnerWindow();
		m_DockInfoMap[GetOwnerWindow()]->rect = GetTransform()->GetRect();
	}
}


// 
void DockSystem::DockResizeLine::Awake()
{
	if (GetOwner()->GetCollider() == nullptr) GetOwner()->RegisterCollider(JGUI_Collider_Box);


}

void DockSystem::DockResizeLine::Start()
{
	m_Line = GetOwner()->FindComponent<JGUIImageRenderer>();
	m_Line->SetColor(JColor::Black());


	GetOwner()->BindMouseMoveFunc([&](const JGUIMouseMoveEvent& e)
	{
		if (m_Line)
		{
			m_Line->SetColor(JColor::Red());
		}
	});

	GetOwner()->BindMouseBtDownFunc([&](const JGUIKeyDownEvent& e)
	{
		if (!m_IsResizeing && e.Code == KeyCode::LeftMouseButton)
		{
			m_PrevMousePos = GetOwnerWindow()->GetMousePos();
			m_IsResizeing = true;
			JGUIExtraEvent extra_e;
			extra_e.Bind(JGUI_EXTRAEVENT_REPEAT, GetOwner(), [&](JGUIExtraEvent& e)
			{
				JGUIRect top_area;
				JGUIRect bottom_area;
				JGUIRect right_area;
				JGUIRect left_area;
				auto mouse_pos = GetOwnerWindow()->GetMousePos();
				if (m_PrevMousePos != mouse_pos)
				{
					int delta_x = mouse_pos.x - m_PrevMousePos.x;
					int delta_y = mouse_pos.y - m_PrevMousePos.y;

					switch (m_LineType)
					{
					case HLine:
						top_area = TopArea();
						top_area.bottom += delta_y;

						bottom_area = BottomArea();
						bottom_area.top += delta_y;


						if (delta_y < 0 && top_area.height() <= 200) break;
						if (delta_y >= 0 && bottom_area.height() <= 200) break;


						for (auto& info_pair : m_TopDock)
						{
							info_pair.second->rect.bottom += delta_y;
							auto rect = info_pair.second->rect;


							if (info_pair.first == GetOwnerWindow()) continue;
							info_pair.first->GetTransform()->SetSize(rect.width(), rect.height());
						}
						for (auto& info_pair : m_BottomDock)
						{
							info_pair.second->rect.top += delta_y;
							auto rect = info_pair.second->rect;
							if (info_pair.first == GetOwnerWindow()) continue;
							info_pair.first->GetTransform()->SetSize(rect.width(), rect.height());
							info_pair.first->GetTransform()->SetLocalPosition(rect.left, rect.top);
						}

						GetTransform()->OffsetLocalPosition(0.0f, delta_y);

						for (auto& line : m_ConnectedResizeLines[DD_Bottom])
						{
							// delta_x 만큼 사이즈빼주고
							auto line_pos = line->GetTransform()->GetLocalPosition();
							line_pos.y += delta_y;
							line->GetTransform()->SetLocalPosition(line_pos);
							auto line_size = line->GetTransform()->GetSize();
							line_size.y -= delta_y;
							line->GetTransform()->SetSize(line_size);
						}
						for (auto& line : m_ConnectedResizeLines[DD_Top])
						{
							// delta_x 만큼 사이즈만 더해줌
							auto line_size = line->GetTransform()->GetSize();
							line_size.y += delta_y;
							line->GetTransform()->SetSize(line_size);
						}

						//
						break;
					case VLine:

						left_area = LeftArea();
						left_area.right += delta_x;

						right_area = RightArea()
							;
						right_area.left += delta_x;

						if (delta_x < 0 && left_area.width() <= 200) break;
						if (delta_x >= 0 && right_area.width() <= 200) break;

						for (auto& info_pair : m_LeftDock)
						{
							info_pair.second->rect.right += delta_x;
							auto rect = info_pair.second->rect;
							if (info_pair.first == GetOwnerWindow()) continue;
							info_pair.first->GetTransform()->SetSize(rect.width(), rect.height());
						}
						for (auto& info_pair : m_RightDock)
						{
							info_pair.second->rect.left += delta_x;
							auto rect = info_pair.second->rect;
							if (info_pair.first == GetOwnerWindow()) continue;
							info_pair.first->GetTransform()->SetSize(rect.width(), rect.height());
							info_pair.first->GetTransform()->SetLocalPosition(rect.left, rect.top);
						}

						GetTransform()->OffsetLocalPosition(delta_x, 0.0f);


						for (auto& line : m_ConnectedResizeLines[DD_Right])
						{
							// delta_x 만큼 사이즈빼주고
							auto line_pos = line->GetTransform()->GetLocalPosition();
							line_pos.x += delta_x;
							line->GetTransform()->SetLocalPosition(line_pos);
							auto line_size = line->GetTransform()->GetSize();
							line_size.x -= delta_x;
							line->GetTransform()->SetSize(line_size);
						}
						for (auto& line : m_ConnectedResizeLines[DD_Left])
						{
							// delta_x 만큼 사이즈만 더해줌
							auto line_size = line->GetTransform()->GetSize();
							line_size.x += delta_x;
							line->GetTransform()->SetSize(line_size);
						}
						break;
					}



				}



				if (JGUI::GetKeyUp(GetOwnerWindow(), KeyCode::LeftMouseButton) || !IsActive())
				{
					m_IsResizeing = false;
					e.flag = JGUI_EXTRAEVENT_EXIT;
					m_Line->SetColor(JColor::Black());
				}

				m_PrevMousePos = mouse_pos;
			});

			JGUI::RegisterExtraEvent(extra_e);

		}
	});







}

void DockSystem::DockResizeLine::Destroy()
{
}

void DockSystem::DockResizeLine::Tick(float tick)
{
	if (!m_IsResizeing && !GetOwner()->GetCollider()->CheckInPoint(GetOwnerWindow()->GetMousePos()))
	{
		m_Line->SetColor(JColor::Black());
	}
}

void DockSystem::DockResizeLine::BindDock(int line_type, DockInfo* l_t_info, DockInfo* r_b_info)
{
	
	m_LineType = line_type;
	if (m_Line) m_Line->SetActive(true);
	auto dock_pos = GetOwner()->GetParent()->GetTransform()->GetPosition();
	l_t_info->rect.Demical();
	r_b_info->rect.Demical();
	switch (m_LineType)
	{
	case HLine:
		m_TopDock[l_t_info->win] = l_t_info;
		m_BottomDock[r_b_info->win] = r_b_info;

		GetTransform()->SetLocalPosition(
			(int)(l_t_info->rect.left - dock_pos.x), (int)(l_t_info->rect.bottom - dock_pos.y));
		GetTransform()->SetSize(l_t_info->rect.width(), m_Thick);

		l_t_info->resizeLines[DD_Bottom] = this;
		r_b_info->resizeLines[DD_Top] = this;



		if (l_t_info->resizeLines[DD_Left])
		{
			l_t_info->resizeLines[DD_Left]->m_ConnectedResizeLines[DD_Right].insert(this);
			m_ConnectedResizeLines[DD_Left].insert(l_t_info->resizeLines[DD_Left]);
		}
		if (l_t_info->resizeLines[DD_Right])
		{
			l_t_info->resizeLines[DD_Right]->m_ConnectedResizeLines[DD_Left].insert(this);
			m_ConnectedResizeLines[DD_Right].insert(l_t_info->resizeLines[DD_Right]);
		}


		break;
	case VLine:
		m_LeftDock[l_t_info->win] = l_t_info;
		m_RightDock[r_b_info->win] = r_b_info;


		GetTransform()->SetLocalPosition(
			(int)(l_t_info->rect.right - dock_pos.x), (int)(l_t_info->rect.top - dock_pos.y));
		GetTransform()->SetSize(m_Thick, l_t_info->rect.height());


		l_t_info->resizeLines[DD_Right] = this;
		r_b_info->resizeLines[DD_Left] = this;

		if (l_t_info->resizeLines[DD_Top])
		{
			l_t_info->resizeLines[DD_Top]->m_ConnectedResizeLines[DD_Bottom].insert(this);
			m_ConnectedResizeLines[DD_Top].insert(l_t_info->resizeLines[DD_Top]);
		}
		if (l_t_info->resizeLines[DD_Bottom])
		{
			l_t_info->resizeLines[DD_Bottom]->m_ConnectedResizeLines[DD_Top].insert(this);
			m_ConnectedResizeLines[DD_Bottom].insert(l_t_info->resizeLines[DD_Bottom]);
		}

		break;
	}
}

void DockSystem::DockResizeLine::UnBindDock(DockInfo* dock_info)
{
	switch (m_LineType)
	{
	case HLine:
		if (m_TopDock.find(dock_info->win) != m_TopDock.end())
		{
			m_TopDock.erase(dock_info->win);
			dock_info->resizeLines[DD_Bottom] = nullptr;
		}
		if (m_BottomDock.find(dock_info->win) != m_BottomDock.end())
		{
			m_BottomDock.erase(dock_info->win);
			dock_info->resizeLines[DD_Top] = nullptr;
		}
		break;
	case VLine:
		if (m_LeftDock.find(dock_info->win) != m_LeftDock.end())
		{
			m_LeftDock.erase(dock_info->win);
			dock_info->resizeLines[DD_Right] = nullptr;
		}
		if (m_RightDock.find(dock_info->win) != m_RightDock.end())
		{
			m_RightDock.erase(dock_info->win);
			dock_info->resizeLines[DD_Left] = nullptr;
		}


		break;
	}
}

void DockSystem::DockResizeLine::AddBindDock(DockDirection dir, DockInfo* dock_info)
{
	if (m_LineType == HLine && (dir == DD_Left || dir == DD_Right)) return;
	if (m_LineType == VLine && (dir == DD_Top || dir == DD_Bottom)) return;


	switch (dir)
	{
	case DD_Left:
		if (m_LeftDock.find(dock_info->win) == m_LeftDock.end())
		{
			m_LeftDock[dock_info->win] = dock_info;
			dock_info->resizeLines[DD_Right] = this;

		}
		break;
	case DD_Right:
		if (m_RightDock.find(dock_info->win) == m_RightDock.end())
		{
			m_RightDock[dock_info->win] = dock_info;
			dock_info->resizeLines[DD_Left] = this;
		}
		break;
	case DD_Top:
		if (m_TopDock.find(dock_info->win) == m_TopDock.end())
		{
			m_TopDock[dock_info->win] = dock_info;
			dock_info->resizeLines[DD_Bottom] = this;
		}
		break;
	case DD_Bottom:
		if (m_BottomDock.find(dock_info->win) == m_BottomDock.end())
		{
			m_BottomDock[dock_info->win] = dock_info;
			dock_info->resizeLines[DD_Top] = this;
		}
		break;
	}
}

bool DockSystem::DockResizeLine::IsBindedDock(DockInfo* info)
{
	switch (m_LineType)
	{
	case HLine:
		if (m_TopDock.find(info->win) != m_TopDock.end()) return true;
		else if (m_BottomDock.find(info->win) != m_BottomDock.end()) return true;
		else return false;
	case VLine:
		if (m_LeftDock.find(info->win) != m_LeftDock.end()) return true;
		else if (m_RightDock.find(info->win) != m_RightDock.end()) return true;
		else return false;
	}

	return false;
}

void DockSystem::DockResizeLine::Reset()
{
	SetActive(false);
	m_LeftDock.clear();
	m_RightDock.clear();
	m_TopDock.clear();
	m_BottomDock.clear();
	m_IsResizeing = false;
	m_ConnectedResizeLines.clear();
	m_PrevMousePos = { 0,0 };
}

void DockSystem::DockResizeLine::SizeUpdateByBindedWindow()
{
	auto dock_pos = GetOwner()->GetParent()->GetTransform()->GetPosition();
	dock_pos.x = (float)(int)dock_pos.x;
	dock_pos.y = (float)(int)dock_pos.y;
	if (m_LineType == 0)
	{
		JGUIRect win_rect;
		win_rect.top = 99999999;
		win_rect.bottom = -1;
		win_rect.left = 99999999;
		win_rect.right = -1;
		for (auto& dock_pair : m_TopDock)
		{
			auto rect = dock_pair.second->rect;
			win_rect.top = std::min<float>(rect.top, win_rect.top);
			win_rect.bottom = std::max<float>(rect.bottom, win_rect.bottom);
			win_rect.left = std::min<float>(rect.left, win_rect.left);
			win_rect.right = std::max<float>(rect.right, win_rect.right);
		}


		GetTransform()->SetLocalPosition(
			(int)(win_rect.left - dock_pos.x), win_rect.bottom - dock_pos.y);
		GetTransform()->SetSize(win_rect.width(), GetLineThickness());
	}

	// VLine
	// 왼쪽 윈도우의 height만큼 사이즈를 가지고 Left Window Rect의 Right, Top의 위치를 가짐
	if (m_LineType == 1)
	{
		JGUIRect win_rect;
		win_rect.top = 99999999;
		win_rect.bottom = -1;
		win_rect.left = 99999999;
		win_rect.right = -1;
		for (auto& dock_pair : m_LeftDock)
		{
			auto rect = dock_pair.second->rect;
			win_rect.top = std::min<float>(rect.top, win_rect.top);
			win_rect.bottom = std::max<float>(rect.bottom, win_rect.bottom);
			win_rect.left = std::min<float>(rect.left, win_rect.left);
			win_rect.right = std::max<float>(rect.right, win_rect.right);
		}
		GetTransform()->SetLocalPosition(
			(int)(win_rect.right - dock_pos.x), win_rect.top - dock_pos.y);
		GetTransform()->SetSize(GetLineThickness(), win_rect.height());
	}
}

JGUIRect DockSystem::DockResizeLine::TopArea()
{
	JGUIRect win_rect;
	win_rect.top = 99999999;
	win_rect.bottom = -1;
	win_rect.left = 99999999;
	win_rect.right = -1;
	for (auto& dock_pair : m_TopDock)
	{
		auto rect = dock_pair.second->rect;
		win_rect.top = std::min<float>(rect.top, win_rect.top);
		win_rect.left = std::min<float>(rect.left, win_rect.left);
		win_rect.bottom = std::max<float>(rect.bottom, win_rect.bottom);
		win_rect.right = std::max<float>(rect.right, win_rect.right);
	}
	return win_rect;
}

JGUIRect DockSystem::DockResizeLine::BottomArea()
{
	JGUIRect win_rect;
	win_rect.top = 99999999;
	win_rect.bottom = -1;
	win_rect.left = 99999999;
	win_rect.right = -1;
	for (auto& dock_pair : m_BottomDock)
	{
		auto rect = dock_pair.second->rect;
		win_rect.top = std::min<float>(rect.top, win_rect.top);
		win_rect.left = std::min<float>(rect.left, win_rect.left);
		win_rect.bottom = std::max<float>(rect.bottom, win_rect.bottom);
		win_rect.right = std::max<float>(rect.right, win_rect.right);
	}
	return win_rect;
}

JGUIRect DockSystem::DockResizeLine::RightArea()
{
	JGUIRect win_rect;
	win_rect.top = 99999999;
	win_rect.bottom = -1;
	win_rect.left = 99999999;
	win_rect.right = -1;
	for (auto& dock_pair : m_RightDock)
	{
		auto rect = dock_pair.second->rect;
		win_rect.top = std::min<float>(rect.top, win_rect.top);
		win_rect.left = std::min<float>(rect.left, win_rect.left);
		win_rect.bottom = std::max<float>(rect.bottom, win_rect.bottom);
		win_rect.right = std::max<float>(rect.right, win_rect.right);
	}
	return win_rect;
}

JGUIRect DockSystem::DockResizeLine::LeftArea()
{
	JGUIRect win_rect;
	win_rect.top = 99999999;
	win_rect.bottom = -1;
	win_rect.left = 99999999;
	win_rect.right = -1;
	for (auto& dock_pair : m_LeftDock)
	{
		auto rect = dock_pair.second->rect;
		win_rect.top = std::min<float>(rect.top, win_rect.top);
		win_rect.left = std::min<float>(rect.left, win_rect.left);
		win_rect.bottom = std::max<float>(rect.bottom, win_rect.bottom);
		win_rect.right = std::max<float>(rect.right, win_rect.right);
	}
	return win_rect;
}
