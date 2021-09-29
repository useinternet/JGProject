#include "pch.h"
#include "ProjectSettingView.h"
#include "Class/Asset/Asset.h"
#include "ExternalImpl/JGImGui.h"
#include "Application.h"
namespace JG
{
	ProjectSettingView::ProjectSettingView()
	{
		UIManager::GetInstance().RegisterMainMenuItem("Windows/ProjectSetting", 0, [&]()
		{
			Open();
		}, nullptr);
	}
	void ProjectSettingView::Load()
	{


	}
	void ProjectSettingView::Initialize()
	{
		mCategoryList[Category_Start_Setting] = Category("Start Settings");
		mCategoryList[Category_Input] = Category("Input");

		LoadIcons();
	}
	void ProjectSettingView::OnGUI()
	{

		ImGui::Begin("ProjectSetting", &mOpenGUI);
		

		f32 winWidth = ImGui::GetWindowWidth();
		f32 categoryWidth = std::min<f32>(200.0f, winWidth * 0.2f);
		if (ImGui::BeginTable("WorldHierarchy_Table", 2, ImGuiTableFlags_BordersInnerV) == true)
		{
			ImGui::TableSetupColumn("##Category", ImGuiTableColumnFlags_WidthFixed, categoryWidth);
			ImGui::TableSetupColumn("##Contents", 0, 0);

			ImGui::TableNextColumn();
			Category_OnGUI();
			ImGui::TableNextColumn();
			Contents_OnGUI();
			ImGui::EndTable();
		}







		ImGui::End();
		if (mOpenGUI == false)
		{
			mOpenGUI = true;
			Close();
		}

	}
	void ProjectSettingView::Destroy()
	{

	}
	void ProjectSettingView::OnEvent(IEvent& e)
	{
	}
	void ProjectSettingView::Category_OnGUI()
	{
		f32 winWidth = ImGui::GetWindowWidth();
		f32 categoryWidth = std::min<f32>(200.0f, winWidth * 0.2f);
		ImGui::BeginChild("ProjectSetting_Category", ImVec2(0, categoryWidth));
	
		for (i32 i = 0; i < Category_Count; ++i)
		{
			
			if (ImGui::Selectable(mCategoryList[i].Label.c_str(), &(mCategoryList[i].IsSelected)))
			{
				for (i32 j = 0; j < Category_Count; ++j)
				{
					if (i == j) continue;
					mCategoryList[j].IsSelected = false;
				}
				mCurrentCategory = i;
			}
		}


		ImGui::EndChild();
	}
	void ProjectSettingView::Contents_OnGUI()
	{
		if (mCurrentCategory == -1)
		{
			return;
		}
		ImGui::BeginChild("ProjectSetting_Contents", ImVec2(0, 0));

		auto& category = mCategoryList[mCurrentCategory];


		ImGui::CollapsingHeader(category.Label.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

		switch (mCurrentCategory)
		{
		case Category_Start_Setting: StartSetting_OnGUI(); break;
		case Category_Input: Input_OnGUI(); break;
		}

		ImGui::EndChild();
	}
	void ProjectSettingView::StartSetting_OnGUI()
	{

		ImGui::Text("Start Project"); 
		ImGui::Separator();
		ImGui::AlignTextToFramePadding();
		char str[128] = { 0, };
		ImGui::Text("Start GameWorld : "); ImGui::SameLine(); 
		ImGui::InputText("##...", str, 128); ImGui::SameLine();
		ImGui::Button("...");


	}
	void ProjectSettingView::Input_OnGUI()
	{

		ImGui::Text("Input Mappings");
		ImGui::Separator();

		auto addIconID = GetTextureID(Icon_Add);
		
		static ImVec2 btSize = ImVec2(18.0f, 18.0f);
		bool isOpen = ImGui::TreeNodeEx("##Action Mappings", ImGuiTreeNodeFlags_FramePadding);
		ImGui::SameLine(); ImGui::Text("Action Mappings"); ImGui::SameLine();
		if (ImGui::ImageButton(addIconID, btSize) == true)
		{

		}
		if (isOpen)
		{
			ImGui::TreePop();
		}



		isOpen = ImGui::TreeNodeEx("##Axis Mappings", ImGuiTreeNodeFlags_FramePadding);
		ImGui::SameLine(); ImGui::Text("Axis Mappings"); ImGui::SameLine();
		 
		if (ImGui::ImageButton(addIconID, btSize) == true)
		{

		}

		if (isOpen)
		{
			ImGui::TreePop();
		}


		isOpen = ImGui::TreeNodeEx("Axis Configs", ImGuiTreeNodeFlags_FramePadding);

		if (isOpen)
		{
			ImGui::TreePop();
		}

	}
	void ProjectSettingView::LoadConfig()
	{
		auto configPath = CombinePath(Application::GetConfigPath(), "ProjectSettings.json");
		mProjectSettingsJson = CreateSharedPtr<Json>();


		//if (fs::exists(configPath) == false)
		//{
		//	for (auto& category : mCategoryList)
		//	{
		//		category.JsonData = configPath.
		//	}


		//}


		


	}
	void ProjectSettingView::LoadIcons()
	{
		mIcons[Icon_Add] = UIManager::GetInstance().GetIcon("Icon_Add");
		mIcons[Icon_Close] = UIManager::GetInstance().GetIcon("Icon_Close");
	}
	ImTextureID ProjectSettingView::GetTextureID(i32 iconEnum) const
	{
		if (mIcons[iconEnum] && mIcons[iconEnum]->IsValid() == false)
		{
			return (ImTextureID)JGImGui::GetInstance().ConvertImGuiTextureID(ITexture::NullTexture()->GetTextureID());
		}
		return (ImTextureID)JGImGui::GetInstance().ConvertImGuiTextureID(mIcons[iconEnum]->Get()->GetTextureID());
	}
}