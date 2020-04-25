#pragma once

#include "CommonCore.h"



#define JGUIEventArgConstruct(classname) classname() : JGUIEventArg(#classname) {}
class JGUIWindow;
class JGUIObject;
class JGUIElement;
class JGUIEventArg
{
private:
	std::string m_Name;
public:
	JGUIEventArg(const std::string& name = "none") : m_Name(name) {}
	virtual std::string ToString() const {
		return m_Name;
	}
};
class JGUIResizeEvent : public JGUIEventArg
{
public:
	JGUIEventArgConstruct(JGUIResizeEvent)
	virtual std::string ToString() const {
		return JGUIEventArg::ToString() + 
			   "  [ " + std::to_string(width) + ", " + std::to_string(height) + " ]";
	}
public:
	float width  = 0.0f;
	float height = 0.0f;
};
class JGUITickEvent : public JGUIEventArg
{
public:
	JGUIEventArgConstruct(JGUITickEvent)
	virtual std::string ToString() const {
		return JGUIEventArg::ToString() +
			"  total : " + std::to_string(totalTime) +
			"  delta : " + std::to_string(deltaTime) +
			"  fps   : " + std::to_string(fps);
	}
public:
	float deltaTime = 0.0f;
	float totalTime = 0.0f;
	int   fps = 0;
};

class JGUIFocusEnterEvent : public JGUIEventArg
{
public:
	JGUIEventArgConstruct(JGUIFocusEnterEvent)
public:
	JGUIObject* prevFocus = nullptr;
};

class JGUIFocusExitEvent : public JGUIEventArg
{
public:
	JGUIEventArgConstruct(JGUIFocusExitEvent)
public:
	JGUIObject* nextFocus= nullptr;
};


class JGUIMouseMoveEvent : public JGUIEventArg
{
public:
	JGUIEventArgConstruct(JGUIMouseMoveEvent)
	virtual std::string ToString() const {
		return JGUIEventArg::ToString() +
			"  pos   : [ " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + " ]" +
			"  delta : [ " + std::to_string(delta.x) + ", " + std::to_string(delta.y) + " ]";
	}
public:
	JVector2 pos;
	JVector2 delta;
};

class JGUIMouseWheelEvent : public JGUIEventArg
{
public:
	JGUIEventArgConstruct(JGUIMouseWheelEvent)
		virtual std::string ToString() const {
		return JGUIEventArg::ToString() +
			"  delta : [ " + std::to_string(delta) + " ]";
	}
public:
	int delta = 0;
};
class JGUIKeyDownEvent : public JGUIEventArg
{
public:
	JGUIEventArgConstruct(JGUIKeyDownEvent)
	virtual std::string ToString() const {
		return JGUIEventArg::ToString() + " : " + Code.ToString();
	}
public:
	KeyCode Code;
};
class JGUIKeyUpEvent : public JGUIEventArg
{
public:
	JGUIEventArgConstruct(JGUIKeyUpEvent)
		virtual std::string ToString() const {
		return JGUIEventArg::ToString() + " : " + Code.ToString();
	}
public:
	KeyCode Code;
};

class JGUICharEvent : public JGUIEventArg
{
public:
	JGUIEventArgConstruct(JGUICharEvent)
		virtual std::string ToString() const {
		return JGUIEventArg::ToString() + " : " + str;
	}
public:
	std::string str = "";
};













using ResizeFunc       = std::function<void(const JGUIResizeEvent&)>;
using MouseWheelFunc   = std::function<void(const JGUIMouseWheelEvent&)>;
using CharFunc         = std::function<void(const JGUICharEvent&)>;
using KeyDownFunc      = std::function<void(const JGUIKeyDownEvent&)>;
using KeyUpFunc        = std::function<void(const JGUIKeyUpEvent&)>;
using MouseBtDownFunc  = std::function<void(const JGUIKeyDownEvent&)>;
using MouseBtUpFunc    = std::function<void(const JGUIKeyUpEvent&)>;
using MouseMoveFunc    = std::function<void(const JGUIMouseMoveEvent&)>;
using MouseLeaveFunc   = std::function<void()>;
using MouseHoverFunc   = std::function<void()>;
using FocusEnterFunc   = std::function<void(const JGUIFocusEnterEvent&)>;
using FocusExitFunc    = std::function<void(const JGUIFocusExitEvent&)>;
using OnFocusFunc      = std::function<void()>;