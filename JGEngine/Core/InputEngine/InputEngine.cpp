#include "pch.h"
#include "InputEngine.h"
#include "DataType/KeyCode.h"
using namespace std;


namespace IE
{
	InputEngine::InputEngine(const GlobalLinkStream& stream)
		: EngineCore(stream)
	{
		for (int i = 0; i<KeyMapCount; ++i)
		{
			m_KeyMap[i] = Key_None;
		}

		m_MousePos = { 0,0 };
		m_PrevMousePos = { 0,0 };
	}

	void InputEngine::Init(HWND hWnd)
	{
		m_hWnd = hWnd;
	}
	void InputEngine::Update()
	{


	}
	void InputEngine::OnEvent(Event& e)
	{

	}
	void InputEngine::KeyboardUpdate(EKeyState state, unsigned int  code)
	{
		
	}
	void InputEngine::MouseUpdate(EKeyState state, unsigned int  code)
	{
		
	}
}