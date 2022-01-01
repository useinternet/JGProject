#include "pch.h"
#include "DevelopView.h"
#include "Application.h"
#include "UI/UIManager.h"

#include "Components/Camera.h"


#include "Graphics/Renderer.h"
#include "Graphics/PostRenderProcess/PostProcess_ToneMapping.h"
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

		if (ImGui::CollapsingHeader("Statistics", ImGuiTreeNodeFlags_DefaultOpen) == true)
		{
			Statistics_OnGUI();
		}
		if (ImGui::CollapsingHeader("Devlop Control", ImGuiTreeNodeFlags_DefaultOpen) == true)
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
		Camera* mainCamera = Camera::GetMainCamera();
		Graphics::Scene* scene = nullptr;


		if (mainCamera)
		{
			scene = mainCamera->GetScene();
		}


		{
			static int checkIndex = 0;
			ImGui::Text("[ Render Process Debug ]");
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
		ImGui::Spacing(); ImGui::Spacing();
		ImGui::Separator();





		{
			//f32  exposure = 0.0f;
			//bool isEnable = true;
			//if (scene)
			//{
			//	scene->GetProcessShaderParam<PostProcess_ToneMapping, bool>("Enable", &isEnable);
			//}

			//ImGui::Text("[ ToneMapping ]");

			//isEnable = ImGui::CheckBox("Enable", isEnable);
			//if(scene)
			//{
			//	scene->SetProcessShaderParam<PostProcess_ToneMapping, bool>("Enable", isEnable);
			//}
			//ImGui::SetNextItemWidth(150.0f);
			//if (ImGui::InputFloat("Exposure", &exposure) == true && scene)
			//{
			//	scene->SetProcessShaderParam<PostProcess_ToneMapping, f32>("Exposure", exposure);
			//}
		}


		ImGui::Spacing(); ImGui::Spacing();
		ImGui::Separator();



	}

}