#pragma once

#include "JGUIObject.h"
#include "GUI/JGUIComponent/JGUIPanel.h"
#include <string>

namespace RE
{
	class RenderItem;
	class FixedGShaderModuleClone;
	class ReCamera;
	class InstanceRenderItem;
}

class JGUIScreen;
class JGUIComponent;
class JGUIRectTransform;
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



	virtual void Char(const JGUICharEvent& e);
	virtual void KeyDown(const JGUIKeyDownEvent& e) {}
	virtual void KeyUp(const JGUIKeyUpEvent& e) {}
	virtual void MouseBtDown(const JGUIKeyDownEvent& e) {}
	virtual void MouseBtUp(const JGUIKeyUpEvent& e) {}
	virtual void MouseMove(const JGUIMouseMoveEvent& e) {}
	virtual void MouseHover() {}
	virtual void MouseLeave() {}
public:
	void SetParent(JGUIWindow* parent);
	JGUIWindow* GetParent() const;

	const std::vector<JGUIWindow*>& GetChilds() const;
	std::vector<JGUIWindow*>& GetChilds();
	std::vector<JGUIComponent*>& GetWindowComponents();
	JGUIWindow* FindChild(uint32_t index);
	JGUIWindow* FindChild(const std::string& name);
	JGUIRectTransform* GetTransform() const {
		return m_RectTransform;
	}
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





	JVector2 ConvertToScreenPos(const JVector2& pos);
	JGUIRect ConvertToScreenRect(const JGUIRect& rect);


	HWND GetRootWindowHandle() const;


	void SetFocus(JGUIComponent* com);
	JGUIComponent* GetFocus();

	JGUIPanel* GetPanel() const {
		return m_Panel;
	}
	JGUIScreen* GetScreen() const {
		return m_Screen;
	}
	void SetPriority(EJGUI_WindowPriority p);
	EJGUI_WindowPriority GetPriority() const
	{
		return m_Priority;
	}
private:
	void Init(const std::string& name, EJGUI_WindowFlags flags);
	void NewLoad();
private:
	JGUIScreen* m_Screen = nullptr;
	JGUIWindow* m_ParentWindow = nullptr;
	std::vector<JGUIComponent*>  m_WindowComponents;
	std::vector<JGUIWindow*>     m_ChildWindows;


	JGUIComponent*               m_FocusComponent = nullptr;
	JGUIRectTransform*           m_RectTransform = nullptr;
	JGUIPanel* m_Panel = nullptr;

	EJGUI_WindowFlags m_Flags = JGUI_WindowFlag_None;


	// RenderEngine 변수들
	std::shared_ptr<RE::FixedGShaderModuleClone> m_GUIModule;
	std::shared_ptr<RE::ReCamera>                m_ReCamera;
	EJGUI_WindowPriority m_Priority = JGUI_WindowPriority_None;
	bool m_IsTracking = false;
	bool m_IsMouseLeave = false;
};

/*
1. 위치나 회전 값은 부모의 위치를 기준으로 한다.
2. 

*/

