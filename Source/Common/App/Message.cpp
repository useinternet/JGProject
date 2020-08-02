#include "pch.h"
#include "Message.h"
#include "SystemCore.h"
#define MAX_MESSAGE_ID 64 
#define PROCESS_MESSAGE_PER_FRAME 64
using namespace std;


void MessageBus::Clear()
{

	//
	lock_guard<shared_mutex> lock(m_MsgMutex);
	
	uint32_t msgCountInQueue = (uint32_t)m_MsgQueue.size();
	uint32_t allowMaxMsgCount = m_ProcessMessagePerFrame * m_AllowMaxProcessFrame;
	uint32_t allowMinMsgCount = m_ProcessMessagePerFrame * m_AllowMinProcessFrame;
	if (msgCountInQueue >= allowMaxMsgCount)
	{
		m_ProcessMessagePerFrame = allowMaxMsgCount / 2;
	}
	else if (msgCountInQueue < allowMinMsgCount)
	{
		m_ProcessMessagePerFrame = msgCountInQueue * m_AllowMinProcessFrame;
	}

	uint32_t msgCount = 0;
	while (!m_MsgQueue.empty())
	{
		auto msg = m_MsgQueue.front(); m_MsgQueue.pop();
		for (auto& sys_pair : m_Systems)
		{
			if (sys_pair.first & msg.receiptID)
			{
				sys_pair.second->ReceiveMessage(msg);
			}
		}
		++msgCount;
		if (msgCount >= m_ProcessMessagePerFrame)
		{
			break;
		}
	}
}

void MessageBus::Post(const Message& msg)
{
	lock_guard<shared_mutex> lock(m_MsgMutex);
	m_MsgQueue.push(msg);
}

void MessageBus::Send(const Message& msg)
{
	shared_lock<shared_mutex> lock(m_MsgMutex);
	for (auto& sys_pair : m_Systems)
	{
		if (sys_pair.first & msg.receiptID)
		{
			sys_pair.second->ReceiveMessage(msg);
		}
	}
}
uint64_t MessageBus::GetID(const std::string& name) const
{
	shared_lock<shared_mutex> lock(m_MsgMutex);

	auto iter = m_IDMap.find(name);
	if (iter == m_IDMap.end()) return -1;
	else return iter->second;
}
uint64_t MessageBus::IssueID(const std::string& name, SystemCore* sys)
{
	
	if (m_Systems.size() >= MAX_MESSAGE_ID)
		assert("MessageBus::IssueID : The maximum number of msgID that can be issued is 64. ");
	if(m_IDMap.find(name) != m_IDMap.end())
		assert("MessageBus::IssueID :  name of msgID already exists.");

	shared_lock<shared_mutex> lock(m_MsgMutex);



	uint64_t id = m_IDOffset;
	m_IDOffset = m_IDOffset << 1;

	m_Systems.emplace(id, sys);
	m_IDMap.emplace(name, id);


	return id;
}
