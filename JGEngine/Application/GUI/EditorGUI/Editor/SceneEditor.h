#pragma once
#include "GUI/EditorGUI/IGWindow.h"



namespace JE
{
	class SceneEditor : public IGWindow
	{

	protected:
		virtual void OnLoad() override;
		virtual void OnGUI() override;
		virtual void OnDestroy() override;
		virtual void OnEventFromRE(ToEditFromReEvent& e) override;
		void SendManagedResource(SendManagedResourceEvent& e);
		void GUIAllocatorReAllocatedNotice(GUIAllocatorReAllocatedNoticeEvent& e);
	private:
		uint64_t m_SceneAddress;
		float    m_SceneAspect;
	};
}