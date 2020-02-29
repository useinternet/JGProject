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
protected:
	virtual void Resize(const JGUIResizeEvent& e) {}
	virtual void FocusEnter(const JGUIFocusEnterEvent& e) {}
	virtual void FocusExit(const JGUIFocusExitEvent& e) {}
	virtual void OnFocus() {}



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
	ComponentType* CreateJGUIComponent(const std::string& name, EJGUI_ComponentFlags flags = JGUI_ComponentFlag_None)
	{
		auto com = JGUI::CreateJGUIComponent<ComponentType>(name, this, flags);
		com->SetParent(m_Panel);
		return com;
	}

	void DestroyJGUIWindow(JGUIWindow* window);
	void DestroyJGUIComponent(JGUIComponent* com);



	// 윈도우 유틸 함수
	JVector2 ConvertToScreenPos(const JVector2& pos);
	JGUIRect ConvertToScreenRect(const JGUIRect& rect);
	JVector2 ConvertToWorldPos(const JVector2& pos);
	JGUIRect ConvertToWorldRect(const JGUIRect& pos);
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

	JGUIScreen* GetScreen() const {
		return m_Screen;
	}


	// 프로퍼티 함수
	void SetPriority(EJGUI_WindowPriority p);
	EJGUI_WindowPriority GetPriority() const
	{
		return m_Priority;
	}

	void SetWindowFlags(EJGUI_WindowFlags flag);
	EJGUI_WindowFlags GetWindowFlags() const;


	virtual void SetActive(bool active, bool is_hierarchy = false) override;
protected:
	virtual void ProcessByWindowFlags(EJGUI_WindowFlags flag);
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
	JGUIWindowTexture*           m_WinTexture     = nullptr;
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



	friend void JGUIWinRectTransform::SendSizeToWin();
};

/*
1. 위치나 회전 값은 부모의 위치를 기준으로 한다.
2. 

*/

