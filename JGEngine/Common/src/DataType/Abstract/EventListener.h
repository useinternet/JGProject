#pragma once
#include "Event/Event.h"
#include "Event/AppEvents.h"
#include "Event/InputEvents.h"
#include "Event/RenderEvent.h"
#include "Event/WindowEvents.h"
#include "Event/GameFrameWorkEvent.h"
#include "Event/PhysicsEvent.h"
#include "Event/SoundEvent.h"

class EventListener
{
	// Copy here
	/*
		virtual void OnRenderEvent(RenderEvent& e) override;
		virtual void OnAppEvent(AppEvent& e) override;
		virtual void OnInputEvent(InputEvent& e) override;
		virtual void OnPhysicsEvent(PhysicsEvent& e) override;
		virtual void OnSoundEvent(SoundEvent& e) override;
		virtual void OnGmaeFrameWorkEvent(GameFrameWorkEvent& e) override;
		virtual void OnWindowEvent(WindowEvent& e) override;
	*/
protected:
	virtual void OnRenderEvent(RenderEvent& e) = 0;
	virtual void OnAppEvent(AppEvent& e) = 0;
	virtual void OnInputEvent(InputEvent& e) = 0;
	virtual void OnPhysicsEvent(PhysicsEvent& e) = 0;
	virtual void OnSoundEvent(SoundEvent& e) = 0;
	virtual void OnGmaeFrameWorkEvent(GameFrameWorkEvent& e) = 0;
	virtual void OnWindowEvent(WindowEvent& e) = 0;
public:
	void OnEvent(Event& e) {

		switch (e.GetEventType())
		{
		case EventType::AppEvent:
			OnAppEvent(CONVERT_EVENT(AppEvent, e));
			return;
		case EventType::GameFrameWorkEvent:
			OnGmaeFrameWorkEvent(CONVERT_EVENT(GameFrameWorkEvent, e));
			return;
		case EventType::InputEvent:
			OnInputEvent(CONVERT_EVENT(InputEvent, e));
			return;
		case EventType::PhysicsEvent:
			OnPhysicsEvent(CONVERT_EVENT(PhysicsEvent, e));
			return;
		case EventType::RenderEvent:
			OnRenderEvent(CONVERT_EVENT(RenderEvent,e));
			return;
		case EventType::SoundEvent:
			OnSoundEvent(CONVERT_EVENT(SoundEvent, e));
			return;
		case EventType::WindowEvent:
			OnWindowEvent(CONVERT_EVENT(WindowEvent, e));
			return;
		}
	}

};