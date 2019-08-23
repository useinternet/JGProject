#pragma once
#include "Event.h"

enum class WindowEventType{
	Resize,

};
class WindowEvent : public Event
{
	WindowEventType m_Type;
public:
	WindowEvent(WindowEventType type) : m_Type(type) {}
	REGISTER_EVENT_TYPE(WindowEvent)
	REGISTER_EVENT_CATEGORY(EventCategory_None)
	virtual std::string ToString() const override {
		return "WindowEvent";
	}
	WindowEventType GetWindowEventType() const {
		return m_Type;
	}
};


class WindowResizeEvent : public WindowEvent
{
public:

	uint32_t width = 0;
	uint32_t height = 0;
public:
	WindowResizeEvent() : WindowEvent(WindowEventType::Resize) {}
	REGISTER_EVENT_CATEGORY(EventCategory_RenderEngine | EventCategory_Application)
	virtual std::string ToString() const override {
		return "WindowResize  width : " + std::to_string(width) + "  height : " + std::to_string(height);
	}

};

