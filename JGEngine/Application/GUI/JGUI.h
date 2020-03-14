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

// Common
enum EJGUI_Colider
{
	JGUI_Collider_Box = 0,
	JGUI_Collider_EmptyBox = 1,
};
enum EJGUI_RectTransform
{
	JGUI_RectTransform_Default,
	JGUI_RectTransform_Window,
};
enum EJGUI_RIMaterial
{
	JGUI_RIMaterial_Default,
	JGUI_RIMaterial_OneTexture,
	JGUI_RIMaterial_Text
};
enum EJGUI_RIMesh
{
	JGUI_RIMesh_Rectangle,
	JGUI_RIMesh_EmptyRectangle,
};
// Window
enum EJGUI_WindowFlags
{
	JGUI_WindowFlag_None         = 0x000000,
	JGUI_WindowFlag_NewLoad      = 0x000001,
	JGUI_WindowFlag_TitileBar    = 0x000002,
	JGUI_WindowFlag_EnableResize = 0x000004,
	JGUI_WindowFlag_TabBar       = 0x000008,
	JGUI_WindowFlag_MenuBar      = 0x000010,
	JGUI_WindowFlag_EnableDock   = 0x000020,
    JGUI_WindowFlag_MultiSwapChain = 0x000040,
};
enum EJGUI_SubmitCmdListPriority
{
	JGUI_SubmitCmdListPriority_Default = 999
};
enum EJGUI_WindowPriority : uint64_t
{
	JGUI_WindowPriority_TopMost    = 50000000000000000,
	JGUI_WindowPriority_Focus      = 40000000000000000,
	JGUI_WindowPriority_Child      = 30000000000000000,
	JGUI_WindowPriority_None       = 20000000000000000,
	JGUI_WindowPriority_BottomMost = 10000000000000000,
};

// Component
enum EJGUI_ComponentFlags
{
	JGUI_ComponentFlag_None               = 0x0000000000,
	JGUI_ComponentFlag_NoChild            = 0x0000000001,
	JGUI_ComponentFlag_NoParent           = 0x0000000002,
	JGUI_ComponentFlag_LockCreateFunction = 0x0000000004,
	JGUI_ComponentFlag_TopMost = 0x0000000008
};

enum EJGUI_Clip_Flags
{
	JGUI_Clip_Flag_None = 0x00,
	JGUI_Clip_Flag_X = 0x01,
	JGUI_Clip_Flag_Y = 0x02,
	JGUI_Clip_Flag_Reverse = 0x04
};
/* EJGUI_TextFlags : 텍스트가 텍스트 사각형 범위내에 벗어 날시 Flag */

enum EJGUI_Text_Flags
{
	JGUI_Text_Flag_None         = 0x000,
	JGUI_Text_Flag_Border       = 0x001,
};

enum EJGUI_Text_Drawing
{
	JGUI_Text_Drawing_Discard,       // 범위 내에 벗어날 때 그만큼 텍스트를 자른다.
	JGUI_Text_Drawing_NextLine,      // 범위 내에 벗어날 때 다음줄로 간다.
	JGUI_Text_Drawing_RightPushed,   // 범위 내에 벗어날 때 오른쪽으로 밀고 벗어난만큼 자른다.
	JGUI_Text_Drawing_DownPushed,    // 범위 내에 벗어날 때 아래쪽으로 밀고 벗어난만큼 자른다.
	JGUI_Text_Drawing_Ignore         // 범위 내에 벗어나도 무시한다.
};

enum EJGUI_Text_HAlignment
{
	JGUI_Text_HAlignment_Left,
	JGUI_Text_HAlignment_Right,
	JGUI_Text_HAlignment_Center,
};
enum EJGUI_Text_VAlignment
{
	JGUI_Text_VAlignment_Top,
	JGUI_Text_VAlignment_Bottom,
	JGUI_Text_VAlignment_Center,
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

	float width() const
	{
		return right - left;
	}
	float height() const
	{
		return bottom - top;
	}
	JVector2 center() const {
		JVector2 center;
		center.x = left + (width()  * 0.5f);
		center.y = top  + (height() * 0.5f);
		return center;
	}
	bool Contains(const JVector2 p)
	{
		if (p.x >= left && p.x <= right && p.y >= top && p.y <= bottom)
			return true;
		return false;
	}
	bool Contains(const JVector2Int p)
	{
		JVector2 fp((float)p.x, (float)p.y);
		return Contains(fp);
	}
	void Demical()
	{
		float intLeft = (int)left;
		float intRight = (int)right;
		float intTop = (int)top;
		float intBottom = (int)bottom;



		if (left - intLeft < 0.5f) left = intLeft;
		else left = intLeft + 1.0f;

		if (right - intRight < 0.5f) right = intRight;
		else right = intRight + 1.0f;

		if (top - intTop < 0.5f) top = intTop;
		else top = intTop + 1.0f;

		if (bottom - intBottom < 0.5f) bottom = intBottom;
		else bottom = intBottom + 1.0f;

	/*	left = (float)(int)left;
		top = (float)(int)top;
		right = (float)(int)right;
		bottom = (float)(int)bottom;*/
	}

	float Area()
	{
		return width()  *  height();
	}

};


enum
{
	JGUI_MOUSETRACKFLAG_MOUSEHOVER = 0,
	JGUI_MOUSETRACKFLAG_MOUSELEAVE = 1,

	JGUI_EXTRAEVENT_REPEAT,
	JGUI_EXTRAEVENT_DEFAULT,
	JGUI_EXTRAEVENT_EXIT
};




#define JGUI_DEFAULT_HOVERTRACKTIME 0.00f
#define JGUI_DEFAULT_LEAVETRACKTIME 0.00f

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
	static void  DestroyObject(JGUIObject* obj);

	template<typename ComponentType>
	static ComponentType* CreateJGUIComponent(const std::string& name, JGUIWindow* owner_window, EJGUI_ComponentFlags flag)
	{
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
		obj->m_Flags = flag;
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
	static int         GetWindowZOrder(JGUIWindow* win);
	static JGUIWindow* GetTopJGUIWindow();
	static JGUIWindow* GetNextJGUIWindow(JGUIWindow* win);
	static void RegisterDraggingWindow(JGUIWindow* win);
	static JGUIWindow* GetCurrentDraggingWindow();
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
	JGUIWindow*      m_DraggingWindow = nullptr;
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
