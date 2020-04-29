#include "pch.h"
#include "JGUI.h"
#include "JGUIObject/JGUIObject.h"
#include "GUI/JGUIObject/JGUIElement.h"
#include "GUI/JGUIObject/JGUIWindow.h"


#include "JGUIFont/JGUIFontManager.h"
#include "JGUIScreen.h"
#include "RenderEngine.h"
#include "Object/DxObject/RenderDevice.h"

#include "InputEngine.h"
using namespace std;


JGUI* JGUI::sm_GUI = nullptr;
JGUI::JGUI(IE::InputEngine* input, const JGUIDesc& desc)
{
	if (sm_GUI == nullptr)
	{
		sm_GUI = this;
	}
	JWindowManager::BindWindowProcFunc(std::bind(&JGUI::WindowProc, this,
		placeholders::_1, placeholders::_2, placeholders::_3, placeholders::_4));
	m_Input = input;
	m_Plugin.Load(m_Desc.mainWindowdllPath);

	m_FontManager = make_shared<JGUIFontManager>();
	if (!m_FontManager->FontLoad(m_DefaultFont))
	{
		ENGINE_LOG_ERROR("Failed Default Font Load");
	}


	m_MainWindow = ((JGUIMainWindowFunc)m_Plugin.GetProcAddress("LoadMainWindowForm"))(m_Desc.mainWindowName);
	m_MainWindow->GetTransform()->SetSize(800, 600);
	m_MainHWND = m_MainWindow->GetRootWindowHandle();
}




JGUI* JGUI::Instance()
{
	return sm_GUI;
}

JGUI** JGUI::InstancePtr()
{
	return &sm_GUI;
}

void        JGUI::DestroyObject(JGUIObject* obj)
{
	auto& objqueue = sm_GUI->m_ExpectedDestroyObject;
	obj->JGUIDestroy();
	obj->m_IsDestroying = true;
	if (sm_GUI->m_DraggingWindow == obj)
		sm_GUI->m_DraggingWindow = nullptr;
	if (obj->GetID() != -1)
	{
		sm_GUI->m_IDQueue.push(obj->GetID());
	}
	objqueue.push(obj);
}

void JGUI::DestroyComponent(JGUIComponent* com)
{
	com->JGUIDestroy();
	sm_GUI->m_ExpectedDestroyComponent.push(com);
}

JGUIScreen* JGUI::ReqeustRegisterJGUIScreen(JGUIWindow* window)
{
	
	auto size = window->GetTransform()->GetSize();
	auto pos = window->GetTransform()->GetPosition();


	auto screen = make_shared<JGUIScreen>(window->GetName() + "_GUIScreen");
	bool result = screen->Init((uint32_t)size.x, (uint32_t)size.y, (uint32_t)pos.x, (uint32_t)pos.y);

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
	if(!queue.empty()) 	RE::RenderEngine::GetDevice()->Flush();
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
	if (owner_window == nullptr) return false;
	if (sm_GUI->m_Input->GetHandle() != owner_window->GetRootWindowHandle())
		return false;
	
	return sm_GUI->m_Input->GetKeyDown(code);
}

bool JGUI::GetKeyUp(JGUIWindow* owner_window, KeyCode code)
{
	if (owner_window == nullptr) return false;
	if (sm_GUI->m_Input->GetHandle() != owner_window->GetRootWindowHandle())
		return false;

	return sm_GUI->m_Input->GetKeyUp(code);
}

void JGUI::InputFlush()
{
	sm_GUI->m_Input->Flush();
}

void JGUI::InputMouseFlush()
{
	sm_GUI->m_Input->MouseFlush();
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
void JGUI::RegisterExtraEvent(const JGUIExtraEvent& e, uint64_t priority)
{
	auto& e_map = sm_GUI->m_ExtraEventMap;
	if (e.com)
	{
		e_map[e.com][priority].push_back(e);
	}
	if (e.win)
	{
		e_map[e.win][priority].push_back(e);
	}
}
JGUIFontManager* JGUI::GetJGUIFontManager()
{
	return sm_GUI->m_FontManager.get();
}
const std::string JGUI::GetDefaultFontName()
{
	return sm_GUI->m_DefaultFont;
}
int  JGUI::GetWindowZOrder(JGUIWindow* win)
{
	int Zorder = 0;
	HWND hWnd = GetTopWindow(NULL);
	
	while (win->GetRootWindowHandle() != hWnd)
	{
		Zorder++;
		hWnd = GetNextWindow(hWnd, GW_HWNDNEXT);
	}
	return Zorder;
}
JGUIWindow* JGUI::GetTopJGUIWindow()
{
	auto& screenPool = sm_GUI->m_ScreenPool;
	JGUIWindow* result = nullptr;
	int min_zorder = INT_MAX;

	for (auto& screen_pair : screenPool)
	{
		if (screen_pair.second.first == nullptr) continue;
		int zorder = GetWindowZOrder(screen_pair.second.first);
		if (zorder < min_zorder)
		{
			min_zorder = zorder;
			result = screen_pair.second.first;
		}
	}
	return result;
}
JGUIWindow* JGUI::GetNextJGUIWindow(JGUIWindow* win)
{
	if (win == nullptr) return GetTopJGUIWindow();
	auto& screenPool = sm_GUI->m_ScreenPool;
	int origin_zorder = JGUI::GetWindowZOrder(win);
	JGUIWindow* result = nullptr;
	map<int, JGUIWindow*> window_by_zorder;
	for (auto& screen_pair : screenPool)
	{
		if (screen_pair.second.first == nullptr) continue;
		if (screen_pair.second.first == win) continue;
		
		int zorder = GetWindowZOrder(screen_pair.second.first);
		if (zorder < origin_zorder) continue;
		window_by_zorder[zorder] = screen_pair.second.first;
	}
	if(!window_by_zorder.empty()) result = (window_by_zorder.begin())->second;
	

	return result;
}

void JGUI::RegisterDraggingWindow(JGUIWindow* win)
{
	sm_GUI->m_DraggingWindow = win;
}

JGUIWindow* JGUI::GetCurrentDraggingWindow()
{
	return sm_GUI->m_DraggingWindow;
}

void JGUI::Update()
{
	while (!m_ExpectedDestroyComponent.empty())
	{
		m_ComponentPool.erase(m_ExpectedDestroyComponent.front());
		m_ExpectedDestroyComponent.pop();
	}


	while (!m_ExpectedDestroyObject.empty())
	{
		m_MouseTrackMap.erase(m_ExpectedDestroyObject.front());
		m_ExtraEventMap.erase(m_ExpectedDestroyObject.front());
		m_ObjectPool.erase(m_ExpectedDestroyObject.front());
		m_ExpectedDestroyObject.pop();
	}


	while (!m_ExpectedCreateObject.empty())
	{
		auto obj = m_ExpectedCreateObject.front();
		m_ExpectedCreateObject.pop();
		m_ObjectPool[obj.get()] = obj;
	}


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
		JGUIElement* com = nullptr;
		JGUIWindow* win = nullptr;
		bool check = false;
		if (iter->second.com)
		{
			com = iter->second.com;
			collider = com->GetCollider();
			ownerwin = com->GetOwnerWindow();
			check = collider->CheckInPoint(ownerwin->GetMousePos());
		}
		if (iter->second.win)
		{
			win = iter->second.win;
			check = win->GetCollider()->CheckInPoint(JGUI::GetCursorPos());
		}
		

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

	for(auto& w_pair : m_ScreenPool)
	{
		auto window = w_pair.second.first;
		if (window == nullptr) continue;
		if (!window->IsActive()) continue;
		if (!window->IsExecuteStartFunc()) window->JGUIStart();
	

		JGUITickEvent e;
		e.deltaTime = GlobalLinkData::GetTick();
		e.totalTime = GlobalLinkData::GetTotalTime();
		e.fps = GlobalLinkData::GetFPS();
		window->JGUITick(e);
	}


	for (auto iter = m_ExtraEventMap.begin(); iter != m_ExtraEventMap.end();)
	{
		if (iter->second.empty())
		{
			iter = m_ExtraEventMap.erase(iter);
		}
		else
		{
			

			for (auto e_array = iter->second.begin();  e_array != iter->second.end();)
			{
				if (e_array->second.empty())
				{
					e_array = iter->second.erase(e_array);
				}
				else
				{
					for (auto _e = e_array->second.begin(); _e != e_array->second.end();)
					{
						switch (_e->flag)
						{
						case JGUI_EXTRAEVENT_DEFAULT:
							if (_e->bindedFunc) _e->bindedFunc(*_e);
							_e = e_array->second.erase(_e);
							break;
						case JGUI_EXTRAEVENT_REPEAT:
							if (_e->bindedFunc) _e->bindedFunc(*_e);
							++_e;
							break;
						case JGUI_EXTRAEVENT_EXIT:
							_e = e_array->second.erase(_e);
						}
					}
					++e_array;
				}
			
			}
			++iter;
		}
	}
}


LRESULT JGUI::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	JGUIWindow* currentWindow = FindRootJGUIWindow(hWnd);
	TRACKMOUSEEVENT tme;
	static bool bin = false;
	//int delta = GET_WHEEL_DELTA_WPARAM(wParam);
	switch (msg)
	{
	case WM_SIZE:
		return WindowProcResize(currentWindow, wParam, lParam);
	case WM_SETFOCUS:
		return WindowSetFocus(currentWindow, wParam);
	case WM_KILLFOCUS:
		return WindowKillFocus(currentWindow, wParam);
	case WM_CHAR:
		return WindowChar(currentWindow, wParam, lParam);
	case WM_MOUSEWHEEL:
		return WindowMouseWheel(currentWindow, wParam);
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
	if (e.width == 0.0f || e.height == 0.0f) return 0;
	window->GetTransform()->SetSize(e.width, e.height);
	return 0;
}
LRESULT JGUI::WindowSetFocus(JGUIWindow* window, WPARAM wParam)
{
	if (window == nullptr) return 0;
	JGUIFocusEnterEvent e;
	auto prev_focus_window = FindRootJGUIWindow((HWND)wParam);
	e.prevFocus = prev_focus_window;
	window->JGUIFocusEnter(e);
	return 0;
}
LRESULT JGUI::WindowKillFocus(JGUIWindow* window, WPARAM wParam)
{
	if (window == nullptr) return 0;
	JGUIFocusExitEvent e;
	auto next_focus_window = FindRootJGUIWindow((HWND)wParam);
	e.nextFocus = next_focus_window;
	window->JGUIFocusExit(e);
	return 0;
}
LRESULT JGUI::WindowChar(JGUIWindow* window, WPARAM wParam, LPARAM lParam)
{
	if (wParam > 128) return 0;
	auto str = (TCHAR)wParam;
	JGUICharEvent e;
	char c;
	wcstombs(&c, (wchar_t*)& str, 1);
	e.str = c;
	window->JGUIChar(e);
	return 0;
}
LRESULT JGUI::WindowMouseWheel(JGUIWindow* window, WPARAM wParam)
{
	
	int delta = GET_WHEEL_DELTA_WPARAM(wParam);

	JGUIMouseWheelEvent e;
	e.delta = delta / WHEEL_DELTA;
	window->JGUIMouseWheel(e);

	return 0;
}
LRESULT  JGUI::WindowKeyDownEvent(WPARAM wParam)
{
	//m_Input->NotifyKeyDown(KeyCode(wParam));
	return 0;
}

LRESULT JGUI::WindowKeyUpEvent(WPARAM wParam)
{
	//m_Input->NotifyKeyUp(KeyCode(wParam));
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

