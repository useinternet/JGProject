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

	// 이벤트
	switch (message)
	{
	case WM_LBUTTONDOWN:
		JGLog::Write(ELogLevel::Default, TT("마우스 클릭"));
		break;
	}
}
