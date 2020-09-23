#pragma once
#include <map>
#include "GUIObject.h"

#if defined CreateWindow
#undef CreateWindow
#endif
namespace JGUI
{
	class GUI_API Window : public GUIObject
	{
		friend class GUI;
		friend class GUIIF;
		friend LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	public:
		virtual ~Window() {}
	protected:
		virtual void SetName(const std::wstring& name);
	private:
		virtual void GUIAwake();
		virtual void GUIStart();
		virtual void GUITick();
		virtual void GUIDestroy();
	protected:
		virtual void Awake()   {}
		virtual void Start()   {}
		virtual void Tick()    {}
		virtual void Destroy() {}
	protected:
		virtual void OnMouseButtonDown(KeyCode bt) {}
		virtual void OnMouseButton(KeyCode bt)     {}
		virtual void OnMouseButtonUp(KeyCode bt)   {}

		virtual void OnMouseClick(KeyCode bt)       {}
		virtual void OnMouseDoubleClick(KeyCode bt) {}

		virtual void OnChar(wchar_t c) {}
		virtual void OnMouseEnter() {}
		virtual void OnMouseMove() {}
		virtual void OnMouseExit()  {}


		virtual void OnKeyDown(KeyCode code) {}
		virtual void OnKey(KeyCode code)     {}
		virtual void OnKeyUp(KeyCode code)   {}


		virtual void OnResize(uint32_t width, uint32_t height) {}
		virtual void OnFocusEnter() {}
		virtual void OnFocus() {}
		virtual void OnFocusExit() {}

		virtual void OnDropItem(const GUIDropItem& item) {}
	public: // 생성 관련
		template<typename T>
		T* CreateElement(const std::wstring& name, ElementFlags flags = ElementFlag_None) {
			T* element = GUIIF::AllocateGUIObject<T>();
			element->SetName(name);
			element->m_OwnerWindow = this;
			element->m_Flags = flags;
			element->GUIAwake();
			element->GetTransform()->SetParent(m_ElementRoot->GetTransform());
			return element;
		}
		template<typename T>
		T* CreateWindow(const std::wstring& name, WindowFlags flag)
		{
			T* win = GUIIF::AllocateGUIObject<T>();
			win->SetName(name);
			win->GUIAwake();
			if (flag & WindowFlag_RootWindow && GUIIF::GetIO().IOFlags & GUIIOFlag_NoMultiSwapChain)
			{
				flag = (WindowFlags)(flag & (WindowFlags)~WindowFlag_RootWindow);
			}
			if (!(flag & WindowFlag_RootWindow))
			{
				win->GetTransform()->SetParent(GetTransform());
				FocusWindowUpdate(win);
			}
			win->SetFlags(flag);
			return win;
		}
		void DestroyElement(Element* element);
		void DestroyWindow(Window* win);
		void PushDrawItem(const GUIDrawItem& item);
		void RegisterInteractionItem(const GUIInteractionItem& item);
		void UnRegisterInteractionItem(Element* element);

		void SetFocusWindow(Window* win);
		void SetFocusElement(Element* element);
		JRect GetClientRect() const;
	public:
		Transform* GetTransform() const;
		JVector2   GetMousePos() const;
		void SetBackGroundColor(const JColor& color);
		void SetFlags(WindowFlags flag);
		uint64_t GetDrawPriority() const {
			return m_DrawPriority;
		}


		Window* GetFocusWindow() const {
			return m_FocusWindow;
		}
		Element* GetFocusElement() const {
			return m_FocusElement;
		}
	private:
		void ReceiveKeyDownEvent(KeyCode code);
		void ReceiveKeyUpEvent(KeyCode code);
		void ReceiveKeyEvent(KeyCode code);
		void ReceiveMouseMoveEvent(const JVector2& delta);
		void ReceiveFocusEvent(int exit);
		void ReceiveDropItemEvent(const GUIDropItem& item);
		void ReceiveCharEvent(wchar_t s);
	private:
		void CreateRootElement();
		void SceneUpdate();
		void RenderUpdate();
		void FlagUpdate();
		void SettingUpdate();
		void FocusWindowUpdate(Window* focusedWin);
		void TrackingFocusWindow();
		void TrackingFocusElement(bool is_under);
		void TrackingItemFlush(Element* prevFocusElement);
	private:
		struct EssentialElementCollection
		{
			class Element* border     = nullptr;
			class Element* resizeGrip = nullptr;
			class Element* titleBar = nullptr;
			class Element* closeBt = nullptr;
			class Element* hideBt = nullptr;
			class Element* maxBt = nullptr;
		};

		/*
		Focus 잃으면 Window Move 리셋
		 */
	private:
		// 윈도우 하나당 하나의 텍스쳐 
		GUIWindowDrawItem m_WindowDrawItem;

		// Setting Var
		WindowFlags m_Flags;
		JRect m_WorkAreaOff;
		bool  m_IsMouseDown      = false;
		bool  m_IsPrevMouseEnter = false;
		bool  m_IsMaximizeState  = false;
		//bool  m_Maximizing       = false;
		JRect    m_RestoreWindowRect;
		JVector2 m_RestorePivot;
		
		// Double Click Var
		bool     m_IsMouseClick = false;
		KeyCode  m_MouseClickedButton;
		JVector2 m_MouseClickedPos;
		float    m_MouseClickDuration = 0.0f;


		// Window Move Var
		bool     m_WindowMove = false;
		JVector2 m_DeltaBetWC;

		// Window Priority
		uint64_t m_DrawPriority              = GUIDraw_Priority_Window;


		// ChildWindow
		Element* m_WindowRoot  = nullptr;
		Element* m_WindowImage = nullptr;

		// Element
		using InteractionItemPool = std::unordered_map<Element*, std::shared_ptr<GUIInteractionItem>>;
		Element* m_ElementRoot = nullptr;
		InteractionItemPool m_InteractionItems;
		std::stack<GUIInteractionItem*> m_IteractionStack;
		std::map<uint64_t, std::vector<GUIInteractionItem*>> m_UnderInteractionItems;
		std::map<uint64_t, std::vector<GUIInteractionItem*>> m_AboveInteractionItems;

		EssentialElementCollection m_Collection;
		bool m_IsExecuteStart = false;

		// Focus
		Window*  m_FocusWindow = nullptr;
		Element* m_FocusElement = nullptr;
	};


}