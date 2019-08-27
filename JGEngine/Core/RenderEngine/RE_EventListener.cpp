#include "pch.h"
#include "RE_EventListener.h"
#include "RenderEngine.h"
#include "Object/DxObject/RenderDevice.h"
#include "Object/DxObject/ResourceDataMap.h"


namespace RE
{
	void RE_EventListener::OnRenderEvent(RenderEvent& e)
	{

	}
	void RE_EventListener::OnAppEvent(AppEvent& e)
	{
		static std::mutex AppMutex;
		std::lock_guard<std::mutex> lock(AppMutex);
		if (e.GetAppEventType() == AppEventType::EditorGUI)
		{
			EditorEvent editorEvent = CONVERT_EVENT(EditorEvent, e);
			if (editorEvent.SentIGWindowID == 0)
			{
				RE_LOG_ERROR("SendIGWindowID is 0");
				return;
			}
			// RequestResourceEvent
			if (editorEvent.GetEditorEventType() == EditorEventType::RequestResource)
			{
				RequestResourceEvent requestEvent = CONVERT_EVENT(RequestResourceEvent, e);
				
				m_RenderDevice->GUIAllocate(requestEvent.RequestResourceName);

				SendManagedResourceEvent send_event;
				send_event.ID = RenderEngineID::RenderDevice;
				send_event.SendIGWindowID = requestEvent.SentIGWindowID;
				send_event.SendIGWindow = requestEvent.SentIGWindow;
				send_event.GpuAddress = m_RenderDevice->GetGUIAllocation(requestEvent.RequestResourceName);
				auto resource = m_RenderDevice->GetRegisteredGUITexture(requestEvent.RequestResourceName);
				send_event.ResourceWidth = (uint32_t)resource.GetDesc().Width;
				send_event.ResourceHeight = resource.GetDesc().Height;
				GlobalLinkData::OnEvent(send_event);
				return;
			}
			// RequestAllResourceDebugInfo
			if (editorEvent.GetEditorEventType() == EditorEventType::RequestAllResourceDebugInfo)
			{
				RequestAllResourceDebugInfoEvent requestEvent = CONVERT_EVENT(RequestAllResourceDebugInfoEvent, e);
				
				SendAllResourceDebugInfoEvent sendEvent;
				sendEvent.ID = RenderEngineID::ResourceDataMap;
				sendEvent.SendIGWindow = requestEvent.SentIGWindow;
				sendEvent.SendIGWindowID = requestEvent.SentIGWindowID;
				ResourceDataMap::GetAllResourceDebugInfo(sendEvent.ResourceInfoArray);
				GlobalLinkData::OnEvent(sendEvent);
				return;
			}
			// RequestDescriptorAllocatorDebugInfo
			if (editorEvent.GetEditorEventType() == EditorEventType::RequestDescriptorAllocatorDebugInfo)
			{
				auto requestEvent = CONVERT_EVENT(RequestDescriptorAllocatorDebugInfoEvent, e);
				SendAllocatorDebugInfoEvent sendEvent;
				sendEvent.ID = RenderEngineID::RenderDevice;
				sendEvent.SendIGWindowID = requestEvent.SentIGWindowID;
				sendEvent.SendIGWindow = requestEvent.SentIGWindow;
				m_RenderDevice->GetGUIAllocatorDebugInfo(sendEvent.GUIDebugInfo);
				m_RenderDevice->GetSrvDescriptorAllocatorDebugInfo(sendEvent.SrvDebugInfo);
				m_RenderDevice->GetUavDescriptorAllocatorDebugInfo(sendEvent.UavDebugInfo);
				m_RenderDevice->GetCbvDescriptorAllocatorDebugInfo(sendEvent.CbvDebugInfo);
				m_RenderDevice->GetRtvDescriptorAllocatorDebugInfo(sendEvent.RtvDebugInfo);
				m_RenderDevice->GetDsvDescriptorAllocatorDebugInfo(sendEvent.DsvDebugInfo);
				GlobalLinkData::OnEvent(sendEvent);
				return;
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
		static std::mutex WindowMutex;
		std::lock_guard<std::mutex> lock(WindowMutex);
		if (e.GetWindowEventType() == WindowEventType::Resize)
		{
			WindowResizeEvent resizeEvent = CONVERT_EVENT(WindowResizeEvent,e);
			m_RenderDevice->Resize(resizeEvent.width, resizeEvent.height);
		}
	}
}
