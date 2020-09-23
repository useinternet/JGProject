#include "pch.h"
#include "InputEngine.h"
#include "InputIF.h"


using namespace std;
std::unique_ptr<InputIF> m_IF = nullptr;


namespace IE
{
	void InputEngine::Awake()
	{
		if (m_IF == nullptr) m_IF = make_unique<InputIF>();

	}
	void InputEngine::Start()
	{

	}
	void InputEngine::Update()
	{
		m_IF->UpdateKeyState();
	}
	void InputEngine::Destroy()
	{
		m_IF = nullptr;

	}
	void InputEngine::ReceiveMessage(const Message& msg)
	{

	}
}

JVector2 InputIF::GetCursorPos()
{
	POINT p;
	::GetCursorPos(&p);
	return JVector2((float)p.x, (float)p.y);
}

bool InputIF::GetMouseButtonDown(int bt)
{
	auto& keyboardState = m_IF->m_KeyboardStates;
	switch (bt)
	{
	case 0: return keyboardState[KeyCode::LeftMouseButton] ==  KeyState_Down;
	case 1: return keyboardState[KeyCode::RightMouseButton] == KeyState_Down;
	case 2: return keyboardState[KeyCode::MouseWheelButton] == KeyState_Down;
	default: return false;
	}
}

bool InputIF::GetMouseButton(int bt)
{
	auto& keyboardState = m_IF->m_KeyboardStates;
	switch (bt)
	{
	case 0: return keyboardState[KeyCode::LeftMouseButton]  == KeyState_Hold;
	case 1: return keyboardState[KeyCode::RightMouseButton] == KeyState_Hold;
	case 2: return keyboardState[KeyCode::MouseWheelButton] == KeyState_Hold;
	default: return false;
	}
}

bool InputIF::GetMouseButtonUp(int bt)
{
	auto& keyboardState = m_IF->m_KeyboardStates;
	switch (bt)
	{
	case 0: return keyboardState[KeyCode::LeftMouseButton] == KeyState_Up;
	case 1: return keyboardState[KeyCode::RightMouseButton] == KeyState_Up;
	case 2: return keyboardState[KeyCode::MouseWheelButton] == KeyState_Up;
	default: return false;
	}
}



bool InputIF::GetKeyDown(KeyCode code)
{
	auto& keyboardState = m_IF->m_KeyboardStates;
	return keyboardState[code.ToInt()] == KeyState_Down;
}

bool InputIF::GetKey(KeyCode code)
{
	auto& keyboardState = m_IF->m_KeyboardStates;
	return keyboardState[code.ToInt()] == KeyState_Hold;
}

bool InputIF::GetKeyUp(KeyCode code)
{
	auto& keyboardState = m_IF->m_KeyboardStates;
	return keyboardState[code.ToInt()] == KeyState_Up;
}

void InputIF::UpdateKeyState()
{
	Concurrency::parallel_for(0, 256, [&](int i)
	{
		if(GetAsyncKeyState(i) & 0x8000)
		{
			if (m_KeyboardStates[i] == KeyState_Down || m_KeyboardStates[i] == KeyState_Hold)
			{
				m_KeyboardStates[i] = KeyState_Hold;
			}
			else
			{
				m_KeyboardStates[i] =  KeyState_Down;
			}
		}
		else
		{
			if (m_KeyboardStates[i] == KeyState_Down || m_KeyboardStates[i] == KeyState_Hold)
			{
				m_KeyboardStates[i] = KeyState_Up;
			}
			else
			{
				m_KeyboardStates[i] = KeyState_None;
			}
		}


	});



}


