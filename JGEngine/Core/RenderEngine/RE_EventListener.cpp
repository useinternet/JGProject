#include "pch.h"
#include "RE_EventListener.h"
#include "RenderEngine.h"
#include "Object/DxObject/RenderDevice.h"


namespace RE
{
	void RE_EventListener::OnRenderEvent(RenderEvent& e)
	{

	}
	void RE_EventListener::OnAppEvent(AppEvent& e)
	{
		if (e.GetAppEventType() == AppEventType::EditorGUI)
		{
			EditorEvent editorEvent = CONVERT_EVENT(EditorEvent, e);

			// RequestResourceEvent
			if (editorEvent.GetEditorEventType() == EditorEventType::RequestResource)
			{
				RequestResourceEvent requestEvent = CONVERT_EVENT(RequestResourceEvent, e);
				
				m_RenderDevice->GUIAllocate(requestEvent.RequestResourceName);

				SendManagedResourceEvent send_event;
				send_event.RequesterName = requestEvent.SentIGWindow;
				send_event.GpuAddress = m_RenderDevice->GetGUIAllocation(requestEvent.RequestResourceName);
				auto resource = m_RenderDevice->GetRegisteredGUITexture(requestEvent.RequestResourceName);
				send_event.ResourceWidth = (uint32_t)resource.GetDesc().Width;
				send_event.ResourceHeight = resource.GetDesc().Height;
				GlobalLinkData::OnEvent(send_event);
			}
		}
	}
	void RE_EventListener::OnInputEvent(InputEvent& e)
	{
	}
	void RE_EventListener::OnPhysicsEvent(PhysicsEvent& e)
	{
	}
	void RE_EventListener::OnSoundEvent(SoundEvent& e)
	{
	}
	void RE_EventListener::OnGmaeFrameWorkEvent(GameFrameWorkEvent& e)
	{
	}
	void RE_EventListener::OnWindowEvent(WindowEvent& e)
	{
		if (e.GetWindowEventType() == WindowEventType::Resize)
		{
			WindowResizeEvent resizeEvent = CONVERT_EVENT(WindowResizeEvent,e);
			m_RenderDevice->Resize(resizeEvent.width, resizeEvent.height);
		}
	}
}
