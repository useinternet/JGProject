#include "pch.h"
#include "ImGuiSystemLayer.h"
#include "ExternalImpl/JGImGui.h"

namespace JG
{
	void ImGuiSystemLayer::OnAttach()
	{
		JGImGui::Create();
	}
	void ImGuiSystemLayer::OnDetach()
	{
		JGImGui::Destroy();
	}
	void ImGuiSystemLayer::Begin() 
	{
		Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, SchedulePriority::ImGuiSystemLayer, SCHEDULE_BIND_FN(&ImGuiSystemLayer::Update));
		
	}
	void ImGuiSystemLayer::OnEvent(IEvent& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<AppResizeEvent>(EVENT_BIND_FN(&ImGuiSystemLayer::Resize));
	}
	String ImGuiSystemLayer::GetLayerName()
	{
		return "ImGuiSystemLayer";
	}


	EScheduleResult ImGuiSystemLayer::Update()
	{
		JGImGui::GetInstance().NewFrame();

		return EScheduleResult::Continue;
	}


	bool ImGuiSystemLayer::Resize(AppResizeEvent& e)
	{
		if (JGImGui::IsValid() == true)
		{
			JGImGui::GetInstance().Resize(e.Width, e.Height);
		}
		return true;
	}
}

