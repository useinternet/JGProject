#pragma once
#include "Events.h"
#include "Common/Define.h"


namespace JG
{
	class AppOpenEvent : public IEvent
	{
		EVENTCLASS(EEventCategory::Application)
	public:
		virtual ~AppOpenEvent() override {}
		virtual String ToString() const override
		{
			return "AppOpenEvent";
		}
	};

	class AppCloseEvent : public IEvent
	{
		EVENTCLASS(EEventCategory::Application)
	public:
		virtual ~AppCloseEvent() override  {}
		virtual String ToString() const override
		{
			return "AppCloseEvent";
		}
	};

	class AppResizeEvent : public IEvent
	{
		EVENTCLASS(EEventCategory::Application)
	public:
		virtual ~AppResizeEvent() override {}
		virtual String ToString() const override
		{
			return "AppResizeEvent = [ W : " + std::to_string(Width) + ", H : " + std::to_string(Height) + " ]";
		}
	public:
		u32 Width  = 0;
		u32 Height = 0;
	};

	class NotifyDestroyJGObjectEvent : public IEvent
	{
		EVENTCLASS(EEventCategory::Application | EEventCategory::Destroy)
	public:
		class IJGObject* DestroyedObject = nullptr;
	public:
		virtual ~NotifyDestroyJGObjectEvent() = default;
		virtual String ToString() const override
		{
			return "NotifyDestroyJGObjectEvent";
		}
	};
	
}
