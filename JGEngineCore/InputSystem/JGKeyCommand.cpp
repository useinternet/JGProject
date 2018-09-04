#include"JGKeyCommand.h"

JGKeyCommand::JGKeyCommand()
{
	m_KeyState = EKeyState::None;
}

JGKeyCommand::~JGKeyCommand()
{
}

void JGKeyCommand::Tick(const float DeltaTime)
{
	JGCommand::Tick(DeltaTime);

	if (GetIsPressed() != m_PrevPressed)
	{
		m_bKeyVariance = true;
		m_PrevPressed = GetIsPressed();
	}
	// 키를 눌렀을때
	if (m_bKeyVariance && GetIsPressed())
	{
		m_KeyState = EKeyState::Down;
	}
	// 키를 똇을떄
	else if (m_bKeyVariance && !GetIsPressed())
	{
		m_KeyState = EKeyState::Up;
		m_bKeyVariance = false;
	}
	else if (!m_bKeyVariance)
	{
		m_KeyState = EKeyState::None;
	}

	// 이벤트 발생용 트리거 키상태와 현재 키상태가 같으면 이벤트 실행.
	if (m_EventTriggerKeyState == m_KeyState)
	{
		m_BindEvent();
	}
}

void JGKeyCommand::BindKeyCommand(const EKeyState KeyState, const std::function<void()>& Event)
{
	m_EventTriggerKeyState = KeyState;
	m_BindEvent = Event;
}
