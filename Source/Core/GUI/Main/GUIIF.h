#pragma once


#include "GUIDefine.h"


#define GUILOG(level, contents, ...) JGLOG(level, GUI, contents,  __VA_ARGS__)
#define GUILOG_DEBUG(contents, ...) GUILOG(LogLevel::Debug, contents, __VA_ARGS__)
#define GUILOG_INFO(contents, ...)  GUILOG(LogLevel::Info, contents, __VA_ARGS__)
#define GUILOG_WARN(contents, ...)  GUILOG(LogLevel::Warning, contents, __VA_ARGS__)
#define GUILOG_ERROR(contents, ...) GUILOG(LogLevel::Error, contents, __VA_ARGS__)
#define GUILOG_FATAL(contents, ...) GUILOG(LogLevel::Fatal, contents, __VA_ARGS__)

namespace JGUI
{
	class GUIObject;
	class Element;
	class Window;
	class Transform;


	class GUI_API GUIIF
	{
		friend class GUI;
		friend LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	public:
		static float GetTick();

		template<typename T>
		static T* AllocateGUIObject() {
			auto obj = std::make_unique<T>();
			auto result = obj.get();
			GetInstance()->m_ObjectPool.push_back(std::move(obj));

			result->m_InstanceID = (uint64_t)result;
			return result;
		}
		static void UnAllocateGUIObject(GUIObject* obj);


		static void RegisterScreen(Window* win);
		static void UnRegisterScreen(Window* win);


		static void ReserveDestroyElement(Element* element);
		static void ReserveDestroyWindow(Window* win);

		static JFont*      LoadFont(const std::wstring& path);
		static MaterialRef GetGUIMaterialRef(GUI_PreGenerated_Material m);
		static Window*     GetMainWindow();
		static void SetCursorPos(const JVector2& pos);
		static JVector2    GetCursorPos() {
			return GetInstance()->m_IO.MousePos;
		}
		static void CursorLock(const JRect& area);
		static void CursorUnLock();
		static void CursorVisible(bool is_visible);

		static GUIIO& GetIO();
		static GUIStyle& GetStyle();

		template<typename WinFormType>
		static void CreateMainWindowForm(const std::wstring& name, WindowFlags flags)
		{
			GetInstance()->m_MainWindow = GUIIF::AllocateGUIObject<WinFormType>();
			GetInstance()->m_MainWindow->SetName(name);
			GetInstance()->m_MainWindow->GUIAwake();
			GetInstance()->m_MainWindow->SetFlags(flags);
			GetInstance()->m_MainWindow->GetTransform()->SetSize(800, 600);
		}
	private:
		struct GUIScreen
		{
			ScreenRef screen;
			JWindow* win = nullptr;

			JVector2 prevSize;
			JVector2 prevLocation;

			bool mouseInScreen = false;
			bool prevMouseInScreen = false;
			bool mouseDown[3];

			enum
			{
				MouseButtonCount = 3
			};
			GUIScreen()
			{
				for (int i = 0; i < MouseButtonCount; ++i)
				{
					mouseDown[i] = false;
				}
			}
		};
		// Down을 통해 선택된 윈도우에서만 Up Event 발동
		// Input
	private:
		static GUIIF* GetInstance();
		void ProcessGarbageCollection(float tick);
		void UpdateScreen();
		void UpdateWindow();
		void UpdateEvent();
		void UpdateEventFocusedGUIScreen(Window* focusWin, GUIScreen* focusScreen);
		void UpdateKeyDuration(KeyCode code, Window* win);
		void ResetKeyDuration(KeyCode code, Window* win);
		void DestroyIF();
	private:
		using GUIObjectPool = std::vector<std::shared_ptr<GUIObject>>;
		using GUIScreenPool = std::unordered_map<Window*, std::shared_ptr<GUIScreen>>;


		GUIObjectPool     m_ObjectPool;
		GUIScreenPool     m_ScreenPool;

		std::vector<std::queue<Element*>> m_ReservedElements;
		std::vector<std::queue<Window*>>  m_ReservedWindows;
		uint64_t m_FrameIndex = 0;


		GUIIO    m_IO;
		GUIStyle m_Style = GUIStyle(m_IO);
		std::unique_ptr<Plugin> m_EditPlugin;
		JFontManager m_FontManager;
		class Window* m_MainWindow = nullptr;
		class Window* m_SelectedWindow = nullptr;
		class Window* m_FocusWindow = nullptr;


		bool m_IsCloseApp = false;
	};
}

#define REGISTER_MAIN_WINFORM(classType, titleName, flag) \
REGISTER_GLOBAL_SHARED_DATA \
class classType; \
extern "C" __declspec(dllexport) void _CreateMainWindowForm() \
{ \
JGUI::GUIIF::CreateMainWindowForm<##classType>(##titleName,##flag);\
} \
