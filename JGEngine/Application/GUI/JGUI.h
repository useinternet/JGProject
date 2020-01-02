#pragma once

#include "CommonCore.h"
#include "JGUIEvent.h"

namespace IE
{
	class InputEngine;
}
/*
- JGUIWindow : public JGUIObject

1. 두가지 윈도우
   - Client 안에서 윈도우 띄우기
   - Client 밖에서 윈도우 띄우기
   1) Client 안일 경우
      DxScreen와 JWindow를 공유
   2) Client 밖일 경우
      DxScreen와 JWindow를 독립

   결국 JGUI에서의 윈도우 = GUI Elememt의 집합


- JGUIClient
1. DxScreen과 JWindow를 가지고있다.
2. 




// 오브젝트 관련 //
1. GUI오브젝트는 각각 JGUI에서 최종 관리


// Focus

*/
class JGUIObject;
class JGUIWindow;
class JGUIScreen;

enum EJGUI_WindowFlags
{
	JGUI_WindowFlag_None      = 0x000000,
};
enum EJGUI_SubmitCmdListPriority
{
	JGUI_SubmitCmdListPriority_Default = 999
};
enum EJGUI_WindowPriority : uint64_t
{
	JGUI_WindowPriority_TopMost    = 40000000000000000,
	JGUI_WindowPriority_Focus      = 30000000000000000,
	JGUI_WindowPriority_None       = 20000000000000000,
	JGUI_WindowPriority_BottomMost = 10000000000000000,
};
// 포커스 플래그
/*
JGUI_FocusFlag_Only : 오직 자기 자신만 포함해 포커스 유무를 판단합니다.
JGUI_FocusFlag_All  : 자기자신 및 자식을 포함해 포커스 유무를 판단합니다.
*/
#define JGUI_FOCUS_RANGE 3
enum EJGUI_FocusFlags
{
	JGUI_FocusFlag_Only,
	JGUI_FocusFlag_All,
};

class JGUIRect
{
public:
	float left = 0.0f;
	float top = 0.0f;
	float right = 0.0f;
	float bottom = 0.0f;

	float width = 0.0f;
	float height = 0.0f;
};


enum
{
	JGUI_MOUSETRACKFLAG_MOUSEHOVER = 0,
	JGUI_MOUSETRACKFLAG_MOUSELEAVE = 1
};
#define JGUI_DEFAULT_HOVERTRACKTIME 0.01f
#define JGUI_DEFAULT_LEAVETRACKTIME 0.01f
class JGUIMouseTrack
{
	friend class JGUI;
public:
	class JGUIWindow*    win = nullptr;
	class JGUIComponent* com = nullptr;
	uint32_t       flag = 0;
	float time = 0.01f;
private:
	bool  start = false;
	float tick  = 0.0f;
};

class JGUI
{
public:
	static void        DestroyObject(JGUIObject* obj);

	template<typename ComponentType>
	static ComponentType* CreateJGUIComponent(const std::string& name, JGUIWindow* owner_window)
	{
		auto& objPool = sm_GUI->m_ObjectPool;
		auto& id_queue = sm_GUI->m_IDQueue;
		auto& id_offset = sm_GUI->m_IDOffset;
		auto obj = std::make_shared<ComponentType>();
		uint64_t id = -1;
		if (!id_queue.empty())
		{
			id = id_queue.front();
			id_queue.pop();
		}
		else
		{
			id = id_offset++;
		}
		obj->m_ID = id;
		obj->Init(name, owner_window);
		obj->JGUIAwake();
		objPool[obj.get()] = obj;
		return obj.get();
	}
	template<typename WindowType>
	static WindowType* CreateJGUIWindow(const std::string& name, bool is_new_window = false)
	{
		auto& objPool   = sm_GUI->m_ObjectPool;
		auto& id_queue  = sm_GUI->m_IDQueue;
		auto& id_offset = sm_GUI->m_IDOffset;
		auto obj = std::make_shared<WindowType>();
		uint64_t id = -1;
		if (!id_queue.empty())
		{
			id = id_queue.front();
			id_queue.pop();
		}
		else
		{
			id = id_offset++;
		}
		obj->m_ID = id;
		obj->Init(name, is_new_window);
		obj->JGUIAwake();
		objPool[obj.get()] = obj;
		return obj.get();
	}
	static JGUIScreen* ReqeustRegisterJGUIScreen(JGUIWindow* window, const std::string& name, uint32_t width, uint32_t height,
		uint32_t startX, uint32_t startY);
	static JGUIWindow* FindRootJGUIWindow(HWND hWnd);
	static JGUIScreen* FindJGUIScreen(HWND hWnd);
	static void        RequestDestroyScreen(HWND hWnd);
	static void        ClearExpectedDestroyScreen();
	static JGUIWindow* GetMainWindow();
	static JVector2Int GetCursorPos();
	static JVector2Int GetMousePos(HWND hWnd);
	static bool        GetKeyDown(JGUIWindow* owner_window, KeyCode code);
	static bool        GetKeyUp(JGUIWindow* owner_window, KeyCode code);
	static void RegisterMouseTrack(const JGUIMouseTrack& mt);
public:
	JGUI(IE::InputEngine* input);
	void Update();
private:
	LRESULT WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT WindowProcResize(JGUIWindow* window, WPARAM wParam, LPARAM lParam);
	LRESULT WindowSetFocus(JGUIWindow* window, WPARAM wParam);
	LRESULT WindowKillFocus(JGUIWindow* window, WPARAM wParam);
	LRESULT WindowKeyDownEvent(WPARAM wParam);
	LRESULT WindowKeyUpEvent(WPARAM wParam);
	LRESULT WindowMouseMove(JGUIWindow* window, LPARAM lParam);
	LRESULT WindowMouseHover(JGUIWindow* window);
	LRESULT WindowMouseLeave(JGUIWindow* window);
private:
	static JGUI* sm_GUI;
	using JGUILoadFunc = JGUIObject * (*)(const std::string&);
	using JGUIMainWindowFunc = JGUIWindow * (*)(const std::string&);
	using JGUIWindowFunc = JGUIWindow * (*)(const std::string&, bool);
	using JGUIObjectPool = std::unordered_map<JGUIObject*, std::shared_ptr<JGUIObject>>;
	using JGUIScreenPool = std::unordered_map<HWND,
		std::pair<JGUIWindow*, std::shared_ptr<JGUIScreen>>>;
	JGUIObjectPool   m_ObjectPool;
	JGUIScreenPool   m_ScreenPool;
	Plugin           m_Plugin;
	std::queue<HWND> m_ExpectedDestroyWindow;
	JGUIWindow*      m_MainWindow = nullptr;
	uint64_t             m_IDOffset = 0;
	std::queue<uint64_t> m_IDQueue;
	std::unordered_map<JGUIObject*, JGUIMouseTrack> m_MouseTrackMap;
	IE::InputEngine*    m_Input = nullptr;
};





// 시작 윈도우 형태
#define JGUI_MAIN_WINFORM(ClassName) \
class ClassName; \
extern "C" __declspec(dllexport) \
inline ClassName* LoadMainWindowForm(const std::string& name) { \
    return JGUI::CreateJGUIWindow<##ClassName>(name, true); \
}\


#define JGUI_REGISTER_WINFORM(ClassName) \
class ClassName; \
	extern "C" __declspec(dllexport) \
	inline ClassName* Load##ClassName(const std::string& name, bool is_newLoad) { \
    return JGUI::CreateJGUIWindow<##ClassName>(name, is_newLoad); \
}\


#define JGUI_REGISTER_COMPONENT(ClassName) \
class ClassName; \
	extern "C" __declspec(dllexport) \
	inline ClassName* Load##ClassName(const std::string& name, JGUIWindow* owner_window) { \
    return JGUI::CreateJGUIComponent<##ClassName>(name, owner_window); \
}\





class JGUILayer
{

};
