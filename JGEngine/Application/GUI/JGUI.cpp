#include "pch.h"
#include "JGUI.h"
#include "JGUIObject/JGUIObject.h"

#include "JGUIComponent/JGUICollider.h"
#include "JGUIScreen.h"
#include "RenderEngine.h"
#include "JGUIObject/JGUIWindow.h"
#include "InputEngine.h"
using namespace std;


JGUI* JGUI::sm_GUI = nullptr;
JGUI::JGUI(IE::InputEngine* input)
{
	if (sm_GUI == nullptr)
	{
		sm_GUI = this;
	}
	JWindowManager::BindWindowProcFunc(std::bind(&JGUI::WindowProc, this,
		placeholders::_1, placeholders::_2, placeholders::_3, placeholders::_4));
	m_Input = input;
	m_Plugin.Load("JGEngine.dll");
	m_MainWindow = ((JGUIMainWindowFunc)m_Plugin.GetProcAddress("LoadMainWindowForm"))("JGUI");
}




void        JGUI::DestroyObject(JGUIObject* obj)
{
	auto& objPool = sm_GUI->m_ObjectPool;
	obj->JGUIDestroy();
	if (obj->GetID() != -1)
	{
		sm_GUI->m_IDQueue.push(obj->GetID());
	}
	sm_GUI->m_MouseTrackMap.erase(obj);
	objPool.erase(obj);

}

JGUIScreen* JGUI::ReqeustRegisterJGUIScreen(JGUIWindow* window, const std::string& name, uint32_t width, uint32_t height,
	uint32_t startX, uint32_t startY)
{
	auto screen = make_shared<JGUIScreen>(name);
	bool result = screen->Init(width, height, startX, startY);

	if (!result)
		return nullptr;
	HWND hWnd = screen->GetHandle();
	auto& screenPool = sm_GUI->m_ScreenPool;
	screenPool[hWnd].first = window;
	screenPool[hWnd].second = screen;
	return screen.get();
}
JGUIWindow* JGUI::FindRootJGUIWindow(HWND hWnd)
{
	auto& screenPool = sm_GUI->m_ScreenPool;
	return screenPool[hWnd].first;
}
JGUIScreen* JGUI::FindJGUIScreen(HWND hWnd)
{
	auto& screenPool = sm_GUI->m_ScreenPool;
	return screenPool[hWnd].second.get();
}
void JGUI::RequestDestroyScreen(HWND hWnd)
{
	auto& queue = sm_GUI->m_ExpectedDestroyWindow;
	queue.push(hWnd);
}

void JGUI::ClearExpectedDestroyScreen()
{
	auto& queue = sm_GUI->m_ExpectedDestroyWindow;
	auto& screenPool = sm_GUI->m_ScreenPool;
	while (!queue.empty())
	{
		auto hWnd = queue.front(); queue.pop();
		screenPool.erase(hWnd);
	}
}

JGUIWindow* JGUI::GetMainWindow()
{
	return sm_GUI->m_MainWindow;
}

JVector2Int JGUI::GetCursorPos()
{
	JVector2Int result;
	POINT P;
	::GetCursorPos(&P);
	result.x = P.x;
	result.y = P.y;
	return result;
}

JVector2Int JGUI::GetMousePos(HWND hWnd)
{
	POINT P;
	::GetCursorPos(&P);
	::ScreenToClient(hWnd, &P);
	JVector2Int result = { P.x, P.y };
	return result;
}

bool JGUI::GetKeyDown(JGUIWindow* owner_window, KeyCode code)
{
	if (sm_GUI->m_Input->GetHandle() != owner_window->GetRootWindowHandle())
		return false;
	
	return sm_GUI->m_Input->GetKeyDown(code);
}

bool JGUI::GetKeyUp(JGUIWindow* owner_window, KeyCode code)
{
	if (sm_GUI->m_Input->GetHandle() != owner_window->GetRootWindowHandle())
		return false;

	return sm_GUI->m_Input->GetKeyUp(code);
}

void JGUI::RegisterMouseTrack(const JGUIMouseTrack& mt)
{
	auto& trcak_map = sm_GUI->m_MouseTrackMap;

	if (mt.com)
	{
		trcak_map[mt.com] = mt;
	}
	else if (mt.win)
	{
		trcak_map[mt.win] = mt;
	}
}

void JGUI::Update()
{
	static int focus_cnt = 0;
	// Focus
	auto focus_window = FindRootJGUIWindow(GetFocus());
	if (focus_window == nullptr) focus_cnt++;
	else focus_cnt = 0;

	if (focus_window)
	{
		focus_window->JGUIOnFocus();
		auto kdownList = m_Input->GetKeyBoardDownList();
		auto kupList = m_Input->GetKeyBoardUpList();
		auto mdownList = m_Input->GetMouseBtDownList();
		auto mupList = m_Input->GetMouseBtUpList();

		for (auto k : kdownList)
		{
			JGUIKeyDownEvent e;
			e.Code = k;
			focus_window->JGUIKeyDown(e);
		}
		for (auto k : kupList)
		{
			JGUIKeyUpEvent e;
			e.Code = k;
			focus_window->JGUIKeyUp(e);
		}
		for (auto k : mdownList)
		{
			JGUIKeyDownEvent e;
			e.Code = k;
			focus_window->JGUIMouseBtDown(e);
		}
		for (auto k : mupList)
		{
			JGUIKeyUpEvent e;
			e.Code = k;
			focus_window->JGUIMouseBtUp(e);
		}
	}

	for (auto iter = m_MouseTrackMap.begin(); iter != m_MouseTrackMap.end();)
	{
		JGUIWindow* ownerwin = nullptr;
		JGUICollider* collider = nullptr;
		JGUIComponent* com = nullptr;
		JGUIWindow* win = nullptr;

		if (iter->second.com)
		{
			com = iter->second.com;
			collider = com->GetCollider();
			ownerwin = com->GetOwnerWindow();
		}
		if (iter->second.win)
		{
			win = iter->second.win;
			ownerwin = JGUI::FindRootJGUIWindow(win->GetRootWindowHandle());
			collider = win->GetPanel()->GetCollider();
		}
		if (focus_cnt > JGUI_FOCUS_RANGE)
		{
			if (iter->second.flag == JGUI_MOUSETRACKFLAG_MOUSELEAVE)
			{
				if (com) com->JGUIMouseLeave();
				else if (win) win->JGUIMouseLeave();
			}
			if (com) com->m_IsMouseTracking = false;
			if (win) win->m_IsTracking = false;
			iter = m_MouseTrackMap.erase(iter);
		}
		else
		{
			HWND focusHandle = ownerwin->GetRootWindowHandle();
			bool check = collider->CheckInPoint(JGUI::GetMousePos(focusHandle));
			switch (iter->second.flag)
			{
			case JGUI_MOUSETRACKFLAG_MOUSEHOVER:
				if (check) iter->second.start = true;
				break;
			case JGUI_MOUSETRACKFLAG_MOUSELEAVE:
				if (check == false) iter->second.start = true;
				break;
			}

			if (iter->second.start) iter->second.tick += GlobalLinkData::GetTick();
			if (iter->second.tick > iter->second.time)
			{
				auto flag = iter->second.flag;
				iter = m_MouseTrackMap.erase(iter);
				switch (flag)
				{
				case JGUI_MOUSETRACKFLAG_MOUSEHOVER:
					if (com) com->JGUIMouseHover();
					else if (win) win->JGUIMouseHover();
					break;
				case JGUI_MOUSETRACKFLAG_MOUSELEAVE:
					if (com) com->JGUIMouseLeave();
					else if (win) win->JGUIMouseLeave();
					break;
				}
			}
			else ++iter;
		}
	}


	// Tick
	for (auto& obj_pair : m_ObjectPool)
	{
		auto obj = obj_pair.second;
		if (!obj->IsActive())
			continue;
		if (!obj->IsExecuteStartFunc())
			obj->JGUIStart();


		JGUITickEvent e;
		e.deltaTime = GlobalLinkData::GetTick();
		e.totalTime = GlobalLinkData::GetTotalTime();
		e.fps = GlobalLinkData::GetFPS();
		obj->JGUITick(e);
	}
}


LRESULT JGUI::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	JGUIWindow* currentWindow = FindRootJGUIWindow(hWnd);
	TRACKMOUSEEVENT tme;
	static bool bin = false;
	switch (msg)
	{
	case WM_SIZE:
		return WindowProcResize(currentWindow, wParam, lParam);
	case WM_SETFOCUS:
		return WindowSetFocus(currentWindow, wParam);
	case WM_KILLFOCUS:
		return WindowKillFocus(currentWindow, wParam);
	case WM_KEYDOWN:
		return WindowKeyDownEvent(wParam);
	case WM_KEYUP:
		return WindowKeyUpEvent(wParam);
	case WM_LBUTTONDOWN:
		return WindowKeyDownEvent(KeyCode::LeftMouseButton);
	case WM_LBUTTONUP:
		return WindowKeyUpEvent(KeyCode::LeftMouseButton);
	case WM_RBUTTONDOWN:
		return WindowKeyDownEvent(KeyCode::RightMouseButton);
	case WM_RBUTTONUP:
		return WindowKeyUpEvent(KeyCode::RightMouseButton);
	case WM_MBUTTONDOWN:
		return WindowKeyDownEvent(KeyCode::MouseWheelButton);
	case WM_MBUTTONUP:
		return WindowKeyUpEvent(KeyCode::MouseWheelButton);
	case WM_MOUSEMOVE:
		if (bin == false)
		{
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_HOVER;
			tme.hwndTrack = hWnd;
			tme.dwHoverTime = 10;
			TrackMouseEvent(&tme);
		}
		return WindowMouseMove(currentWindow, lParam);
	case WM_MOUSEHOVER:
		bin = true;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = hWnd;
		tme.dwHoverTime = 10;
		TrackMouseEvent(&tme);
		return WindowMouseHover(currentWindow);
	case WM_MOUSELEAVE:
		bin = false;
		return WindowMouseLeave(currentWindow);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT JGUI::WindowProcResize(JGUIWindow* window, WPARAM wParam, LPARAM lParam)
{
	if (window == nullptr) return 0;

	JGUIResizeEvent e;
	e.width = (float)LOWORD(lParam);
	e.height = (float)HIWORD(lParam);
	window->JGUIResize(e);
	return 0;
}
LRESULT JGUI::WindowSetFocus(JGUIWindow* window, WPARAM wParam)
{
	if (window == nullptr) return 0;
	JGUIFocusEnterEvent e;
	auto prev_focus_window = FindRootJGUIWindow((HWND)wParam);
	e.prevFocusWindow = prev_focus_window;
	window->JGUIFocusEnter(e);
	return 0;
}
LRESULT JGUI::WindowKillFocus(JGUIWindow* window, WPARAM wParam)
{
	if (window == nullptr) return 0;
	JGUIFocusHoverEvent e;
	auto next_focus_window = FindRootJGUIWindow((HWND)wParam);
	e.nextFocusWindow = next_focus_window;
	window->JGUIFocusHover(e);
	return 0;
}
LRESULT  JGUI::WindowKeyDownEvent(WPARAM wParam)
{
	m_Input->NotifyKeyDown(KeyCode(wParam));
	return 0;
}

LRESULT JGUI::WindowKeyUpEvent(WPARAM wParam)
{
	m_Input->NotifyKeyUp(KeyCode(wParam));
	return 0;
}

LRESULT JGUI::WindowMouseMove(JGUIWindow* window, LPARAM lParam)
{
	int x = (int)(short)LOWORD(lParam);
	int y = (int)(short)HIWORD(lParam);
	m_Input->NotifyMousePos(x, y);
	JGUIMouseMoveEvent e;
	e.pos = { (float)x,(float)y };
	auto delta =  m_Input->GetMouseDelta();
	e.delta = { (float)delta.x, (float)delta.y };
	window->JGUIMouseMove(e);
	return 0;
}

LRESULT JGUI::WindowMouseHover(JGUIWindow* window)
{
	window->JGUIMouseHover();
	return 0;
}

LRESULT JGUI::WindowMouseLeave(JGUIWindow* window)
{
	window->JGUIMouseLeave();
	return 0;
}

