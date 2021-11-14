#include "pch.h"
#include "ProjectSettingView.h"
#include "UI/UIManager.h"
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



		auto windowSize = ImGui::GetWindowSize();
		auto padding    = ImGui::GetStyle().FramePadding;
		auto buttonSize = ImVec2(100, 40);
		ImGui::SetCursorPos(ImVec2(windowSize.x - buttonSize.x - padding.x, windowSize.y - buttonSize.y - padding.y));
		if (ImGui::Button("Save", buttonSize) == true)
		{
			ProjectSetting::GetInstance().Save();
		}
		ImGui::EndChild();
	}
	void ProjectSettingView::StartSetting_OnGUI()
	{
		String startGameWorld = ProjectSetting::GetInstance().GetStartGameWorldPath();




		ImGui::Text("Start Project"); 
		ImGui::Separator();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Start GameWorld : "); ImGui::SameLine();




		char temp[256] = { 0, };
		strcpy(temp, startGameWorld.c_str());
		ImGui::InputText("##...", temp, 256);





		ProjectSetting::GetInstance().SetStartGameWorldPath(temp);
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