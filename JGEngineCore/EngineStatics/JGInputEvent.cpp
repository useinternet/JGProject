#include"JGInputEvent.h"
#include"JGLog.h"
#include"../InputSystem/InputSystem.h"

JGInputEvent::JGInputEvent() {}
JGInputEvent::~JGInputEvent() {}
void JGInputEvent::LinkInputSystem(InputSystem* input)
{
	m_pInputSystem = input;
}
void JGInputEvent::DoEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_pInputSystem)
	{
		m_pInputSystem->ReceiveInputEvent(message, wParam, lParam);
	}

	// ¿Ã∫•∆Æ
	switch (message)
	{
	case WM_LBUTTONDOWN:
		break;
	}
}
