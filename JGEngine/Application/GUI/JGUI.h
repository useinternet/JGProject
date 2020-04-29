#pragma once

#include "JE.h"
#include "JGUIEvent.h"
#include <vector>
namespace IE
{
	class InputEngine;
}

// 기본



class JGUIObject;
class JGUIComponent;
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
enum EJGUI_Collider
{
	JGUI_Collider_Box = 0,
};
enum EJGUI_RIMaterial
{
	JGUI_RIMaterial_Default,
	JGUI_RIMaterial_OneTexture,
	JGUI_RIMaterial_Text
};
// Window
enum EJGUI_WindowFlags : uint64_t
{
	JGUI_WindowFlag_None           = 0x000000,
	JGUI_WindowFlag_NewLoad        = 0x000001,
	JGUI_WindowFlag_TitleBar       = 0x000002,
	JGUI_WindowFlag_EnableResize   = 0x000004,
	JGUI_WindowFlag_NoMove         = 0x000008,
	JGUI_WindowFlag_MenuBar        = 0x000010,
	JGUI_WindowFlag_EnableDock     = 0x000020,
    JGUI_WindowFlag_MultiSwapChain = 0x000040,
	JGUI_WindowFlag_Border         = 0x000080,
	JGUI_WindowFlag_TopMost        = 0x000100,
	JGUI_WindowFlag_BottomMost     = 0x000200,
	JGUI_WindowFlag_EnableClose    = 0x000400,
	JGUI_WindowFlag_EnableMinimize = 0x000800,
	JGUI_WindowFlag_EnableMaximize = 0x001000,
	JGUI_WindowFlag_HorizontalScrollbar = 0x002000,
	JGUI_WindowFlag_VerticalScrollbar = 0x004000,
};

// MainWindow 타이틀, resize,

#define JGUI_WindowFlag_MainWindow \
JGUI_WindowFlag_TitleBar       | JGUI_WindowFlag_EnableResize | JGUI_WindowFlag_Border         | \
JGUI_WindowFlag_EnableDock     | JGUI_WindowFlag_EnableClose  | JGUI_WindowFlag_EnableMaximize | \
JGUI_WindowFlag_EnableMinimize | JGUI_WindowFlag_MenuBar      | JGUI_WindowFlag_NewLoad          \



#define JGUI_WindowFlag_ChildWindow    JGUI_WindowFlag_TitleBar | JGUI_WindowFlag_EnableResize | JGUI_WindowFlag_Border   | JGUI_WindowFlag_EnableClose
#define JGUI_WindowFlag_MainTabWindow  JGUI_WindowFlag_NoMove   | JGUI_WindowFlag_EnableDock | JGUI_WindowFlag_BottomMost | JGUI_WindowFlag_Border
#define JGUI_WindowFlag_ChildTabWindow JGUI_WindowFlag_NoMove   | JGUI_WindowFlag_BottomMost | JGUI_WindowFlag_Border
#define JGUI_WindowFlag_DockWindow     JGUI_WindowFlag_TitleBar | JGUI_WindowFlag_Border     | JGUI_WindowFlag_BottomMost

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
enum EJGUI_ElementFlags
{
	JGUI_ElementFlag_None               = 0x0000000000,
};
enum EJGUI_Canvas_Flags
{
	JGUI_CanvasFlag_None    = 0,
	JGUI_CanvasFlag_TopMost = 2,
	JGUI_CanvasFlag_Overlay = 4
};


enum EJGUI_Clip_Flags
{
	JGUI_Clip_Flag_None = 0x00,
	JGUI_Clip_Flag_X = 0x01,
	JGUI_Clip_Flag_Y = 0x02,
	JGUI_Clip_Flag_Reverse = 0x04
};
/* EJGUI_TextFlags : 텍스트가 텍스트 사각형 범위내에 벗어 날시 Flag */


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

class JGENGINE_API JGUIRect
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
	bool Contains(const JVector2& p)
	{
		if (p.x >= left && p.x <= right && p.y >= top && p.y <= bottom)
			return true;
		return false;
	}
	bool Contains(const JVector2Int& p)
	{
		JVector2 fp((float)p.x, (float)p.y);
		return Contains(fp);
	}
	bool Overlap(const JGUIRect& r)
	{
		if (Contains(JVector2(r.left, r.top))) return true;
		if (Contains(JVector2(r.right, r.top))) return true;
		if(Contains(JVector2(r.left, r.bottom))) return true;
		if (Contains(JVector2(r.right, r.bottom))) return true;

		return false;

	}
	void Demical()
	{
		float intLeft = (float)(int)left;
		float intRight = (float)(int)right;
		float intTop = (float)(int)top;
		float intBottom = (float)(int)bottom;



		if (left - intLeft < 0.5f) left = intLeft;
		else left = intLeft + 1.0f;

		if (right - intRight < 0.5f) right = intRight;
		else right = intRight + 1.0f;

		if (top - intTop < 0.5f) top = intTop;
		else top = intTop + 1.0f;

		if (bottom - intBottom < 0.5f) bottom = intBottom;
		else bottom = intBottom + 1.0f;

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

class JGENGINE_API JGUIMouseTrack
{
	friend class JGUI;
public:
	class JGUIWindow*    win = nullptr;
	class JGUIElement* com = nullptr;
	uint32_t       flag = 0;
	float time = 0.01f;
private:
	bool  start = false;
	float tick  = 0.0f;
};

class JGENGINE_API JGUIExtraEvent
{
	friend class JGUI;
public:
	uint32_t flag = JGUI_EXTRAEVENT_DEFAULT;
	void Bind(uint32_t flag, JGUIElement* com, const std::function<void(JGUIExtraEvent&)>& func) {
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
	class JGUIElement* com = nullptr;
	class JGUIWindow* win = nullptr;
	std::function<void(JGUIExtraEvent&)> bindedFunc;
	uint64_t id = 0;
};

class JGENGINE_API JGUIDesc
{
public:
	std::vector<std::string> dllPaths;
	std::string mainWindowName    = "JGUI";
	std::string mainWindowdllPath = "JGEditor.dll";
	std::string configFilePath    = "none";
};

class JGENGINE_API JGUI
{
public:
	static JGUI*  Instance();
	static JGUI** InstancePtr();
	static float Gap() { return 10.0f; }
	static void  DestroyObject(JGUIObject* obj);
	static void  DestroyComponent(JGUIComponent* com);


	template<typename ComType>
	static ComType* CreateJGUIComponent(const std::string& name)
	{
		auto com = std::make_shared<ComType>();
		com->SetName(name);

		auto result = com.get();
		sm_GUI->m_ComponentPool[com.get()] = com;

		return result;
	}

	template<typename ElementType>
	static ElementType* CreateJGUIElement(const std::string& name, JGUIWindow* owner_window, EJGUI_ElementFlags flag)
	{
		auto& objqueue = sm_GUI->m_ExpectedCreateObject;
		auto& id_queue = sm_GUI->m_IDQueue;
		auto& id_offset = sm_GUI->m_IDOffset;
		auto obj = std::make_shared<ElementType>();

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
		obj->m_OwnerWindow = owner_window;
		obj->SetName(name);
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
		obj->m_IsWindow = true;
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
		obj->m_Flags = flags;
		obj->SetName(name);
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
	static void        RegisterExtraEvent(const JGUIExtraEvent& e, uint64_t priority = 0);
	static JGUIFontManager*  GetJGUIFontManager();
	static const std::string GetDefaultFontName();
	static int         GetWindowZOrder(JGUIWindow* win);
	static JGUIWindow* GetTopJGUIWindow();
	static JGUIWindow* GetNextJGUIWindow(JGUIWindow* win);
	static void RegisterDraggingWindow(JGUIWindow* win);
	static JGUIWindow* GetCurrentDraggingWindow();
public:
	JGUI(IE::InputEngine* input, const JGUIDesc& desc = JGUIDesc());
	void Update();
private:
	LRESULT WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT WindowProcResize(JGUIWindow* window, WPARAM wParam, LPARAM lParam);
	LRESULT WindowSetFocus(JGUIWindow* window, WPARAM wParam);
	LRESULT WindowKillFocus(JGUIWindow* window, WPARAM wParam);
	LRESULT WindowChar(JGUIWindow* window, WPARAM wParam, LPARAM lParam);
	LRESULT WindowMouseWheel(JGUIWindow* window, WPARAM wParam);
	LRESULT WindowKeyDownEvent(WPARAM wParam);
	LRESULT WindowKeyUpEvent(WPARAM wParam);
	LRESULT WindowMouseMove(JGUIWindow* window, LPARAM lParam);
	LRESULT WindowMouseHover(JGUIWindow* window);
	LRESULT WindowMouseLeave(JGUIWindow* window);
private:
	Plugin            m_Plugin;
    static JGUI* sm_GUI;
	using JGUILoadFunc = JGUIObject * (*)(const std::string&);
	using JGUIMainWindowFunc = JGUIWindow * (*)(const std::string&);
	using JGUIWindowFunc = JGUIWindow * (*)(const std::string&, bool);
	using JGUIObjectPool = std::unordered_map<JGUIObject*, std::shared_ptr<JGUIObject>>;
	using JGUIComponentPool = std::unordered_map<JGUIComponent*, std::shared_ptr<JGUIComponent>>;
	using JGUIScreenPool = std::unordered_map<HWND,
		std::pair<JGUIWindow*, std::shared_ptr<JGUIScreen>>>;


	JGUIObjectPool    m_ObjectPool;
	JGUIComponentPool m_ComponentPool;
	JGUIScreenPool    m_ScreenPool;


	
	JGUIDesc          m_Desc;

	std::queue<HWND> m_ExpectedDestroyWindow;
	std::queue<std::shared_ptr<JGUIObject>> m_ExpectedCreateObject;
	std::queue<JGUIObject*>    m_ExpectedDestroyObject;
	std::queue<JGUIComponent*> m_ExpectedDestroyComponent;

	JGUIWindow*      m_MainWindow = nullptr;
	JGUIWindow*      m_DraggingWindow = nullptr;
	HWND             m_MainHWND;
	uint64_t             m_IDOffset = 0;
	std::queue<uint64_t> m_IDQueue;


	std::unordered_map<JGUIObject*, JGUIMouseTrack> m_MouseTrackMap;
	std::unordered_map<JGUIObject*, std::map<uint64_t, std::vector<JGUIExtraEvent>>> m_ExtraEventMap;
	
	std::shared_ptr<JGUIFontManager> m_FontManager;
	std::string m_DefaultFont = GlobalLinkData::_EngineConfig->InEngine("Fonts/Consolas.fnt");
	IE::InputEngine*    m_Input = nullptr;
};





// 시작 윈도우 형태
#define JGUI_MAIN_WINFORM(ClassName, flag) \
class ClassName; \
extern "C" __declspec(dllexport) \
inline ClassName* LoadMainWindowForm(const std::string& name) { \
    return JGUI::CreateJGUIWindow<##ClassName>(name, flag); \
}\

#define JGUI_REGISTER_WINFORM(ClassName) \
class ClassName; \
extern "C" __declspec(dllexport) \
inline ClassName* LoadWindowForm_##ClassName(JGUIWindow* owner, const std::string& name) { \
    return owner->CreateJGUIForm<##ClassName>(name); \
}\

#define JGUI_REGISTER_ELEMENT(ClassName) \
class ClassName; \
extern "C" __declspec(dllexport) \
inline ClassName* LoadEelement_##ClassName(JGUIWindow* owner, const std::string& name) { \
    return owner->CreateJGUIElement<##ClassName>(name); \
}\
inline ClassName * LoadEelement_##ClassName(JGUIElement* owner, const std::string & name) { \
    return owner->CreateJGUIElement<##ClassName>(name); \
}\



#define JGUI_REGISTER_COMPONENT(ClassName) \
class ClassName; \
extern "C" __declspec(dllexport) \
inline ClassName* LoadComponent_##ClassName(JGUIWindow* owner) { \
    return owner->CreateJGUIComponent<##ClassName>(); \
}\
inline ClassName* LoadComponent_##ClassName(JGUIElement* owner) { \
    return owner->CreateJGUIComponent<##ClassName>(); \
}\



#define JGUI_REGISTER_PTR(JGUIPtr)  \
*JGUI::InstancePtr() = JGUIPtr;  \





