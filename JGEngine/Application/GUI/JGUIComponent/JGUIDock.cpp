#include "pch.h"
#include "JGUIDock.h"
#include "JGUIShape.h"
#include "JGUIRectTransform.h"
#include "JGUICollider.h"
#include "JGUITitleBar.h"
#include "GUI/JGUIForm/JGUIForm.h"
using namespace std;

void JGUIDock::Awake()
{
	m_DockTarget = CreateJGUIComponent<JGUIComponent>("DockTarget");
	m_DockTarget->GetTransform()->SetPivot(0.5f, 0.5f);
	for (int i = 0; i < DD_Count; ++i)
	{
		m_Docks[i] = CreateJGUIComponent<JGUIRectangle>("Dock_" + to_string(i));
		m_Docks[i]->SetColor(JColor(1, 1, 1, 0.5F));
		m_Docks[i]->RegisterCollider(GetOwnerWindow(), JGUI_Collider_Box);
		if (i == DD_Left || i == DD_Right || i == DD_Center || i == DD_Top || i == DD_Bottom)
		{
			m_Docks[i]->SetParent(m_DockTarget);
			m_Docks[i]->GetTransform()->SetPivot(0.5f, 0.5f);
		}
	}
	m_PrevRect = GetTransform()->GetRect();
	m_PrevRect.Demical();



	Setting();

	JGUIExtraEvent extra_e;
	extra_e.Bind(JGUI_EXTRAEVENT_REPEAT, this,
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

void JGUIDock::Start()
{
	
}

void JGUIDock::Resize(const JGUIResizeEvent& e)
{
	Setting();
	//m_DockTarget->GetTransform()->SetLocalPosition(e.width * 0.5f, e.height * 0.5f);
}

void JGUIDock::Tick(const JGUITickEvent& e)
{
	auto dragging_win = JGUI::GetCurrentDraggingWindow();

	if (dragging_win)
	{
		// 마우스 위치 알아내고
        // 존재하는 사각형에 Dock 아이콘을 위치시킨다.
		DockInfo* contained_dock = nullptr;
		auto mouse_pos = GetOwnerWindow()->GetMousePos();
		for (auto& info_pair : m_DockInfoPool)
		{
			auto rect = info_pair.second->rect;

			if (rect.Contains(mouse_pos))
			{
				m_DockTarget->GetTransform()->SetLocalPosition(rect.center());
				contained_dock = info_pair.second.get();
			}
		}


		DockDirection dir = DD_Count;
		for (int i = 0; i < DD_Count; ++i)
		{
			if (m_Docks[i]->GetCollider()->CheckInPoint(GetOwnerWindow()->GetMousePos()))
			{
				dir = (DockDirection)i;
				m_Docks[i]->SetColor(m_InteractionColor);
			}
			else
			{
				m_Docks[i]->SetColor(m_DefaultColor);
			}
		}


		if (JGUI::GetKeyUp(GetOwnerWindow(), KeyCode::LeftMouseButton))
		{
			DockExecuteByDirection(dir, (JGUIForm*)dragging_win, contained_dock);
		}
	}


}

void JGUIDock::RequestRemoveBinedDock(JGUIWindow* win)
{
	if (m_DockInfoMap.find(win) == m_DockInfoMap.end()) return;
	ArrangeDockInfo(m_DockInfoMap[win]);
}

void JGUIDock::Setting()
{
	auto dock_size = GetTransform()->GetSize();
	float mid_x = dock_size.x * 0.5f;
	float mid_y = dock_size.y * 0.5f;


	m_Docks[DD_Left]->GetTransform()->SetSize(30, 30);
	m_Docks[DD_Left]->GetTransform()->SetLocalPosition(-m_DockDistance,0.0F);

	m_Docks[DD_Right]->GetTransform()->SetSize(30, 30);
	m_Docks[DD_Right]->GetTransform()->SetLocalPosition(m_DockDistance, 0.0F);

	m_Docks[DD_Center]->GetTransform()->SetSize(30, 30);
	m_Docks[DD_Center]->GetTransform()->SetLocalPosition(0.0f, 0.0f);

	m_Docks[DD_Top]->GetTransform()->SetSize(30, 30);
	m_Docks[DD_Top]->GetTransform()->SetLocalPosition(0.0F, -m_DockDistance);

	m_Docks[DD_Bottom]->GetTransform()->SetSize(30, 30);
	m_Docks[DD_Bottom]->GetTransform()->SetLocalPosition(0.0f, m_DockDistance);



	if (m_DockInfoMap.find(GetOwnerWindow()) == m_DockInfoMap.end())
	{
		unique_ptr<DockInfo> dock_info = make_unique<DockInfo>();
		m_DockInfoMap[GetOwnerWindow()] = dock_info.get();
		m_DockInfoPool[dock_info.get()] = move(dock_info);
		m_DockInfoMap[GetOwnerWindow()]->win  = GetOwnerWindow();
	}
	if(m_DockInfoPool.size() == 1)
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

void JGUIDock::SettingDockInfo()
{
	auto  resize_rect = GetTransform()->GetRect();

	float wratio = resize_rect.width() / m_PrevRect.width();
	float hratio = resize_rect.height() / m_PrevRect.height();

	ENGINE_LOG_INFO("wratio : {0}   hratio : {1}", 
		to_string(wratio), to_string(hratio));
	for (auto& dock_info_pair : m_DockInfoPool)
	{
		auto info = dock_info_pair.first;
		auto rect = info->rect;
		auto result_rect = resize_rect;
		float left_distance   = rect.left - m_PrevRect.left;
		float top_distance    = rect.top - m_PrevRect.top;
		float right_distance  = m_PrevRect.right - rect.right;
		float bottom_distance = m_PrevRect.bottom - rect.bottom;

		float wratio = resize_rect.width() / m_PrevRect.width();
		float hratio = resize_rect.height() / m_PrevRect.height();
	
		result_rect.left   += (left_distance * wratio);
		result_rect.top    += (top_distance * hratio);
		result_rect.right  -= (right_distance * wratio);
		result_rect.bottom -= (bottom_distance * hratio);
		result_rect.Demical();
		ENGINE_LOG_INFO("{0}   left : {1}    top : {2}", info->win->GetName(),
			to_string(result_rect.left), to_string(result_rect.top));
		if (info->win != GetOwnerWindow())
		{
			info->win->GetTransform()->SetSize(result_rect.width(), result_rect.height());
			info->win->GetTransform()->SetLocalPosition(result_rect.left, result_rect.top);
		}
		info->rect = result_rect;
	}
}



void JGUIDock::DockExecuteByDirection(DockDirection dir, JGUIForm* dragging_win, DockInfo* containing_dock)
{
	if (dir >= DD_Count || dir < 0) return;
	dragging_win->SetParent(GetOwnerWindow());
	auto dock_info = make_unique<DockInfo>();
	containing_dock->rect.Demical();
	JGUIRect containing_rect = containing_dock->rect;
	// rect
	switch (dir)
	{
	case DD_Right:
		dragging_win->GetTransform()->SetSize(containing_rect.width() * 0.5f, containing_rect.height());
		dragging_win->GetTransform()->SetLocalPosition(containing_rect.left + (containing_rect.width() * 0.5f), containing_rect.top);
		containing_rect.right -= (containing_rect.width() * 0.5f);

	
;		break;
	case DD_Left:
		dragging_win->GetTransform()->SetSize(containing_rect.width() * 0.5f, containing_rect.height());
		dragging_win->GetTransform()->SetLocalPosition(containing_rect.left, containing_rect.top);
		containing_rect.left += (containing_rect.width() * 0.5f);
	
		break;
	case DD_Top:
		dragging_win->GetTransform()->SetSize(containing_rect.width(), containing_rect.height() * 0.5f);
		dragging_win->GetTransform()->SetLocalPosition(containing_rect.left, containing_rect.top);
		containing_rect.top += (containing_rect.height() * 0.5f);
		break;
	case DD_Bottom:
		dragging_win->GetTransform()->SetSize(containing_rect.width(), containing_rect.height() * 0.5f);
		dragging_win->GetTransform()->SetLocalPosition(containing_rect.left, containing_rect.top + (containing_rect.height() * 0.5f));
		containing_rect.bottom -= (containing_rect.height() * 0.5f);
		break;
	case DD_Center:
		if (containing_dock->win == GetOwnerWindow())
		{
			dragging_win->GetTransform()->SetSize(containing_rect.width(), containing_rect.height());
			dragging_win->GetTransform()->SetLocalPosition(containing_rect.left, containing_rect.top);

			m_DockInfoMap.erase(containing_dock->win);
			m_DockInfoPool.erase(containing_dock);
			containing_dock = nullptr;
		}
		else
		{
			// Tab 기능 구현 후 활성화
			// 비활성화
			return;
		}
		
		break;
	}
	dock_info->win  = dragging_win;
	dock_info->rect = dragging_win->GetTransform()->GetRect();

	m_DockInfoMap[dragging_win]     = dock_info.get();
	m_DockInfoPool[dock_info.get()] = move(dock_info);


	if (containing_dock && containing_dock->win && containing_dock->win->GetParent() != nullptr)
	{
		containing_dock->win->GetTransform()->SetSize(containing_rect.width(), containing_rect.height());
		containing_dock->win->GetTransform()->SetLocalPosition(containing_rect.left, containing_rect.top);
	}
	if (containing_dock)
	{
		containing_dock->rect = containing_rect;

		switch (dir)
		{
		case DD_Left:
		case DD_Top:
			CreateResizeLineByDirection(dir, m_DockInfoMap[dragging_win], containing_dock);
			break;
		case DD_Right:
		case DD_Bottom:
			CreateResizeLineByDirection(dir, containing_dock, m_DockInfoMap[dragging_win]);
			break;
		}
	}

	auto title_bar = dragging_win->GetTitleBar();
	if (title_bar)
	{
		title_bar->UnConnect();
	}
	m_Docks[dir]->SetColor(m_DefaultColor);
}

void JGUIDock::CreateResizeLineByDirection(DockDirection dir, DockInfo* l_t_info, DockInfo* r_b_info)
{
	//auto resize_line = CreateJGUIComponent<JGUIDockResizeLine>("DockResizeLine", JGUI_ComponentFlag_TopMost);
	//switch (dir)
	//{
	//case DD_Left:
	//case DD_Right:
	//	resize_line->BindDock(1, l_t_info, r_b_info);
	//	break;
	//case DD_Top:
	//case DD_Bottom:
	//	resize_line->BindDock(0, l_t_info, r_b_info);
	//	break;
	//}


}

void JGUIDock::ArrangeDockInfo(DockInfo* remove_info)
{
	vector<DockInfo*> lr_perfect_adjoin_dock;
	vector<DockInfo*> tb_prefect_adjoin_dock;
	vector<DockInfo*> lr_adjoin_dock;
	vector<DockInfo*> tb_adjoin_dock;

	int lcnt = 0;
	int rcnt = 0;
	int tcnt = 0;
	int bcnt = 0;
	JGUIRect rect1 = remove_info->rect;
	rect1.Demical();

	// 인접한 사각형 찾기
	for (auto& dock_info_pair : m_DockInfoMap)
	{
		if (remove_info == dock_info_pair.second) continue;
		auto rect2 = dock_info_pair.second->rect;
		rect2.Demical();

		// 왼쪽 오른쪽 인접
		if (rect1.left == rect2.right || rect1.right == rect2.left)
		{
			if (rect1.left == rect2.right) lcnt++;
			else rcnt++;
			// 완전 인접
			if (rect1.top == rect2.top && rect1.bottom == rect2.bottom)
				lr_perfect_adjoin_dock.push_back(dock_info_pair.second);

			if ((rect1.top <= rect2.top && rect2.top <= rect1.bottom) &&
				(rect1.top <= rect2.bottom && rect2.bottom <= rect1.bottom))
			{
				lr_adjoin_dock.push_back(dock_info_pair.second);
			}
		}
		// 위 아래 인접
		else if (rect1.top == rect2.bottom || rect1.bottom == rect2.top)
		{
			if (rect1.top == rect2.bottom) tcnt++;
			else bcnt++;
			// 완전 인접
			if (rect1.left == rect2.left && rect1.right == rect2.right)
				tb_prefect_adjoin_dock.push_back(dock_info_pair.second);

			// 부분 인접
			if ((rect1.left <= rect2.left && rect2.left <= rect1.right) &&
				(rect1.left <= rect2.right && rect2.right <= rect1.right))
			{
				tb_adjoin_dock.push_back(dock_info_pair.second);
			}
			
		}
	}

	if (!lr_perfect_adjoin_dock.empty() || !tb_prefect_adjoin_dock.empty())
	{
		DockInfo* result = nullptr;
		for (auto& dock : lr_perfect_adjoin_dock)
		{
			if (result == nullptr) result = dock;
			else
			{
			
				if (result->rect.Area() > dock->rect.Area() &&
					dock->win != GetOwnerWindow()) result = dock;
			}
		}
		for (auto& dock : tb_prefect_adjoin_dock)
		{
			if (result == nullptr) result = dock;
			else
			{

				if (result->rect.Area() > dock->rect.Area() &&
					dock->win != GetOwnerWindow()) result = dock;
			}
		}
		if (result)
		{
			// 합병
			auto rect1 = result->rect; 	rect1.Demical();
			auto rect2 = remove_info->rect; rect2.Demical();

			JGUIRect result_rect;

			if (rect1.left == rect2.left && rect1.right == rect2.right)
			{
				result_rect.left  = rect1.left;
				result_rect.right = rect1.right;
				result_rect.top   = min<float>(rect1.top, rect2.top);
				result_rect.bottom = max<float>(rect1.bottom, rect2.bottom);
			}
			if (rect1.top == rect2.top && rect1.bottom == rect2.bottom)
			{
				result_rect.left   = min<float>(rect1.left, rect2.left);
				result_rect.right  = max<float>(rect1.right, rect2.right);
				result_rect.top    = rect1.top;
				result_rect.bottom = rect1.bottom;
			}
			result->rect = result_rect;

			if (result->win && result->win->GetParent() != nullptr)
			{
				result->win->GetTransform()->SetSize(result->rect.width(), result->rect.height());
				result->win->GetTransform()->SetLocalPosition(result->rect.left, result->rect.top);
			}
		}
	}
	else
	{
		if (lr_adjoin_dock.size() < tb_adjoin_dock.size() || tb_adjoin_dock.empty())
		{
			// 왼쪽 오른쪽
			// 위아래 고정
			for (auto info : lr_adjoin_dock)
			{
				//
				// if (rect1.left == rect2.right || rect1.right == rect2.left)
				if (lcnt >= rcnt && rect1.right == info->rect.left) continue;
				if (lcnt < rcnt && rect1.left == info->rect.right) continue;



				info->rect.Demical();
				info->rect.left  = min<float>(rect1.left, info->rect.left);
				info->rect.right = max<float>(rect1.right, info->rect.right);
				if (info->win && info->win->GetParent() != nullptr)
				{
					info->win->GetTransform()->SetSize(info->rect.width(), info->rect.height());
					info->win->GetTransform()->SetLocalPosition(info->rect.left, info->rect.top);
				}
			}
		}
		if(lr_adjoin_dock.size() >= tb_adjoin_dock.size() || lr_adjoin_dock.empty())
		{
			// 위 아래
			// 왼쪽 오른쪽 고정
			for (auto info : tb_adjoin_dock)
			{
				if (tcnt >= bcnt && rect1.bottom == info->rect.top) continue;
				if (tcnt < bcnt && rect1.top == info->rect.bottom) continue;


				info->rect.Demical();
				info->rect.top = min<float>(rect1.top, info->rect.top);
				info->rect.bottom = max<float>(rect1.bottom, info->rect.bottom);
				if (info->win && info->win->GetParent() != nullptr)
				{
					info->win->GetTransform()->SetSize(info->rect.width(), info->rect.height());
					info->win->GetTransform()->SetLocalPosition(info->rect.left, info->rect.top);
				}
			}

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

void JGUIDock::JGUIDockResizeLine::Awake()
{
	RegisterCollider(GetOwnerWindow(), JGUI_Collider_Box);
	m_Line = CreateJGUIComponent<JGUIRectangle>("DockResizeLine");
	GetTransform()->AttachTransform(m_Line->GetTransform());
	m_Line->SetColor(JColor::Black());
}

void JGUIDock::JGUIDockResizeLine::MouseMove(const JGUIMouseMoveEvent& e)
{

	if (m_Line)
	{
		m_Line->SetColor(JColor::Red());
	}
}

void JGUIDock::JGUIDockResizeLine::MouseLeave()
{
	if (!m_IsResizeing)
	{
		m_Line->SetColor(JColor::Black());
	}
}

void JGUIDock::JGUIDockResizeLine::MouseBtDown(const JGUIKeyDownEvent& e)
{
	ENGINE_LOG_INFO("ResizeLine");
	if (!m_IsResizeing)
	{
		m_PrevMousePos = GetOwnerWindow()->GetMousePos();
		m_IsResizeing = true;
		JGUIExtraEvent extra_e;
		extra_e.Bind(JGUI_EXTRAEVENT_REPEAT, this, [&](JGUIExtraEvent& e)
		{

			auto mouse_pos = GetOwnerWindow()->GetMousePos();
			if (m_PrevMousePos != mouse_pos)
			{
				int delta_x = mouse_pos.x - m_PrevMousePos.x;
				int delta_y = mouse_pos.y - m_PrevMousePos.y;

				switch (m_LineType)
				{
				case HLine:
					// 위쪽 윈도우는 bottom만 +
					// 아래쪽 윈도우는 top만 -

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

					//
					break;
				case VLine:
					// 왼쪽 윈도우는 right만+
					// 오른쪽 윈도우는 left만-
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
}

void JGUIDock::JGUIDockResizeLine::BindDock(int line_type, DockInfo* l_t_info, DockInfo* r_b_info)
{
	m_LineType = line_type;
	if (m_Line) m_Line->SetActive(true);
	auto dock_pos = GetParent()->GetTransform()->GetPosition();
	switch (m_LineType)
	{
	case HLine:
		m_TopDock[l_t_info->win] = l_t_info;
		m_BottomDock[r_b_info->win] = r_b_info;

		GetTransform()->SetLocalPosition(l_t_info->rect.left, l_t_info->rect.bottom - dock_pos.y);
		GetTransform()->SetSize(l_t_info->rect.width(), m_Thick);
		break;
	case VLine:
		m_LeftDock[l_t_info->win] = l_t_info;
		m_RightDock[r_b_info->win] = r_b_info;

		
		GetTransform()->SetLocalPosition(l_t_info->rect.right, l_t_info->rect.top - dock_pos.y);
		GetTransform()->SetSize(m_Thick, l_t_info->rect.height());
		break;
	}

}
// 이거는 바인딩된 윈도우의 수가 줄어들을때 쓰는 함수
void JGUIDock::JGUIDockResizeLine::UnBindDock(DockInfo* dock_info)
{
	switch (m_LineType)
	{
	case HLine:
		if (m_TopDock.find(dock_info->win) != m_TopDock.end()) m_TopDock.erase(dock_info->win);
		if (m_BottomDock.find(dock_info->win) != m_BottomDock.end()) m_BottomDock.erase(dock_info->win);
		break;
	case VLine:
		if (m_LeftDock.find(dock_info->win) != m_LeftDock.end()) m_LeftDock.erase(dock_info->win);
		if (m_RightDock.find(dock_info->win) != m_RightDock.end()) m_RightDock.erase(dock_info->win);

		
		break;
	}


}
// 이거는 바인딩된 윈도의수가 늘어날때 쓰는 함수
void JGUIDock::JGUIDockResizeLine::AddBindDock(DockInfo* dock_info)
{
	switch (m_LineType)
	{
	case HLine:
		if (m_TopDock.find(dock_info->win) != m_TopDock.end()) m_TopDock[dock_info->win] = dock_info;
		if (m_BottomDock.find(dock_info->win) != m_BottomDock.end()) m_BottomDock[dock_info->win] = dock_info;
		break;
	case VLine:
		if (m_LeftDock.find(dock_info->win) == m_LeftDock.end()) m_LeftDock[dock_info->win] = dock_info;
		if (m_RightDock.find(dock_info->win) == m_RightDock.end()) m_RightDock[dock_info->win] = dock_info;

		
		break;
	}


}
bool JGUIDock::JGUIDockResizeLine::IsBindedDock(DockInfo* info)
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

void JGUIDock::JGUIDockResizeLine::Reset()
{
	if (m_Line) m_Line->SetActive(false);
	m_LeftDock.clear();
	m_RightDock.clear();
	m_TopDock.clear();
	m_BottomDock.clear();
	m_IsResizeing = false;

}
