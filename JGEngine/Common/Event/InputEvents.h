#pragma once
#include "Event.h"
#include "DataType/KeyCode.h"

enum class InputEventType
{
	MouseMove,
	MouseButton,
	Key
};
enum class MouseEventType
{
	LeftMouseButtonDown,
	LeftMouseButtonUp,
	RightMouseButtonDown,
	RightMouseButtonUp,
	LeftMouseClick,
	RightMouseClick,
	LeftMouseDoubleClick,
	RightMouseDoubleClick,

	WheelButtonDown,
	WheelButtonUp,
	WheelButtonClick,
	WheelButtonDoubleClick
};

enum class KeyType
{
	Pressed,
	Released
};
class InputEvent : public Event
{
public:
	InputEvent(InputEventType type) : m_InputType(type) {}
protected:
	InputEventType m_InputType;
public:
	REGISTER_EVENT_TYPE(InputEvent)
	REGISTER_EVENT_CATEGORY(EventCategory_None)
	virtual std::string ToString() const override
	{
		return "InputEvent";
	}
};

class MouseMoveEvent : public InputEvent
{
public:
	float x, y;
	float delta_x, delta_y;
public:
	MouseMoveEvent() :
		InputEvent(InputEventType::MouseMove),
		x(0.0f), y(0.0f), delta_x(0.0f), delta_y(0.0f) { }
public:
	REGISTER_EVENT_CATEGORY(EventCategory_All)
	virtual std::string ToString() const override
	{
		return "MouseMove  x : " + std::to_string(x) + ",  y : " + std::to_string(y);
	}
};

class MouseButtonEvent : public InputEvent
{
public:
	MouseButtonEvent() : InputEvent(InputEventType::MouseButton) {}
public:
	REGISTER_EVENT_CATEGORY(EventCategory_All)
};




class KeyEvent : public InputEvent
{
public:
	KeyEvent() : InputEvent(InputEventType::Key) {}
public:
	REGISTER_EVENT_CATEGORY(EventCategory_All)
};
