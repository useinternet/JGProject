#pragma once
#include "Event/Event.h"


class SoundEvent : public Event
{
public:
	REGISTER_EVENT_TYPE(SoundEvent)
	REGISTER_EVENT_CATEGORY(EventCategory_None)
	virtual std::string ToString() const override
	{
		return "SoundEvent";
	}
};