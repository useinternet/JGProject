#pragma once
#include "GUI/EditorGUI/IGWindow.h"



namespace JE
{
	class SceneEditor : public IGWindow
	{
		AsyncEventHandler m_EventHandler;
	protected:
		virtual void OnLoad() override;
		virtual void OnGUI() override;
		virtual void OnDestroy() override;



		virtual void SendManagedResource(SendManagedResourceEvent& e) override;
		virtual void GUIAllocatorReAllocated(GUIAllocatorReAllocatedEvent& e) override;
	private:
		uint64_t m_SceneAddress;
		float    m_SceneAspect;
	};
}