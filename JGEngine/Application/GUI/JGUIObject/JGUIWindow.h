#pragma once

#include "JGUIObject.h"
#include "GUI/JGUIComponent/JGUIPanel.h"
#include "GUI/JGUIComponent/JGUIRectTransform.h"
#include <string>

namespace RE
{
	class FixedGShaderModuleClone;
	class ReCamera;
}

class JGUIScreen;
class JGUIComponent;
class JGUIRectTransform;
class JGUIRectangle;
class JGUIWindowTexture;
class JGUIWinRectTransform;
class JGUIBoxCollider;
class JGUIWindow : public JGUIObject
{
	friend JGUI;
private:
	virtual void JGUIAwake() override;
	virtual void JGUIStart() override;
	virtual void JGUIDestroy() override;
	virtual void JGUITick(const JGUITickEvent& e) override;
	virtual void JGUIResize(const JGUIResizeEvent& e);
	virtual void JGUIFocusEnter(const JGUIFocusEnterEvent& e);
	virtual void JGUIFocusExit(const JGUIFocusExitEvent& e);
	virtual void JGUIOnFocus();


	virtual void JGUIChar(const JGUICharEvent& e);
	virtual void JGUIKeyDown(const JGUIKeyDownEvent& e);
	virtual void JGUIKeyUp(const JGUIKeyUpEvent& e);
	virtual void JGUIMouseBtDown(const JGUIKeyDownEvent& e);
	virtual void JGUIMouseBtUp(const JGUIKeyUpEvent& e);
	virtual void JGUIMouseMove(const JGUIMouseMoveEvent& e);
	virtual void JGUIMouseHover();
	virtual void JGUIMouseLeave();
	virtual void JGUIParentUpdateNotification();
protected:
	virtual void Resize(const JGUIResizeEvent& e) {}
	virtual void FocusEnter(const JGUIFocusEnterEvent& e) {}
	virtual void FocusExit(const JGUIFocusExitEvent& e) {}
	virtual void OnFocus() {}



	virtual void Char(const JGUICharEvent& e);
	virtual void KeyDown(const JGUIKeyDownEvent& e) {}
	virtual void KeyUp(const JGUIKeyUpEvent& e) {}
	virtual void MouseBtDown(const JGUIKeyDownEvent& e) {}
	virtual void MouseBtUp(const JGUIKeyUpEvent& e) {}
	virtual void MouseMove(const JGUIMouseMoveEvent& e) {}
	virtual void MouseHover() {}
	virtual void MouseLeave() {}
	virtual void ParentUpdateNotification();
public:
	// 자식 부모 관련 함수 
	void        SetParent(JGUIWindow* parent);
	JGUIWindow* GetParent() const;

	const std::vector<JGUIWindow*>& GetChilds() const;
	std::vector<JGUIWindow*>&       GetChilds();

	std::vector<JGUIComponent*>& GetWindowComponents();
	JGUIWindow*                  FindChild(uint32_t index);
	JGUIWindow*                  FindChild(const std::string& name);



	// 윈도우 & 컴포넌트 생성 및 삭제

	template<typename WindowType>
	WindowType* CreateJGUIWindow(const std::string& name, EJGUI_WindowFlags flag = JGUI_WindowFlag_None)
	{
		auto win = JGUI::CreateJGUIWindow<WindowType>(name, flag);
		if ((flag & JGUI_WindowFlag_NewLoad) == false)
		{
			win->SetParent(this);
		}
		return win;
	}

	template<typename ComponentType>
	ComponentType* CreateJGUIComponent(const std::string& name)
	{
		auto com = JGUI::CreateJGUIComponent<ComponentType>(name, this);
		com->SetParent(m_Panel);
		return com;
	}

	// 윈도우 유틸 함수
	JVector2 ConvertToScreenPos(const JVector2& pos);
	JGUIRect ConvertToScreenRect(const JGUIRect& rect);
	JVector2Int GetMousePos();

	HWND GetRootWindowHandle() const;

	// Focus
	void           SetFocusComponent(JGUIComponent* com);
	JGUIComponent* GetFocusComponent();
	void           SetFocusWindow(JGUIWindow* win);
	JGUIWindow*    GetFocusWindow();


	// 윈도우 컴포넌트
	JGUIPanel* GetPanel() const {
		return m_Panel;
	}
	JGUIBoxCollider* GetColider() const {
		return m_BoxColider;
	}

	JGUIWinRectTransform* GetTransform() const {
		return m_RectTransform;
	}
	JGUIScreen* GetScreen() const {
		return m_Screen;
	}


	// 프로퍼티 함수
	void SetPriority(EJGUI_WindowPriority p);
	EJGUI_WindowPriority GetPriority() const
	{
		return m_Priority;
	}
private:
	void Init(const std::string& name, EJGUI_WindowFlags flags);
	void NewLoad();
	void ReadyGUIModule();
	bool Interaction();
	EJGUI_WindowPriority ChildWindowSortByPriority();
	JGUIWindow* TrackingCanInteractionWindow();
private:
	JGUIScreen* m_Screen = nullptr;
	JGUIWindow* m_ParentWindow = nullptr;
	std::vector<JGUIComponent*>  m_WindowComponents;
	std::vector<JGUIWindow*>     m_ChildWindows;
	JGUIComponent*               m_FocusComponent = nullptr;
	JGUIWindow*                  m_FocusWindow    = nullptr;
	JGUIWinRectTransform*        m_RectTransform  = nullptr;
	JGUIWindowTexture*           m_WinTexture     = nullptr;
	JGUIBoxCollider*             m_BoxColider     = nullptr;
	JGUIPanel*                   m_Panel          = nullptr;

	EJGUI_WindowFlags m_Flags = JGUI_WindowFlag_None;

	// RenderEngine 변수들
	std::shared_ptr<RE::FixedGShaderModuleClone> m_GUIModule;
	std::shared_ptr<RE::ReCamera>                m_ReCamera;
	EJGUI_WindowPriority m_Priority = JGUI_WindowPriority_None;

	// 작동 변수들
	bool m_IsTracking   = false;
	bool m_IsMouseLeave = false;
	bool m_IsMouseDown  = false;

	// 부가 파츠
	class JGUITitleBar*  m_Title = nullptr;
	class JGUIResizeBox* m_ResizeBox = nullptr;


	friend void JGUIWinRectTransform::SendSizeToWin();
};

/*
1. 위치나 회전 값은 부모의 위치를 기준으로 한다.
2. 

*/

