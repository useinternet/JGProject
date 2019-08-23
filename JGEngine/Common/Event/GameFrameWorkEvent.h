#pragma once
#include "Event.h"


class GameFrameWorkEvent : public Event
{
public:
	REGISTER_EVENT_TYPE(GameFrameWorkEvent)
	REGISTER_EVENT_CATEGORY(EventCategory_None)
	virtual std::string ToString() const override
	{
		return "GameFrameWorkEvent";
	}
};