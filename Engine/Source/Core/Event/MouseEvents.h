#pragma once
#include "Events.h"
#include "Common/Define.h"
#include "Common/Type.h"

namespace JG
{
	CREATE_EVENT_TWO_ARG_(EEventCategory::Mouse, MouseMovedEvent, f64, X, 0, f64, Y, 0, "MouseMovedEvent : [ X : " + std::to_string(X) + ", Y : " + std::to_string(Y) + " ]")
	CREATE_EVENT_ONE_ARG_(EEventCategory::Input | EEventCategory::MouseButton | EEventCategory::Mouse, MouseButtonPressedEvent, EMouseButton, MouseButton, EMouseButton::Left, "MouseButtonPressed : " + MouseButtonToString(MouseButton))
	CREATE_EVENT_ONE_ARG_(EEventCategory::Input | EEventCategory::MouseButton | EEventCategory::Mouse, MouseButtonReleasedEvent, EMouseButton, MouseButton, EMouseButton::Left, "MouseButtonReleased : " + MouseButtonToString(MouseButton))
	CREATE_EVENT_TWO_ARG_(EEventCategory::Mouse, MouseScrollMovedEvent, f64, Offset, 0, f64, HOffset, 0, "MouseScrollMoved Offset : " + std::to_string(Offset) + "  HOffset : " + std::to_string(HOffset))

	
}
