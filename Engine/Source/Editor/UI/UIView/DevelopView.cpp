#include "pch.h"
#include "DevelopView.h"
#include "Application.h"
#include "UI/UIManager.h"
#include "Graphics/Renderer.h"
namespace JG
{



	DevelopView::DevelopView()
	{
		UIManager::GetInstance().RegisterMainMenuItem("Windows/Devlop", 0, [&]()
		{
			Open();
		}, nullptr);
	}

	void DevelopView::Load()
	{
	}

	void DevelopView::Initialize()
	{
	}

	void DevelopView::OnGUI()
	{
		ImGui::Begin("Develop", &mOpenGUI);

		if (ImGui::CollapsingHeader("Statistics") == true)
		{
			Statistics_OnGUI();
		}
		if (ImGui::CollapsingHeader("Devlop Control") == true)
		{
			Dev_OnGUI();
		}
	

		ImGui::End();
		if (mOpenGUI == false)
		{
			mOpenGUI = true;
			Close();
		}
	}

	void DevelopView::Destroy()
	{
	}

	void DevelopView::OnEvent(IEvent& e)
	{
	}

	void DevelopView::Statistics_OnGUI()
	{
		int fps = Application::GetInstance().GetAppTimer()->GetFPS();
		ImGui::Text("FPS : %d", fps);

		ImGui::Text("Draw Total Object : %d", Renderer::Statistics.TotalObjectCount);
		ImGui::Text("Draw Object : %d", Renderer::Statistics.VisibleObjectCount);
		ImGui::Text("Culling Object : %d", Renderer::Statistics.CullingObjectCount);
	}

	void DevelopView::Dev_OnGUI()
	{
		enum 
		{
			Debug_None,
			Debug_Visible_Cluster,
			Debug_Count,
		};

		static int checkIndex = 0;

		ImGui::RadioButton("None", &checkIndex, Debug_None);
		ImGui::RadioButton("Visible Cluster", &checkIndex, Debug_Visible_Cluster);






		switch (checkIndex)
		{
		case Debug_Visible_Cluster:
			Renderer::Debugger.Mode = ERenderDebugMode::Visible_ActiveCluster;
			break;
		default:
			Renderer::Debugger.Mode = ERenderDebugMode::None;
		}


	}

}