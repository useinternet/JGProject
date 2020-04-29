#pragma once
#include "JGUIObject.h"
#include "GUI/JGUIComponent/JGUITransform.h"
#include <string>
#include <set>
class RenderItem;
class JGUIWindow;
class JGUICollider;
class JGUIRectTransform;


class JGENGINE_API JGUIElement : public JGUIObject
{
	friend JGUI;
	friend JGUIWindow;
private:
	static uint64_t m_RIOffset;
	static void RISortingOrderReset() {
		m_RIOffset = 0;
	}
public:
	JGUIWindow* GetOwnerWindow() const;
	virtual void SetActive(bool active, bool is_hierarchy = false) override;
protected:
	virtual void JGUIAwake() override;
	virtual void JGUIStart() override;
	virtual void JGUIDestroy() override;
	virtual void JGUITick(const JGUITickEvent& e) override;
	virtual void JGUIResize(const JGUIResizeEvent& e);
	virtual void JGUIMouseWheel(const JGUIMouseWheelEvent& e);
	virtual void JGUIChar(const JGUICharEvent& e);
	virtual void JGUIKeyDown(const JGUIKeyDownEvent& e);
	virtual void JGUIKeyUp(const JGUIKeyUpEvent& e);
	virtual void JGUIMouseMove(const JGUIMouseMoveEvent& e);
	virtual void JGUIMouseBtDown(const JGUIKeyDownEvent& e);
	virtual void JGUIMouseBtUp(const JGUIKeyUpEvent& e);
	virtual void JGUIMouseLeave();
	virtual void JGUIMouseHover();
	virtual void JGUIFocusEnter(const JGUIFocusEnterEvent& e);
	virtual void JGUIFocusExit(const JGUIFocusExitEvent& e);
	virtual void JGUIOnFocus();
protected:
	virtual void Resize(const JGUIResizeEvent& e) {}
	virtual void MouseWheel(const JGUIMouseWheelEvent& e) {}
	virtual void Char(const JGUICharEvent& e) {}
	virtual void KeyDown(const JGUIKeyDownEvent& e) {}
	virtual void KeyUp(const JGUIKeyUpEvent& e) {}
	virtual void MouseBtDown(const JGUIKeyDownEvent& e) {}
	virtual void MouseBtUp(const JGUIKeyUpEvent& e) {}
	virtual void MouseMove(const JGUIMouseMoveEvent& e) {}
	virtual void MouseLeave() {}
	virtual void MouseHover() {}
	virtual void FocusEnter(const JGUIFocusEnterEvent& e) {}
	virtual void FocusExit(const JGUIFocusExitEvent& e) {}
	virtual void OnFocus() {}
public:
	virtual void SetParent(JGUIElement* parent);
	JGUIElement* GetParent() const;

	uint32_t GetChildCount() const {
		return (uint32_t)m_Childs.size();
	}
	JGUIElement* GetChild(uint32_t n) {
		if (n < GetChildCount()) return m_Childs[n];
		else
		{
			return nullptr;
		}
	}
	EJGUI_ElementFlags GetElementFlags() const {
		return m_Flags;
	}

	void AddElementFlags(EJGUI_ElementFlags flags);
	void RemoveElementFlags(EJGUI_ElementFlags flags);
	void SetElementFlags(EJGUI_ElementFlags flags);
	uint64_t GetRISortingOrder() const {
		return m_RISortingOrder;
	}
public:
	template<typename ComponentType>
	ComponentType* CreateJGUIElement(const std::string& name, EJGUI_ElementFlags flag = JGUI_ElementFlag_None)
	{
		auto com = JGUI::CreateJGUIElement<ComponentType>(name, m_OwnerWindow, flag);
		com->SetParent(this);
		return com;
	}

	JGUIElement* CreateJGUIElement(const std::string& name, EJGUI_ElementFlags flag = JGUI_ElementFlag_None)
	{
		return CreateJGUIElement<JGUIElement>(name, flag);
	}

	void DestroyJGUIElement(JGUIElement* com);


private:
	bool Interation();
	JGUIElement* TrackingCanInteractionComponent();
	void NotifyChangeParentToComponents(JGUIObject* prevParent);
private:
	JGUIWindow* m_OwnerWindow = nullptr;

	// 부모
	JGUIElement* m_Parent   = nullptr;
	

	// 플래그
	EJGUI_ElementFlags m_Flags;
	std::vector<JGUIElement*> m_Childs;

	JVector2 m_PrevSize;
	bool     m_IsMouseTracking = false;
	uint32_t m_Priority       = 0;
	uint64_t m_RISortingOrder = 0;

	friend void JGUITransform::SendSizeToElement();

private:
	// 함수들
	std::map<uint64_t, std::vector<ResizeFunc>>      m_ResizeFuncs;
	std::map<uint64_t, std::vector<MouseWheelFunc>>  m_MouseWheelFuncs;
	std::map<uint64_t, std::vector<CharFunc>>        m_CharFuncs;
	std::map<uint64_t, std::vector<KeyDownFunc>>     m_KeyDownFuncs;
	std::map<uint64_t, std::vector<KeyUpFunc>>       m_KeyUpFuncs;
	std::map<uint64_t, std::vector<MouseBtDownFunc>> m_MouseBtDownFuncs;
	std::map<uint64_t, std::vector<MouseBtUpFunc>>   m_MouseBtUpFuncs;
	std::map<uint64_t, std::vector<MouseMoveFunc>>   m_MouseMoveFuncs;
	std::map<uint64_t, std::vector<MouseLeaveFunc>>  m_MouseLeaveFuncs;
	std::map<uint64_t, std::vector<MouseHoverFunc>>  m_MouseHoverFuncs;
	std::map<uint64_t, std::vector<FocusEnterFunc>>  m_FocusEnterFuncs;
	std::map<uint64_t, std::vector<FocusExitFunc>>   m_FocusExitFuncs;
	std::map<uint64_t, std::vector<OnFocusFunc>>     m_OnFocusFuncs;

public:
	void BindResizeFunc(const ResizeFunc& func, int n = 0)
	{
		m_ResizeFuncs[n].push_back(func);
	}
	void BindMouseWheelFunc(const MouseWheelFunc& func, int n = 0)
	{
		m_MouseWheelFuncs[n].push_back(func);
	}
	void BindCharFunc(const CharFunc& func, int n = 0)
	{
		m_CharFuncs[n].push_back(func);
	}
	void BindKeyDownFunc(const KeyDownFunc& func, int n = 0)
	{
		m_KeyDownFuncs[n].push_back(func);
	}
	void BindKeyUpFunc(const KeyUpFunc& func, int n = 0)
	{
		m_KeyUpFuncs[n].push_back(func);
	}
	void BindMouseBtDownFunc(const MouseBtDownFunc& func, int n = 0)
	{
		m_MouseBtDownFuncs[n].push_back(func);
	}
	void BindMouseBtUpFunc(const MouseBtUpFunc& func, int n = 0)
	{
		m_MouseBtUpFuncs[n].push_back(func);
	}
	void BindMouseMoveFunc(const MouseMoveFunc& func, int n = 0)
	{
		m_MouseMoveFuncs[n].push_back(func);
	}
	void BindMouseLeaveFunc(const MouseLeaveFunc& func, int n = 0)
	{
		m_MouseLeaveFuncs[n].push_back(func);
	}
	void BindMouseHoverFunc(const MouseHoverFunc& func, int n = 0)
	{
		m_MouseHoverFuncs[n].push_back(func);
	}
	void BindFocusEnterFunc(const FocusEnterFunc& func, int n = 0)
	{
		m_FocusEnterFuncs[n].push_back(func);
	}
	void BindFocusExitFunc(const FocusExitFunc& func, int n = 0)
	{
		m_FocusExitFuncs[n].push_back(func);
	}
	void BindOnFocusFunc(const OnFocusFunc& func, int n = 0)
	{
		m_OnFocusFuncs[n].push_back(func);
	}


};