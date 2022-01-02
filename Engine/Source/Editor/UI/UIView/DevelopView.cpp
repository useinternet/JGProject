#include "pch.h"
#include "DevelopView.h"
#include "Application.h"
#include "UI/UIManager.h"

#include "Components/Camera.h"


#include "Graphics/Renderer.h"
#include "Graphics/PostRenderProcess/PostProcess_Bloom.h"
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

		if (scene == nullptr || scene->GetRenderer() == nullptr)
		{
			return;
		}
		{
			RP_Global_Float exposureVal      = RP_Global_Float::Load("Renderer/Exposure", scene->GetRenderer());
			RP_Global_Float paperWhiteVal    = RP_Global_Float::Load("Renderer/HDRPaperWhite", scene->GetRenderer());
			RP_Global_Float maxDisplayLumVal = RP_Global_Float::Load("Renderer/MaxDisplayLuminance", scene->GetRenderer());


			f32 exposure      = exposureVal.GetValue();
			f32 paperWhite    = paperWhiteVal.GetValue();
			f32 maxDisplayLum = maxDisplayLumVal.GetValue();

			ImGui::Text("[Renderer Option]");
			ImGui::SliderFloat("Exposure", &exposure, -8.0f, 8.0f, "%.5f", ImGuiSliderFlags_NoRoundToFormat);
			ImGui::SliderFloat("HDRPaperWhite", &paperWhite, 100.0f, 500.0f, "%.5f", ImGuiSliderFlags_NoRoundToFormat);
			ImGui::SliderFloat("MaxDisplayLuminance", &maxDisplayLum, 500.0f, 10000.0f, "%.5f", ImGuiSliderFlags_NoRoundToFormat);

			exposureVal.SetValue(exposure);
			paperWhiteVal.SetValue(paperWhite);
			maxDisplayLumVal.SetValue(maxDisplayLum);

		}
		ImGui::Spacing(); ImGui::Spacing();
		ImGui::Separator();

		{
			/*
		RP_Local_Float mBloomThreshold;    // 4.0f, 0.0f, 8.0f
		RP_Local_Float mUpSamplingFactor;  // 0.65, 0.0f, 1.0f
		RP_Local_Float mBloomStrength;     // 0.1f, 0.0f, 2.0f, 0.05f);
			*/
			RP_Local_Float bloomThresholdVal   = RP_Local_Float::Load(JGTYPE(PostProcess_Bloom), "BloomThreshold", scene->GetRenderer());
			RP_Local_Float upSamplingFactorVal = RP_Local_Float::Load(JGTYPE(PostProcess_Bloom), "UpSamplingFactor", scene->GetRenderer());
			RP_Local_Float bloomStrengthVal       = RP_Local_Float::Load(JGTYPE(PostProcess_Bloom), "BloomStrength", scene->GetRenderer());

			f32 bloomThreshold = bloomThresholdVal.GetValue();
			f32 upSamplingFactor = upSamplingFactorVal.GetValue();
			f32 bloomStrength = bloomStrengthVal.GetValue();

			ImGui::Text("[Bloom Option]");
			ImGui::SliderFloat("BloomThreshold", &bloomThreshold, 0.0f, 8.0f, "%.5f", ImGuiSliderFlags_NoRoundToFormat);
			ImGui::SliderFloat("BloomStrength", &bloomStrength, 0.0f, 2.0f, "%.5f", ImGuiSliderFlags_NoRoundToFormat);
			ImGui::SliderFloat("UpSamplingFactor", &upSamplingFactor, 0.0f, 1.0f, "%.5f", ImGuiSliderFlags_NoRoundToFormat);

			bloomThresholdVal.SetValue(bloomThreshold);
			bloomStrengthVal.SetValue(bloomStrength);
			upSamplingFactorVal.SetValue(upSamplingFactor);
		}


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