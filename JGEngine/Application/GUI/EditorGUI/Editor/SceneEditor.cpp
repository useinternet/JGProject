#include "pch.h"
#include "SceneEditor.h"


namespace JE
{
	void SceneEditor::OnLoad()
	{
		m_SceneAddress = 0;
		m_SceneAspect = 0.0f;
		RequestResourceEvent e;
		e.SentIGWindowID = (uint64_t)this;
		e.SentIGWindow = GetName();
		e.RequestResourceName = "Default";
		EventNotify(e);
	}
	void SceneEditor::OnGUI()
	{
		ImGui::Text(("GPUAddress : " + std::to_string(m_SceneAddress)).c_str());
		ImGui::SameLine();
		ImGui::Text(("FPS : " + std::to_string(GlobalLinkData::GetFPS())).c_str());
		ImGui::SameLine();
		ImGui::Text(("Tick : " + std::to_string(GlobalLinkData::GetTick())).c_str());
		ImGui::SameLine();
		ImGui::Text(("Total : " + std::to_string(GlobalLinkData::GetTotalTime())).c_str());

		if (m_SceneAddress)
		{
			float scene_size = std::max<float>(400, ImGui::GetWindowSize().x - 15.0f);
			
			ImGui::Image(
				(ImTextureID)m_SceneAddress,
				{ scene_size , scene_size * m_SceneAspect},
				{ 0,0 },
				{ 1,1 },
				{ 1,1,1,1 },
				{ 1,1,1,1 });
		}

	}
	void SceneEditor::OnDestroy()
	{


	}
	void SceneEditor::OnEventFromRE(ToEditFromReEvent& e)
	{
		switch (e.GetCommand())
		{
		case ToEditFromReCommand::SendManagedResource:
			SendManagedResource(CONVERT_EVENT(SendManagedResourceEvent, e));
			return;

		case ToEditFromReCommand::GUIAllocatorReAllocatedNotice:
			GUIAllocatorReAllocatedNotice(CONVERT_EVENT(GUIAllocatorReAllocatedNoticeEvent, e));
			return;

		}
	}
	void SceneEditor::SendManagedResource(SendManagedResourceEvent& e)
	{
		m_SceneAddress = e.GpuAddress;
		m_SceneAspect = (float)e.ResourceHeight / (float)e.ResourceWidth;
	}
	void SceneEditor::GUIAllocatorReAllocatedNotice(GUIAllocatorReAllocatedNoticeEvent& e)
	{
		RequestResourceEvent request_e;
		request_e.SentIGWindowID = (uint64_t)this;
		request_e.SentIGWindow = GetName();
		request_e.RequestResourceName = "FinalScene";
		EventNotify(request_e);
	}
}