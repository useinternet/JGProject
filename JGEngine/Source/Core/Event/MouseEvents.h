#pragma once
#include "Events.h"
#include "Common/Define.h"


namespace JG
{
	class MouseMovedEvent : public IEvent
	{
		EVENTCLASS(EEventCategory::Mouse)
	public:
		virtual ~MouseMovedEvent() override {}
		virtual String ToString() const override
		{
			return "MouseMovedEvent : [ X : " + std::to_string(X) + ", Y : " + std::to_string(Y) + " ]";
		}

	public:
		f64 X = 0;
		f64 Y = 0;
	};


	class MouseButtonPressedEvent : public IEvent
	{
		EVENTCLASS(EEventCategory::Input | EEventCategory::MouseButton | EEventCategory::Mouse)
	public:
		virtual ~MouseButtonPressedEvent() override {}
		virtual String ToString() const override
		{
			return "MouseButtonPressed : " + MouseButtonToString(MouseButton);
		}
	public:
		EMouseButton MouseButton;
	};

	class MouseButtonReleasedEvent : public IEvent
	{
		EVENTCLASS(EEventCategory::Input | EEventCategory::MouseButton | EEventCategory::Mouse)
	public:
		virtual ~MouseButtonReleasedEvent() override {}
		virtual String ToString() const override
		{
			return "MouseButtonReleased : " + MouseButtonToString(MouseButton);
		}
	public:
		EMouseButton MouseButton;
	};

	class MouseScrollMovedEvent : public IEvent
	{
		EVENTCLASS( EEventCategory::Mouse);
	public:
		virtual ~MouseScrollMovedEvent(){}
		virtual String ToString() const override
		{
			return "MouseScrollMoved Offset : " + std::to_string(Offset) + "  HOffset : " + std::to_string(HOffset);
		}
	public:
		f64 Offset  = 0.0f;
		f64 HOffset = 0.0f;
	};
	
	
}
