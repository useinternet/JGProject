#pragma once


#include "Event.h"

enum class RenderEventType
{
	RenderDevice,
	Renderer,
};
enum class RenderDeviceEventType
{
	SendManagedResource,
	GUIAllocatorReAllocated,
};
enum class RendererEventType
{

};
class RenderEvent : public Event
{
	RenderEventType m_Type;
public:
	RenderEvent(RenderEventType type) : m_Type(type) {}
	REGISTER_EVENT_TYPE(RenderEvent)
	REGISTER_EVENT_CATEGORY(EventCategory_None)
	RenderEventType GetRenderEventType() {
		return m_Type;
	}
	virtual std::string ToString() const override {
		return "RenderEvent";
	}
};


class RenderDeviceEvent : public RenderEvent
{
	RenderDeviceEventType m_Type;
public:
	RenderDeviceEvent(RenderDeviceEventType type) : RenderEvent(RenderEventType::RenderDevice), m_Type(type) {}
	RenderDeviceEventType GetRenderDeviceEventType() {
		return m_Type;
	}
	virtual std::string ToString() const override {
		return "RenderDeviceEventType";
	}
};


class RendererEvent : public RenderEvent
{
	RendererEventType m_Type;
public:
	RendererEvent(RendererEventType type) : RenderEvent(RenderEventType::Renderer), m_Type(type) {}
	RendererEventType GetRenderDeviceEventType() {
		return m_Type;
	}
	virtual std::string ToString() const override {
		return "RendererEvent";
	}


};



class SendManagedResourceEvent : public RenderDeviceEvent
{
public:
	std::string RequesterName = "None";
	uint64_t GpuAddress = 0;
	uint32_t ResourceWidth = 0;
	uint32_t ResourceHeight = 0;
public:
	SendManagedResourceEvent() : RenderDeviceEvent(RenderDeviceEventType::SendManagedResource) {}
	REGISTER_EVENT_CATEGORY(EventCategory_Application)
	virtual std::string ToString() const override {
		return "Send Resource( " + std::to_string(GpuAddress) + " ) .. To : " + RequesterName;
	}
};

class GUIAllocatorReAllocatedEvent : public RenderDeviceEvent
{
public:
	GUIAllocatorReAllocatedEvent() : RenderDeviceEvent(RenderDeviceEventType::GUIAllocatorReAllocated) {}
	REGISTER_EVENT_CATEGORY(EventCategory_Application)
	virtual std::string ToString() const override {
		return "GUIAllocatorReAllcatedEvent";
	}

};


