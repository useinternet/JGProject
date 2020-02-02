#pragma once

#include "CommonCore.h"



#define JGUIEventArgConstruct(classname) classname() : JGUIEventArg(#classname) {}
class JGUIWindow;
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
	JGUIWindow* prevFocusWindow = nullptr;
};

class JGUIFocusExitEvent : public JGUIEventArg
{
public:
	JGUIEventArgConstruct(JGUIFocusExitEvent)
public:
	JGUIWindow* nextFocusWindow = nullptr;
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
