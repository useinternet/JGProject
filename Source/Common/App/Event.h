#pragma once

#include <functional>

class EventBase
{
public:
	virtual void operator()() = 0;
	virtual bool IsValid() const = 0;
};



template<class Return, class ... Factors>
class Event : public EventBase
{
	using EventFunc = std::function<Return(Factors ...)>;
public:
	void Bind(const EventFunc& func) {
		this->func = func;
	}
	virtual void operator()() override {
		func();
	}
	virtual bool IsValid() const override {
		return func != nullptr;
	}
private:
	EventFunc func;
};



class EventManager
{




	
};