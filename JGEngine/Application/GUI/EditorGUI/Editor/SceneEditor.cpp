#include "pch.h"
#include "SceneEditor.h"


namespace JE
{
	void SceneEditor::OnLoad()
	{
		m_SceneAddress = 0;
		m_SceneAspect = 0.0f;
		RequestResourceEvent e;
		e.SentIGWindow = GetName();
		e.RequestResourceName = "FinalScene";
		EventNotify(e);


		AsyncEvent test;
		test.Function = []() {
			ENGINE_LOG_ERROR("이것은 테스트 로그이다.!!!");
		};
		m_EventHandler = GlobalLinkData::EngineEventManager->GetAsyncEventHandler(test);
		m_EventHandler.Excute();
	}
	void SceneEditor::OnGUI()
	{
		if (m_EventHandler.IsComplete())
		{
			ENGINE_LOG_ERROR("테스트로그이벤트 완료!!");
		}
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
	void SceneEditor::SendManagedResource(SendManagedResourceEvent& e)
	{
		if (e.RequesterName != GetName())
		{
			return;
		}
		m_SceneAddress = e.GpuAddress;
		m_SceneAspect = (float)e.ResourceHeight / (float)e.ResourceWidth;
	}
	void SceneEditor::GUIAllocatorReAllocated(GUIAllocatorReAllocatedEvent& e)
	{
		RequestResourceEvent request_e;
		request_e.SentIGWindow = GetName();
		request_e.RequestResourceName = "FinalScene";
		EventNotify(e);
	}
}