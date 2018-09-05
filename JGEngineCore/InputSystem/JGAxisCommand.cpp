#include"JGAxisCommand.h"
#include"JGPressManager.h"
JGAxisCommand::JGAxisCommand()
{
}

JGAxisCommand::~JGAxisCommand()
{
}

void JGAxisCommand::Tick()
{
	// 이벤트가 바인딩 되면 해당 값으로 실행한다. 
	if (bIsBindEvent)
	{
		m_BindEvent(GetPressManager()->GetAxisKey(GetKey()));
	}
}

void JGAxisCommand::BindAxisCommand(const std::function<void(const float)>& Event)
{
	bIsBindEvent = true;
	m_BindEvent = Event;
}
