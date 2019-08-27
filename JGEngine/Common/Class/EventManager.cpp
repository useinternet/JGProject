#include "pch.h"
#include "EventManager.h"

using namespace std;
using namespace concurrency;

void EventManager::Dispatch(const CustomEvent& e)
{
	lock_guard<mutex> lock(m_EventMutex);
	if (m_CustomEventPool.find(e.EventName) != m_CustomEventPool.end())
	{
		ENGINE_LOG_ERROR("{0}' already register in EventManager", e.EventName);
		return;
	}
	m_CustomEventPool.emplace(e.EventName, e);
}
const CustomEvent& EventManager::DesireCustomEvent(const std::string& name)
{
	lock_guard<mutex> lock(m_EventMutex);
	if (m_CustomEventPool.find(name) == m_CustomEventPool.end())
	{
		assert(false && "This CustomEvent not exist in EventManager");
	}
	return m_CustomEventPool[name];
}
AsyncEventHandler EventManager::GetAsyncEventHandler(AsyncEvent& e)
{
	AsyncEventHandler handler;
	handler.m_AsyncEvent = make_shared<AsyncEvent>(e);
	return handler;
}



void AsyncEventHandler::Excute()
{
	if (m_IsRun)
		return;
	m_IsRun = true;
	m_Task = create_task([&]() {
		m_AsyncEvent->m_IsCompelete = false;
		m_AsyncEvent->Function();
		m_AsyncEvent->m_IsCompelete = true;
	});
}
void AsyncEventHandler::Reset()
{
	if (!IsComplete())
		m_Task.wait();
	m_IsRun = false;
	m_AsyncEvent->m_IsCompelete = false;
}
bool AsyncEventHandler::IsComplete() const
{
	return m_AsyncEvent->m_IsCompelete;
}