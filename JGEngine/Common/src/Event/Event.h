#pragma once

#include <string>
#include <functional>
#include <unordered_map>
// Event
enum class EventType
{
	WindowEvent,
	AppEvent,
	InputEvent,
	RenderEvent,
	PhysicsEvent,
	SoundEvent,
	GameFrameWorkEvent,
	AsyncEvent,
	CustomEvent,
};
enum EventCategory
{
	EventCategory_None = 0,
	EventCategory_Application = 0x0001,
	EventCategory_InputEngine = 0x0002,
	EventCategory_RenderEngine = 0x0004,
	EventCategory_PhysicsEngine = 0x008,
	EventCategory_SoundEngine = 0x0016,
	EventCategory_Game = 0x0032,
	EventCategory_All = 0xfffffff
};

#define REGISTER_EVENT_TYPE(event_type)  \
 static  EventType GetStaticType() { return EventType::##event_type; } \
 virtual EventType GetEventType() const override { return EventType::event_type;} \
 virtual const char* GetName() const override { return #event_type; } \

#define REGISTER_EVENT_CATEGORY(category) \
 virtual int GetEventCategory() const override { return category; } \


#define CONVERT_EVENT(EventType, Event) *(##EventType*)&##Event  

class Event
{
public:
	virtual EventType GetEventType() const = 0;
	virtual const char* GetName()  const = 0;
	virtual int GetEventCategory() const = 0;
	virtual std::string ToString() const = 0;
public:
	bool IsInCategory(EventCategory gory) {
		return GetEventCategory() & gory;
	}
};


class CustomEvent : Event
{
	friend class EventManager;
private:
	std::string EventName;
	// 파라미터
public:
	REGISTER_EVENT_TYPE(CustomEvent)
	REGISTER_EVENT_CATEGORY(EventCategory_None)
	virtual std::string ToString() const override {
		return EventName;
	}
	CustomEvent() = default;
	CustomEvent(const std::string& event_name) : EventName(event_name) {}
};

class AsyncEvent : Event
{
	friend class AsyncEventHandler;
	friend class EventManager;
private:
	bool m_IsCompelete;
public:
	std::function<void()> Function;
public:
	REGISTER_EVENT_TYPE(AsyncEvent)
	REGISTER_EVENT_CATEGORY(EventCategory_None)
	virtual std::string ToString() const override {
		return "AsyncEvent";
	}
};



//// Only Write in Cpp file  \\ macro(eventname, return type, param...)
//#define CREATE_DISPATCH_CUSTOMEVENT(Event_Name, Return, ...) \
//class Dispatch_##Event_Name : public CustomEvent  \
//{\
//public: \
//    std::function<##Return(__VA_ARGS__)> Function; \
//public: \
//    Dispatch_##Event_Name(const std::function<##Return(__VA_ARGS__)>& func) : CustomEvent(#Event_Name), Function(func) {} \
//}; \
//
//// Only Write in Cpp file \\ macro(eventname, return type, param...)
//#define DESIRE_DISPATCH_CUSTOMEVENT(Event_Name, Return, ...) \
//CREATE_DISPATCH_CUSTOMEVENT(Event_Name, Return, __VA_ARGS__) \
//class Event_Name \
//{\
//public: \
//	std::function<##Return(__VA_ARGS__)> Excute;\
//public:\
//	Event_Name() {\
//		auto custom_event = *(Dispatch_##Event_Name*)&EventManager::DesireCustomEvent(#Event_Name);\
//		Excute = custom_event.Function; \
//	}\
//};\









// 비동기 이벤트
/*
비동기 이벤트 매니저에 함수를 등록하면 비동기 이벤트 핸들러가 나오는데 여기서 다시 반복하거나 로직이 끝낫는지 등등 알수있다.
비동기 이벤트 매니저는 글로벌 클래스

이것도 밑에꺼랑 비슷
*/



// 커스텀 이벤트
/*
커스텀 이벤트는 이벤트이름과 함수를 저장
다른곳에서 Dispatcher로 이름을 대면 저장된 함수를 실행하는 거

1. CustomEvent 저장 <- 이름 중복 되면 에러 로그
2. CreateCustomEvent(EventName, ReturnType, ...Param) 으로 이벤트 생성하고 <- 무조건 cpp파일로
3. Dispatch_EventName_Event 클래스 자동 생성후 Dispatcher 클래스에 저장
4. LoadCustomEvent(EventName, ReturnType, ...Param) 으로 이벤트 불러오고 <- 무조건 cpp파일로
5. EventName 클래스로 ReturnType Excute(Param, Param) 함수 생성

*/

