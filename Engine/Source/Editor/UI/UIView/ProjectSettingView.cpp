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



		for (int i = 0; i < 512; ++i)
		{
			auto str = KeyCodeToString((EKeyCode)i);
			if (str != "None")
			{
				mKeyNames.insert(str);
			}
		}
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
		
		static JVector2 btSize = JVector2(18.0f, 18.0f);
		bool isOpen = ImGui::TreeNodeEx("##Action Mappings", ImGuiTreeNodeFlags_FramePadding);
		ImGui::SameLine(); ImGui::Text("Action Mappings"); ImGui::SameLine();
		if (ImGui::ImageButton(0, addIconID, btSize) == true)
		{
			InputManager::GetInstance().AddActionMappings("None");
		}
		if (isOpen)
		{
			List<ActionMappingData*> removeDataList;
			InputManager::GetInstance().ForEach([&](ActionMappingData* data)
			{
				if (ActionMappingData_OnGUI(data) == false)
				{
					removeDataList.push_back(data);
				}
			});
			for (auto& data : removeDataList)
			{
				InputManager::GetInstance().RemoveActionMappings(data);
			}
			// 
			ImGui::TreePop();
		}



		isOpen = ImGui::TreeNodeEx("##Axis Mappings", ImGuiTreeNodeFlags_FramePadding);
		ImGui::SameLine(); ImGui::Text("Axis Mappings"); ImGui::SameLine();
		 
		if (ImGui::ImageButton(0, addIconID, btSize) == true)
		{
			InputManager::GetInstance().AddAxisMappings("None");
		}

		if (isOpen)
		{
			List<AxisMappingData*> removeDataList;
			InputManager::GetInstance().ForEach([&](AxisMappingData* data)
			{
				if (AxisMappingData_OnGUI(data) == false)
				{
					removeDataList.push_back(data);
				}
			});
			for (auto& data : removeDataList)
			{
				InputManager::GetInstance().RemoveAxisMappings(data);
			}
			ImGui::TreePop();
		}


		isOpen = ImGui::TreeNodeEx("Axis Configs", ImGuiTreeNodeFlags_FramePadding);

		if (isOpen)
		{
			ImGui::TreePop();
		}

	}
	bool ProjectSettingView::ActionMappingData_OnGUI(ActionMappingData* data)
	{
		String changeName;
		static JVector2 btSize = JVector2(18.0f, 18.0f);
		auto   addIconID   = GetTextureID(Icon_Add);
		auto   closeIconID = GetTextureID(Icon_Close);
		bool   result = true;

		ImGui::Columns(2, 0, false);
		ImGui::SetColumnWidth(0, 40.0f);
		bool isOpen = ImGui::TreeNodeEx(("##TreeNode_" + std::to_string((u64)data)).c_str(), ImGuiTreeNodeFlags_FramePadding);

		ImGui::NextColumn();
		ImGui::SameLine();
		ImGui::Spacing(); ImGui::SameLine();
		ImGui::Spacing(); ImGui::SameLine();
		ImGui::SetNextItemWidth(250.0f);
		if (ImGui::InputText(data, data->Name, changeName) == true)
		{
			data->Name = changeName;
		}ImGui::SameLine();
		
		// +
		if (ImGui::ImageButton((u64)data, addIconID, btSize) == true)
		{
			data->KeyList.push_back(ActionMappingData::KeyData());
		}
		ImGui::SameLine();
		// -
		if (ImGui::ImageButton((u64)data, closeIconID, btSize) == true)
		{
			result = false;
		}
		if (isOpen)
		{
			auto cnt = data->KeyList.size();
			auto removeIndex = -1;
			for (auto i = 0; i < cnt; ++i)
			{
				ImGui::Spacing(); ImGui::SameLine();
				ImGui::Spacing(); ImGui::SameLine();
				auto keyData = data->KeyList[i];
				auto id = ImGui::GetUniqueID("Combo" + std::to_string(i), (u64)data);
				ImGui::SetNextItemWidth(200.0f);
				if (ImGui::BeginCombo(id.c_str(), KeyCodeToString(keyData.Code).c_str()) == true)
				{
					for (auto& keyName : mKeyNames)
					{
						if (ImGui::Selectable(keyName.c_str(), false) == true)
						{
							data->KeyList[i].Code = StringToKeyCode(keyName);
						}
					}
					ImGui::EndCombo();
				}
				ImGui::SameLine();
				ImGui::Text("Shift"); ImGui::SameLine();
				ImGui::Checkbox(ImGui::GetUniqueID("Check_Shift" + std::to_string(i), (u64)data).c_str(), &data->KeyList[i].IsShift); ImGui::SameLine();
				ImGui::Text("Ctrl"); ImGui::SameLine();
				ImGui::Checkbox(ImGui::GetUniqueID("Check_Ctrl" + std::to_string(i), (u64)data).c_str(), &data->KeyList[i].IsCtrl); ImGui::SameLine();
				ImGui::Text("Alt"); ImGui::SameLine();
				ImGui::Checkbox(ImGui::GetUniqueID("Check_Alt" + std::to_string(i), (u64)data).c_str(), &data->KeyList[i].IsAlt); ImGui::SameLine();



				id = ImGui::GetUniqueID("Key_CloseButton" + std::to_string(i), (u64)data);
				ImGui::PushID(id.c_str());
				if (ImGui::ImageButton(0, closeIconID, btSize) == true)
				{
					removeIndex = i;
				}
				ImGui::PopID();
			}
			if (removeIndex != -1)
			{
				data->KeyList.erase(data->KeyList.begin() + removeIndex);
			}

			ImGui::TreePop();
		}
		ImGui::Columns(1);
		return result;
	}
	bool ProjectSettingView::AxisMappingData_OnGUI(AxisMappingData* data)
	{
		String changeName;
		static JVector2 btSize = JVector2(18.0f, 18.0f);
		auto   addIconID = GetTextureID(Icon_Add);
		auto   closeIconID = GetTextureID(Icon_Close);
		bool   result = true;
		//static bool isOpen = false;

		ImGui::Columns(2, 0, false);
		ImGui::SetColumnWidth(0, 40.0f);
		bool isOpen = ImGui::TreeNodeEx(ImGui::GetUniqueID("TreeNode_", (u64)data).c_str(), ImGuiTreeNodeFlags_FramePadding);

		ImGui::NextColumn();
		ImGui::SameLine();
		ImGui::Spacing(); ImGui::SameLine();
		ImGui::Spacing(); ImGui::SameLine();



		ImGui::SetNextItemWidth(250.0f);
		if (ImGui::InputText(data, data->Name, changeName) == true)
		{
			data->Name = changeName;
		}ImGui::SameLine();

		// +
		if (ImGui::ImageButton((u64)data, addIconID, btSize) == true)
		{
			data->KeyList.push_back(AxisMappingData::KeyData());
		}
		ImGui::SameLine();
		// -
		if (ImGui::ImageButton((u64)data, closeIconID, btSize) == true)
		{
			result = false;
		}
		if (isOpen)
		{
			auto cnt = data->KeyList.size();
			auto removeIndex = -1;
			for (auto i = 0; i < cnt; ++i)
			{
				ImGui::Spacing(); ImGui::SameLine();
				ImGui::Spacing(); ImGui::SameLine();
				auto keyData = data->KeyList[i];
				auto id = ImGui::GetUniqueID("Combo" + std::to_string(i), (u64)data);
				ImGui::SetNextItemWidth(200.0f);
				if (ImGui::BeginCombo(id.c_str(), KeyCodeToString(keyData.Code).c_str()) == true)
				{
					for (auto& keyName : mKeyNames)
					{
						if (ImGui::Selectable(keyName.c_str(), false) == true)
						{
							data->KeyList[i].Code = StringToKeyCode(keyName);
						}
					}
					ImGui::EndCombo();
				}
				ImGui::SameLine();
				ImGui::Text("Scale"); ImGui::SameLine();
				ImGui::SetNextItemWidth(150.0f);
				ImGui::SliderFloat(ImGui::GetUniqueID("SliderFloat_" + std::to_string(i), (u64)data).c_str(), &data->KeyList[i].Scale, -1.0f, 1.0f);
				ImGui::SameLine();


				id = ImGui::GetUniqueID("Key_CloseButton" + std::to_string(i), (u64)data);
				ImGui::PushID(id.c_str());
				if (ImGui::ImageButton(0, closeIconID, btSize) == true)
				{
					removeIndex = i;
				}
				ImGui::PopID();
			}
			if (removeIndex != -1)
			{
				data->KeyList.erase(data->KeyList.begin() + removeIndex);
			}

			ImGui::TreePop();
		}
		ImGui::Columns(1);
		return result;
	}
	void ProjectSettingView::LoadConfig()
	{


		


	}
	void ProjectSettingView::LoadIcons()
	{
		mIcons[Icon_Add] = UIManager::GetInstance().GetIcon("Icon_Add");
		mIcons[Icon_Close] = UIManager::GetInstance().GetIcon("Icon_Close");
	}
	TextureID ProjectSettingView::GetTextureID(i32 iconEnum) const
	{
		if (mIcons[iconEnum] && mIcons[iconEnum]->IsValid() == false)
		{
			return ITexture::NullTexture()->GetTextureID();
		}
		return mIcons[iconEnum]->Get()->GetTextureID();
	}
}