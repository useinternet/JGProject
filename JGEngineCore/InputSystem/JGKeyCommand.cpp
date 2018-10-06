#include"JGKeyCommand.h"

JGKeyCommand::JGKeyCommand()
{
	m_KeyState = EKeyState::None;
}

JGKeyCommand::~JGKeyCommand()
{
}

void JGKeyCommand::Tick()
{
	JGCommand::Tick();

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
	}
	else if (!m_bKeyVariance)
	{
		m_KeyState = EKeyState::None;
	}


	// 키를 누르거나 떼면 바인딩된 함수가 실행..
	if (m_bIsBindKeyDownEvent && m_KeyState == EKeyState::Down)
	{
		m_BindKeyDownEvent();
	}
	if (m_bIsBindKeyUpEvent && m_KeyState == EKeyState::Up)
	{
		m_BIndKeyUpEvent();
		m_bKeyVariance = false;
	}


}

void JGKeyCommand::BindKeyCommand(const EKeyState KeyState, const std::function<void()>& Event)
{
	switch (KeyState)
	{
	case EKeyState::Down:
		m_BindKeyDownEvent = Event;
		m_bIsBindKeyDownEvent = true;
		break;
	case EKeyState::Up:
		m_BIndKeyUpEvent = Event;
		m_bIsBindKeyUpEvent = true;
		break;
	}
}

void JGKeyCommand::KillCommand()
{
	m_bIsBindKeyDownEvent = false;
	m_bIsBindKeyUpEvent = false;
}
