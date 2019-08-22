#pragma once



#include "DataType/Abstract/EventListener.h"


namespace RE
{
	class RenderDevice;
	class RenderEngine;
	class RE_EventListener : public EventListener
	{
	public:
		RenderEngine* m_RenderEngine = nullptr;
		RenderDevice* m_RenderDevice = nullptr;
	protected:
		virtual void OnRenderEvent(RenderEvent& e) override;
		virtual void OnAppEvent(AppEvent& e) override;
		virtual void OnInputEvent(InputEvent& e) override;
		virtual void OnPhysicsEvent(PhysicsEvent& e) override;
		virtual void OnSoundEvent(SoundEvent& e) override;
		virtual void OnGmaeFrameWorkEvent(GameFrameWorkEvent& e) override;
		virtual void OnWindowEvent(WindowEvent& e) override;
	};
}