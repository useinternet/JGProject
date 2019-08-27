#pragma once
#include "CommonCore.h"
#include <ppltasks.h>
#include <mutex>



/*

*/
class AsyncEventHandler;
class EventManager
{

	using CustomEventPool = std::unordered_map<std::string, CustomEvent>;
	using AsyncEventPool = std::vector<AsyncEvent>;
	CustomEventPool m_CustomEventPool;
	AsyncEventPool  m_AsyncEventPool;
	std::mutex m_EventMutex;
public:
	void Dispatch(const CustomEvent& e);
	const CustomEvent& DesireCustomEvent(const std::string& name);
	AsyncEventHandler GetAsyncEventHandler(AsyncEvent& e);
};


class AsyncEventHandler
{
	friend EventManager;
	bool m_IsRun = false;
	concurrency::task<void>     m_Task;
	std::shared_ptr<AsyncEvent> m_AsyncEvent;
public:
	~AsyncEventHandler() {
		if (m_IsRun)
			m_Task.wait();
	}
	void Excute();
	void Reset();
	bool IsComplete() const;
};