#pragma once


#include "Event.h"
// #include "DataType/Debug/RE_DebugInfo.h"
enum class RenderEngineID
{
	None,
	RenderDevice,
	Renderer,
	ResourceDataMap
};
enum class RenderEventType
{
	ToEditFromRe,
};

class RenderEvent : public Event
{
	RenderEventType m_Type;
public:
	RenderEngineID ID = RenderEngineID::None;
public:
	RenderEvent(RenderEventType type) : m_Type(type) {}
	REGISTER_EVENT_TYPE(RenderEvent)
	REGISTER_EVENT_CATEGORY(EventCategory_None)
	RenderEventType GetRenderEventType() const {
		return m_Type;
	}
	virtual std::string ToString() const override {
		return "RenderEvent";
	}
};






// Edit && RE Event
///summary  
enum class ToEditFromReCommand
{
	SendManagedResource,
	SendAllResourceDebugInfo,
	SendAllocatorDebugInfo,
	GUIAllocatorReAllocatedNotice,
};
class ToEditFromReEvent : public RenderEvent
{
private:
	ToEditFromReCommand m_Command;
public:
	uint64_t    SendIGWindowID = 0;
	std::string SendIGWindow = "None";
public:
	ToEditFromReEvent(ToEditFromReCommand cmd) :
		RenderEvent(RenderEventType::ToEditFromRe), m_Command(cmd) { }
	REGISTER_EVENT_CATEGORY(EventCategory_Application)
	virtual std::string ToString() const override {
		return "SendToEditorEvent";
	}
	ToEditFromReCommand GetCommand() const { return m_Command; }
};

// editor & renderengine event
class SendManagedResourceEvent : public ToEditFromReEvent
{
public:
	uint64_t GpuAddress = 0;
	uint32_t ResourceWidth = 0;
	uint32_t ResourceHeight = 0;
public:
	SendManagedResourceEvent() : ToEditFromReEvent(ToEditFromReCommand::SendManagedResource) {}
	virtual std::string ToString() const override {
		return "Send Resource( " + std::to_string(GpuAddress) + " ) .. To : " + SendIGWindow;
	}
};
//class SendAllResourceDebugInfoEvent : public ToEditFromReEvent
//{
//public:
//	std::vector<Debug::ResourceInfo> ResourceInfoArray;
//public:
//	SendAllResourceDebugInfoEvent() : ToEditFromReEvent(ToEditFromReCommand::SendAllResourceDebugInfo) {}
//	virtual std::string ToString() const override {
//		return "SendAllResourceDebugInfoEvent";
//	}
//};
//class SendAllocatorDebugInfoEvent : public ToEditFromReEvent
//{
//public:
//	Debug::GUIAllocatorInfo GUIDebugInfo;
//	Debug::DescritporAllocatorInfo SrvDebugInfo;
//	Debug::DescritporAllocatorInfo UavDebugInfo;
//	Debug::DescritporAllocatorInfo CbvDebugInfo;
//	Debug::DescritporAllocatorInfo RtvDebugInfo;
//	Debug::DescritporAllocatorInfo DsvDebugInfo;
//public:
//	SendAllocatorDebugInfoEvent() : ToEditFromReEvent(ToEditFromReCommand::SendAllocatorDebugInfo) {}
//	virtual std::string ToString() const override {
//		return "SendAllocatorDebugInfoEvent";
//	}
//};
class GUIAllocatorReAllocatedNoticeEvent : public ToEditFromReEvent
{
public:
	GUIAllocatorReAllocatedNoticeEvent() : ToEditFromReEvent(ToEditFromReCommand::GUIAllocatorReAllocatedNotice) {}
	virtual std::string ToString() const override {
		return "GUIAllocatorReAllocatedNoticeEvent";
	}

};


