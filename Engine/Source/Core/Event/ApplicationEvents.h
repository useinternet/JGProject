#pragma once
#include "Events.h"
#include "Common/Define.h"
#include "Common/Type.h"

namespace JG
{
	CREATE_EVENT(EEventCategory::Application, AppOpenEvent)
	CREATE_EVENT(EEventCategory::Application, AppCloseEvent)
	CREATE_EVENT_TWO_ARG_(EEventCategory::Application, AppResizeEvent, u32, Width, 0,  u32, Height, 0,  "AppResizeEvent = [ W : " + std::to_string(Width) + ", H : " + std::to_string(Height) + " ]")




	
}
