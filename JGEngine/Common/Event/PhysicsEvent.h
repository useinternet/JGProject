#pragma once
#include "Event.h"


class PhysicsEvent : public Event
{
public:
	REGISTER_EVENT_TYPE(PhysicsEvent)
	REGISTER_EVENT_CATEGORY(EventCategory_None)
	virtual std::string ToString() const override
	{
		return "PhysicsEvent";
	}
};