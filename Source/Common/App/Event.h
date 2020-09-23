#pragma once

#include <functional>
#include <shared_mutex>
#include <mutex>

class IEvent
{
public:
	virtual bool IsValid() const = 0;
	virtual ~IEvent() = default;
};

template<class Return, class ... Params>
class Event :  public IEvent
{
	using EventFunc = std::function<Return(Params ...)>;
public:
	Event(const EventFunc& func) : func(func) {}
	Event() = default;
public:
	Return operator()(Params ... params) const{
		return func(params...);
	}
	virtual bool IsValid() const override {
		return func != nullptr;
	}
	virtual ~Event() = default;
private:
	EventFunc func;
};




class EventManager
{
private:
	struct EventDispatcher
	{
		std::map <uint64_t, std::unique_ptr<IEvent>> DispatchedEvents;
	};
private:
	std::shared_mutex m_EPLock;
	std::shared_mutex m_EDPLock;
	std::unordered_map<std::wstring, std::unique_ptr<IEvent>> m_EventPool;
	std::unordered_map<std::wstring, EventDispatcher> m_EventDispatcherPool;
public:
	/*
	* 1. 이름으로 이벤트 지정후 Call
	* 2. 이벤트 디스패쳐-> 디스패쳐에 함수 등록하고 다른쪽에서 이벤트를 호출하면 등록된 이벤트가 모두 호출
	
	
	*/
	/*
	
	*/
	template<class Return, class ... Params>
	bool RegisterEvent(const std::wstring& eventName, const Event<Return, Params ...>& _event_) {

		std::lock_guard<std::shared_mutex> lock(m_EPLock);
		if (m_EventPool.find(eventName) != m_EventPool.end()) {
			return false;
		}
		


		auto e_p = std::make_unique<Event<Return, Params...>>(_event_);
		m_EventPool.emplace(eventName, std::move(e_p));

		return true;
	}

	void UnRegisterEvent(const std::wstring& eventName) {
		std::lock_guard<std::shared_mutex> lock(m_EPLock);

		if (m_EventPool.find(eventName) == m_EventPool.end()) {
			return;
		}
		m_EventPool.erase(eventName);

	}



	template<class Return, class ... Params>
	uint64_t DispatchEvent(const std::wstring& eventName, const Event<Return, Params ...>& _event_) {
		std::lock_guard<std::shared_mutex> lock(m_EDPLock);

		auto e = std::make_unique<Event<Return, Params ...>>(_event_);
		size_t eID = (size_t)e.get();
		m_EventDispatcherPool[eventName].DispatchedEvents.emplace(eID, std::move(e));
		return eID;
	}

	void CancelDispatchEvent(const std::wstring& eventName, uint64_t eventID) {
		std::lock_guard<std::shared_mutex> lock(m_EDPLock);
		if (m_EventDispatcherPool.find(eventName) == m_EventDispatcherPool.end()) return;

		m_EventDispatcherPool[eventName].DispatchedEvents.erase(eventID);
		if (m_EventDispatcherPool[eventName].DispatchedEvents.empty()) m_EventDispatcherPool.erase(eventName);
	}


	template<class Return, class ... Params>
	Event<Return, Params ...> EventCall(const std::wstring& eventName) {
		IEvent* findEvent = nullptr;
		{
			std::shared_lock lock(m_EPLock);
			auto iter = m_EventPool.find(eventName);
			if (iter == m_EventPool.end())
			{
				return Event<Return, Params ...>();
			}
			findEvent = iter->second.get();
		}
		auto e = dynamic_cast<Event<Return, Params ...>*>(findEvent);
		if (e == nullptr)
		{
			return Event<Return, Params ...>();
		}
		return *e;
	}


	template<class Return, class ... Params>
	void EventNotify(const std::wstring& eventName, Params ... params){
		EventDispatcher* dispatcher = nullptr;
		{
			std::shared_lock lock(m_EDPLock);
			auto iter = m_EventDispatcherPool.find(eventName);
			if (iter == m_EventDispatcherPool.end()) return;
			dispatcher = &iter->second;
		}
		for (auto& dispachedEvent : dispatcher->DispatchedEvents)
		{
			auto e = dynamic_cast<Event<Return, Params ...>*>(dispachedEvent.second.get());
			if (e && e->IsValid())
			{
				(*e)(params ...);
			}
		}
	}
};