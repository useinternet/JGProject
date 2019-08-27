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
	RequestAllResourceDebugInfo,
	RequestDescriptorAllocatorDebugInfo
};
class EditorEvent : public AppEvent
{
	EditorEventType m_Type;
public:
	uint64_t    SentIGWindowID  = 0;
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
class RequestAllResourceDebugInfoEvent : public EditorEvent
{
public:
	RequestAllResourceDebugInfoEvent() : EditorEvent(EditorEventType::RequestAllResourceDebugInfo) {}
	REGISTER_EVENT_CATEGORY(EventCategory_RenderEngine)
	virtual std::string ToString() const override {
		return "RequestAllResourceDebugInfoEvent";
	}
};
class RequestDescriptorAllocatorDebugInfoEvent : public EditorEvent
{
public:
	RequestDescriptorAllocatorDebugInfoEvent() : EditorEvent(EditorEventType::RequestDescriptorAllocatorDebugInfo) {}
	REGISTER_EVENT_CATEGORY(EventCategory_RenderEngine)
	virtual std::string ToString() const override {
		return "From Window : " + SentIGWindow + "  Request DebugInformation of DescriptorAllocator";
	}
};




