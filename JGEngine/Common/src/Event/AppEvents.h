#pragma once
#include "Event.h"

enum class AppEventType
{
	EditorGUI
};

class AppEvent : public Event
{
	AppEventType m_Type;
public:
	AppEvent(AppEventType type) : m_Type(type) {}
	REGISTER_EVENT_TYPE(AppEvent)
	REGISTER_EVENT_CATEGORY(EventCategory_None)
	AppEventType GetAppEventType() const { return m_Type; }
	virtual std::string ToString() const override {
		return "AppEvent";
	}
};



// Editor Event
enum class EditorEventType 
{
	RequestResource,
	RequestRenderDeviceDebugInfo
};
class EditorEvent : public AppEvent
{
	EditorEventType m_Type;
public:
	std::string SentIGWindow = "None";
public:
	EditorEvent(EditorEventType type) : AppEvent(AppEventType::EditorGUI), m_Type(type) {}
	EditorEventType GetEditorEventType() const { return m_Type; }
	virtual std::string ToString() const override {
		return "EditorEvent";
	}
};

class RequestResourceEvent : public EditorEvent
{
public:
	std::string RequestResourceName;
public:
	RequestResourceEvent() : EditorEvent(EditorEventType::RequestResource) {}
	REGISTER_EVENT_CATEGORY(EventCategory_RenderEngine)
	virtual std::string ToString() const override {
		return "From Window : " + SentIGWindow + "  Request Resource : " + RequestResourceName;
	}
};

class RequestRenderDeviceDebugInfoEvent : EditorEvent
{
public:








public:
	RequestRenderDeviceDebugInfoEvent() : EditorEvent(EditorEventType::RequestRenderDeviceDebugInfo) {}
	REGISTER_EVENT_CATEGORY(EventCategory_RenderEngine)
	virtual std::string ToString() const override {
		return "From Window : " + SentIGWindow + "  Request DebugInformation of RenderDevice";
	}
};




