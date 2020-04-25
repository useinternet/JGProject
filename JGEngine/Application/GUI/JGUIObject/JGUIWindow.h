#pragma once

#include "JGUIObject.h"
#include "GUI/JGUIElement/JGUIPanel.h"

#include <string>

class JGUIScreen;
class JGUIElement;

class JGUIWindowRenderer;
class JGUICamera;


class TitleBar;
class DockSystem;
class TabSystem;
class Scrollbar;
class JGUIShapeRenderer;



//class JGUIEmptyRectangle;
class JGUIMenu;
//class JGUIScrollbar;
class JGUIWindow : public JGUIObject
{
	friend JGUI;
protected:
	virtual void JGUIAwake() override;
	virtual void JGUIStart() override;
	virtual void JGUIDestroy() override;
private:
	virtual void JGUITick(const JGUITickEvent& e) override;
	virtual void JGUIResize(const JGUIResizeEvent& e);
	virtual void JGUIFocusEnter(const JGUIFocusEnterEvent& e);
	virtual void JGUIFocusExit(const JGUIFocusExitEvent& e);
	virtual void JGUIOnFocus();
	virtual void JGUIMouseWheel(const JGUIMouseWheelEvent& e);
	virtual void JGUIChar(const JGUICharEvent& e);
	virtual void JGUIKeyDown(const JGUIKeyDownEvent& e);
	virtual void JGUIKeyUp(const JGUIKeyUpEvent& e);
	virtual void JGUIMouseBtDown(const JGUIKeyDownEvent& e);
	virtual void JGUIMouseBtUp(const JGUIKeyUpEvent& e);
	virtual void JGUIMouseMove(const JGUIMouseMoveEvent& e);
	virtual void JGUIMouseHover();
	virtual void JGUIMouseLeave();
protected:
	virtual void Resize(const JGUIResizeEvent& e) {}
	virtual void FocusEnter(const JGUIFocusEnterEvent& e) {}
	virtual void FocusExit(const JGUIFocusExitEvent& e) {}
	virtual void OnFocus() {}
	virtual void MouseWheel(const JGUIMouseWheelEvent& e) {}
	virtual void Char(const JGUICharEvent& e) {}
	virtual void KeyDown(const JGUIKeyDownEvent& e) {}
	virtual void KeyUp(const JGUIKeyUpEvent& e) {}
	virtual void MouseBtDown(const JGUIKeyDownEvent& e) {}
	virtual void MouseBtUp(const JGUIKeyUpEvent& e) {}
	virtual void MouseMove(const JGUIMouseMoveEvent& e) {}
	virtual void MouseHover() {}
	virtual void MouseLeave() {}
public:
	// 자식 부모 관련 함수 
	void        SetParent(JGUIWindow* parent);
	JGUIWindow* GetParent() const;

	const std::vector<JGUIWindow*>& GetChilds() const;
	std::vector<JGUIWindow*>&       GetChilds();

	std::vector<JGUIElement*>&   GetWindowElements();
	JGUIWindow*                  FindChild(uint32_t index);
	JGUIWindow*                  FindChild(const std::string& name);

	template<typename WinFormType>
	WinFormType* CreateJGUIForm(const std::string& name, EJGUI_WindowFlags flag = JGUI_WindowFlag_None)
	{
		auto win = JGUI::CreateJGUIWindow<WinFormType>(name, flag);
		if ((flag & JGUI_WindowFlag_NewLoad) == false)
		{
			win->SetParent(this);
		}
		return win;
	}


	template<typename ComponentType>
	ComponentType* CreateJGUIElement(const std::string& name, EJGUI_ComponentFlags flags = JGUI_ComponentFlag_None)
	{
		auto com = JGUI::CreateJGUIElement<ComponentType>(name, this, flags);
		com->SetParent(m_ElementCanvas);
		return com;
	}

	JGUIElement* CreateJGUIElement(const std::string& name, EJGUI_ComponentFlags flags = JGUI_ComponentFlag_None)
	{
		return CreateJGUIElement<JGUIElement>(name, flags);
	}



	void DestroyJGUIWindow(JGUIWindow* window);
	void DestroyJGUIElement(JGUIElement* com);

	const std::vector<JGUIElement*>& GetElements() const {
		return m_WindowElements;
	}



	// 윈도우 유틸 함수
	JVector2Int GetMousePos();

	HWND GetRootWindowHandle() const;

	// Focus
	void           SetFocusElement(JGUIElement* com);
	JGUIElement*   GetFocusElement();
	void           SetFocusWindow(JGUIWindow* win);
	JGUIWindow*    GetFocusWindow(); 


	JGUIScreen* GetScreen() const {
		return m_Screen;
	}
	TitleBar* GetTitleBar() const {
		return m_TitleBar;
	}
	DockSystem* GetDockSystem() const {
		return m_DockSystem;
	}
	TabSystem* GetTabSystem() const {
		return m_TabSystem;
	}
	JGUIMenu* GetMenu() const {
		return m_Menu;
	}
	JGUICamera* GetCamera() const {
		return m_WinCamera;
	}
	// 프로퍼티 함수
	void SetPriority(EJGUI_WindowPriority p);
	EJGUI_WindowPriority GetPriority() const
	{
		if (m_Flags & JGUI_WindowFlag_BottomMost) return JGUI_WindowPriority_BottomMost;
		if (m_Flags & JGUI_WindowFlag_TopMost)    return JGUI_WindowPriority_TopMost;
		return m_Priority;
	}

	void SetWindowFlags(EJGUI_WindowFlags flag);
	void AddWindowFlags(EJGUI_WindowFlags flag);
	void RemoveWindowFlags(EJGUI_WindowFlags flag);
	EJGUI_WindowFlags GetWindowFlags() const;
	

	virtual void  SetActive(bool active, bool is_hierarchy = false) override;
	JGUIRect      GetClientRect() const;
	void          SetActiveExceptEssentials(bool is_active);
protected:
	virtual void ProcessByWindowFlags(EJGUI_WindowFlags flag);
private:
	void NewLoad();
	void ReadyCamera();
	bool Interaction();
	void NotifyChangeParentToComponents(JGUIObject* prevparent);
	EJGUI_WindowPriority ChildWindowSortByPriority();
	JGUIWindow* TrackingCanInteractionWindow();


	// 일반
	template<typename ComponentType>
	ComponentType* CreateJGUIEssentialElement(const std::string& name, EJGUI_ComponentFlags flags = JGUI_ComponentFlag_None)
	{
		auto com = JGUI::CreateJGUIElement<ComponentType>(name, this, flags);
		com->SetParent(m_EssentialCanvas);
		return com;
	}
private:
	//
	JGUIScreen* m_Screen = nullptr;
	JGUIWindow* m_ParentWindow = nullptr;


	// 윈도우 자식 및 UI 요소
	std::vector<JGUIElement*>    m_WindowElements;
	std::vector<JGUIWindow*>     m_ChildWindows;

	// FocusWindow 및 FocusElement
	JGUIElement*                 m_FocusElement  = nullptr;
	JGUIWindow*                  m_FocusWindow = nullptr;


	// 윈도우 컴포넌트
	JGUIWindowRenderer*          m_WinRenderer  = nullptr;
	JGUICamera*                  m_WinCamera    = nullptr;



	// Script



	/* 윈도우 사이즈 조절 */
	class WindowSizeAdjustment*  m_WinSizeAdjment = nullptr;
	float m_ResizeBoxSize = 20.0f;


	TitleBar*   m_TitleBar   = nullptr;
	float       m_TitleSize  = 20.0f;
	DockSystem* m_DockSystem = nullptr;
	TabSystem*  m_TabSystem  = nullptr;
	Scrollbar*  m_HScrollbar = nullptr;
	Scrollbar*  m_VScrollbar = nullptr;

	JGUIShapeRenderer* m_Border = nullptr;
	// 윈도우 캔버스 모음
	JGUIElement*                 m_RootElement     = nullptr;
	JGUIElement*                 m_EssentialCanvas = nullptr;
	JGUIElement*                 m_ElementCanvas   = nullptr;


	// 윈도우 설정 변수
	EJGUI_WindowFlags    m_Flags = JGUI_WindowFlag_None;
	EJGUI_WindowPriority m_Priority = JGUI_WindowPriority_None;



	// 이벤트 작동 변수들
	bool m_IsTracking   = false;
	bool m_IsMouseLeave = false;
	bool m_IsMouseDown  = false;

	// 부가 파츠







	//JGUITitleBar*       m_Title = nullptr;

	JGUIMenu*           m_Menu = nullptr;
	//JGUIDock*           m_Dock = nullptr;
	//JGUIScrollbar*      m_HScrollbar = nullptr;
	//JGUIScrollbar*      m_VScrollbar = nullptr;




	friend void JGUITransform::SendSizeToWin();
};



