#include"JGInputEvent.h"
#include"JGLog.h"


JGInputEvent::JGInputEvent() {}
JGInputEvent::~JGInputEvent() {}
void JGInputEvent::DoEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	// 이벤트
	switch (message)
	{
	case WM_LBUTTONDOWN:
		JGLog::Write(ELogLevel::Default, TT("마우스 클릭"));
		break;
	}
}
