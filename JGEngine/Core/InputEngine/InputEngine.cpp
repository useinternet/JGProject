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
		}
	}
	void InputEngine::Update()
	{
		m_hWnd = GetFocus();
		ENGINE_PERFORMANCE_TIMER("Application", "InputEngine");

		m_KeyDownList.clear();
		m_KeyUpList.clear();
		m_MBtDownList.clear();
		m_MBtUpList.clear();

		while (!m_KeyNoneBuffer.empty())
		{
			KeyCode code = m_KeyNoneBuffer.front(); m_KeyNoneBuffer.pop();
			m_KeyMap[code.ToInt()] = Key_None;
		}
		while (!m_KeyDownBuffer.empty())
		{
			KeyCode code = m_KeyDownBuffer.front(); m_KeyDownBuffer.pop();
			m_KeyMap[code.ToInt()] = Key_Down;
		}
		while (!m_KeyUpBuffer.empty())
		{
			KeyCode code = m_KeyUpBuffer.front(); m_KeyUpBuffer.pop();
			m_KeyMap[code.ToInt()] = Key_Up;
			m_KeyNoneBuffer.push(code);
		}
		

		for (int i = 0; i < KeyMapCount; ++i)
		{
			EKeyState state = m_KeyMap[i];
			KeyCode code(i);
			switch (state)
			{
			case Key_Down:
				if (code == KeyCode::LeftMouseButton | code == KeyCode::RightMouseButton |
					code == KeyCode::MouseWheelButton)
				{
					m_MBtDownList.push_back(code);
				}
				else
					m_KeyDownList.push_back(code);
				break;
			case Key_Up:
				if (code == KeyCode::LeftMouseButton | code == KeyCode::RightMouseButton |
					code == KeyCode::MouseWheelButton)
				{
					m_MBtUpList.push_back(code);
				}
				else
					m_KeyUpList.push_back(code);
				break;
			}
		}
	}
	void InputEngine::OnEvent(Event& e)
	{

	}

	bool InputEngine::GetKeyDown(KeyCode code)
	{
		return m_KeyMap[code.ToInt()] == Key_Down;
	}
	bool InputEngine::GetKeyUp(KeyCode code)
	{
		return m_KeyMap[code.ToInt()] == Key_Up;
	}

	const JVector2Int InputEngine::GetMousePosition()
	{
		return m_MousePos;
	}
	const JVector2Int InputEngine::GetMouseDelta()
	{
		return { m_MousePos.x - m_PrevMousePos.x, m_MousePos.y - m_PrevMousePos.y };
	}
	void InputEngine::NotifyKeyDown(KeyCode code)
	{
		m_KeyDownBuffer.push(code);
	}
	void InputEngine::NotifyKeyUp(KeyCode code)
	{
		m_KeyUpBuffer.push(code);
	}
	void InputEngine::NotifyMousePos(int x, int y)
	{
		m_PrevMousePos = m_MousePos;
		m_MousePos.x = x;
		m_MousePos.y = y;
	}
}