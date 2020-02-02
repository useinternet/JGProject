#pragma once

#include "CommonCore.h"
#include "JGUIEvent.h"
#include <vector>
namespace IE
{
	class InputEngine;
}
class JGUIObject;
class JGUIWindow;
class JGUIScreen;
class JGUIFontManager;

template<typename JGUIFlag>
JGUIFlag operator|(JGUIFlag f1, JGUIFlag f2)
{
	return (JGUIFlag)((uint64_t)f1 | (uint64_t)f2);
}

template<typename JGUIFlag>
JGUIFlag operator&(JGUIFlag f1, JGUIFlag f2)
{
	return (JGUIFlag)((uint64_t)f1 & (uint64_t)f2);
}


template<typename JGUIFlag>
JGUIFlag operator~(JGUIFlag f1)
{
	return (JGUIFlag)~(uint64_t)f1;
}


// Window
enum EJGUI_WindowFlags
{
	JGUI_WindowFlag_None    = 0x000000,
	JGUI_WindowFlag_NewLoad = 0x000001,
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

// Component
enum EJGUI_ComponentInteractionFlags
{
	JGUI_ComponentInteractionFlag_None = 0x00000000,
	JGUI_ComponentInteractionFlag_Default = 0x00000001
};
//enum EJGUI_ComponentEventLockFlags
//{
//	JGUI_ComponentEventLockFlag_None = 0x0000,
//	JGUI_ComponentEventLockFlag_SendParent = 0x0001,
//	JGUI_ComponentEventLockFlag_SendChild  = 0x0002,
//	JGUI_ComponentEventLockFlag_Once       = 0x0004,
//};
enum EJGUI_ComponentEvents
{
	JGUI_ComponentEvent_Awake,
	JGUI_ComponentEvent_Start,
	JGUI_ComponentEvent_Tick,
	JGUI_ComponentEvent_Destroy,
	JGUI_ComponentEvent_Resize,
	JGUI_ComponentEvent_MouseMove,
	JGUI_ComponentEvent_MouseBtDown,
	JGUI_ComponentEvent_MouseBtUp,
	JGUI_ComponentEvent_MouseLeave,
	JGUI_ComponentEvent_MouseHover,
	JGUI_ComponentEvent_ParentUpdateNotification,
};

enum EJGUI_Component_Colider
{
	JGUI_Component_Colider_Box = 0,
	JGUI_Component_Colider_EmptyBox = 1,
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
	JGUI_MOUSETRACKFLAG_MOUSELEAVE = 1,

	JGUI_EXTRAEVENT_REPEAT,
	JGUI_EXTRAEVENT_DEFAULT,
	JGUI_EXTRAEVENT_EXIT
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

class JGUIExtraEvent
{
	friend class JGUI;
public:
	uint32_t flag = JGUI_EXTRAEVENT_DEFAULT;
	void Bind(uint32_t flag, JGUIComponent* com, const std::function<void(JGUIExtraEvent&)>& func) {
		this->com = com;
		bindedFunc = func;
		this->flag = flag;
	}
	void Bind(uint32_t flag, JGUIWindow* win, const std::function<void(JGUIExtraEvent&)>& func) {
		this->win = win;
		bindedFunc = func;
		this->flag = flag;
	}
private:
	class JGUIComponent* com = nullptr;
	class JGUIWindow* win = nullptr;
	std::function<void(JGUIExtraEvent&)> bindedFunc;
};
class JGUI
{
	
public:
	static float Gap() { return 10.0f; }
	static void        DestroyObject(JGUIObject* obj);

	template<typename ComponentType>
	static ComponentType* CreateJGUIComponent(const std::string& name, JGUIWindow* owner_window)
	{
		//auto& objPool = sm_GUI->m_ObjectPool;
		auto& objqueue = sm_GUI->m_ExpectedCreateObject;
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
		objqueue.push(obj);
		return obj.get();
	}
	template<typename WindowType>
	static WindowType* CreateJGUIWindow(const std::string& name, EJGUI_WindowFlags flags)
	{
		auto& objqueue = sm_GUI->m_ExpectedCreateObject;
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
		obj->Init(name, flags);
		obj->JGUIAwake();
		objqueue.push(obj);
		return obj.get();
	}
	static JGUIScreen* ReqeustRegisterJGUIScreen(JGUIWindow* window);
	static JGUIWindow* FindRootJGUIWindow(HWND hWnd);
	static JGUIScreen* FindJGUIScreen(HWND hWnd);
	static void        RequestDestroyScreen(HWND hWnd);
	static void        ClearExpectedDestroyScreen();
	static JGUIWindow* GetMainWindow();
	static JVector2Int GetCursorPos();
	static JVector2Int GetMousePos(HWND hWnd);
	static bool        GetKeyDown(JGUIWindow* owner_window, KeyCode code);
	static bool        GetKeyUp(JGUIWindow* owner_window, KeyCode code);
	static void        InputFlush();
	static void        InputMouseFlush();
	static void        RegisterMouseTrack(const JGUIMouseTrack& mt);
	static void        RegisterExtraEvent(const JGUIExtraEvent& e);
	static JGUIFontManager*  GetJGUIFontManager();
	static const std::string GetDefaultFontName();

public:
	JGUI(IE::InputEngine* input);
	void Update();
private:
	LRESULT WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT WindowProcResize(JGUIWindow* window, WPARAM wParam, LPARAM lParam);
	LRESULT WindowSetFocus(JGUIWindow* window, WPARAM wParam);
	LRESULT WindowKillFocus(JGUIWindow* window, WPARAM wParam);
	LRESULT WindowChar(JGUIWindow* window, WPARAM wParam, LPARAM lParam);
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
	std::queue<std::shared_ptr<JGUIObject>> m_ExpectedCreateObject;
	std::queue<JGUIObject*> m_ExpectedDestroyObject;


	JGUIWindow*      m_MainWindow = nullptr;
	HWND             m_MainHWND;
	uint64_t             m_IDOffset = 0;
	std::queue<uint64_t> m_IDQueue;


	std::unordered_map<JGUIObject*, JGUIMouseTrack> m_MouseTrackMap;
	std::unordered_map<JGUIObject*, JGUIExtraEvent> m_ExtraEventMap;
	std::shared_ptr<JGUIFontManager> m_FontManager;
	std::string m_DefaultFont = GlobalLinkData::_EngineConfig->InEngine("Fonts/Consolas.fnt");
	IE::InputEngine*    m_Input = nullptr;
};





// 시작 윈도우 형태

#define JGUI_MAIN_WINFORM(ClassName) \
class ClassName; \
extern "C" __declspec(dllexport) \
inline ClassName* LoadMainWindowForm(const std::string& name) { \
    return JGUI::CreateJGUIWindow<##ClassName>(name, JGUI_WindowFlag_NewLoad); \
}\


#define JGUI_MAIN_WINFORM_ADDFLAG(ClassName, flag) \
class ClassName; \
extern "C" __declspec(dllexport) \
inline ClassName* LoadMainWindowForm(const std::string& name) { \
    return JGUI::CreateJGUIWindow<##ClassName>(name, JGUI_WindowFlag_NewLoad | flag); \
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
