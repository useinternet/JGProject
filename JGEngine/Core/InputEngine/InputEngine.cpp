#include "pch.h"
#include "InputEngine.h"
#include "DataType/KeyCode.h"
using namespace std;


namespace IE
{
	InputEngine::InputEngine(const GlobalLinkStream& stream)
		: EngineCore(stream)
	{
		for (int i = 0; i < KeyMapCount; ++i)
		{
			m_KeyMap[i]     = Key_None;
			m_PrevKeyMap[i] = Key_None;
		}
	}

	void InputEngine::Init(HWND hWnd)
	{
		m_hWnd = hWnd;
	}
	void InputEngine::Update()
	{
		ENGINE_PERFORMANCE_TIMER("Application", "InputEngine");

		for (int i = 0; i < KeyMapCount; ++i)
		{
			m_PrevKeyMap[i] = m_KeyMap[i];
			if (GetAsyncKeyState(i))
			{
				m_KeyMap[i] = Key_Down;
			}
			else
			{
				if (m_KeyMap[i] == Key_Down)
				{
					m_KeyMap[i] = Key_Up;
				}
				else
					m_KeyMap[i] = Key_None;
			}
		}
		

		m_PrevMousePos = m_MousePos;

		POINT p;
		GetCursorPos(&p);
		ScreenToClient(m_hWnd, &p);
		m_MousePos = { (float)p.x, (float)p.y };
	}
	void InputEngine::OnEvent(Event& e)
	{

	}

	bool InputEngine::GetKeyDown(KeyCode code)
	{
		if (m_KeyMap[code.ToInt()] == Key_Down)
			return true;
		return false;
	}
	bool InputEngine::GetKeyUp(KeyCode code)
	{
		if (m_KeyMap[code.ToInt()] == Key_Up)
			return true;
		return false;
	}
	bool InputEngine::GetKeyAsButton(KeyCode code)
	{
		if (m_KeyMap[code.ToInt()] == Key_Up &&
			m_PrevKeyMap[code.ToInt()] == Key_Down)
		{
			return true;
		}
		return false;
	}

	const JVector2 InputEngine::GetMousePosition()
	{
		return m_MousePos;
	}
	const JVector2 InputEngine::GetMouseDelta()
	{
		return { m_MousePos.x - m_PrevMousePos.x, m_MousePos.y - m_PrevMousePos.y };
	}

}