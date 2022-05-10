#include "pch.h"
#include "AnimationView.h"
#include "Application.h"
#include "ExternalImpl/JGImGui.h"
#include "ExternalImpl/TextEditor.h"
#include "Class/Asset/Asset.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/GraphicsHelper.h"
#include "UI/UIManager.h"
#include "UI/EditorUIScene.h"
#include "UI/NodeGUI.h"
#include "UI/UIManager.h";

namespace JG
{
	AnimationView::AnimationView()
	{
		DisableUniqueView();

	}

	void AnimationView::Load()
	{
		
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Create/AnimationClip", 0, [&]()
		{
			CreateAnimationClipNode();
			JG_LOG_INFO("Create Node");
		}, nullptr);
	}

	void AnimationView::Initialize()
	{
		mNodeEditor = CreateUniquePtr<StateNodeGUI::StateNodeEditor>();
		mNodeEditor->BindContextMenuFunc([&]()
		{
			if (UIManager::GetInstance().ShowContextMenu(GetType(), false) == true)
			{

			}
		});
		CreateRootNode();
	}

	void AnimationView::OnGUI()
	{
		ImVec2 winSize = ImGui::GetWindowSize();
		ImGui::Columns(3);
		// 애니메이션 뷰
		ImGui::SetColumnWidth(0, 350.0f);
		ImGui::BeginChild("AnimSceneView");
		ImGui::Text("Animation Asset Management && Scene View");
		ImGui::EndChild();
		ImGui::NextColumn();
		ImGui::SetColumnWidth(1, Math::Max<f32>(winSize.x - 700, 1));

		// Animation Play && Debugging
		ImGui::BeginChild("aosijasdf", ImVec2(0.0f, 30.0f));
		ImGui::Button("Play"); ImGui::SameLine();
		ImGui::Text("Debbuging && Anim Play");
	
		ImGui::Separator();
		ImGui::EndChild();
		mNodeEditor->OnGUI();
		// 
		ImGui::NextColumn();


		// Param Controller 
		ImGui::SetColumnWidth(2, 350.0f);
		ImGui::BeginChild("ParamEditor", ImVec2(350.0f, 0.0f));

		List<AnimParamData> data;
		data.resize(6);

		data[0] = { "Test1", EAnimationParameterType::Bool };
		data[1] = { "Test2", EAnimationParameterType::Float };
		data[2] = { "Test4", EAnimationParameterType::Int };
		data[3] = { "Test5", EAnimationParameterType::Float };
		data[4] = { "Test6", EAnimationParameterType::Int };
		data[5] = { "Test8", EAnimationParameterType::Int };


		ImGui::Button("Default"); ImGui::SameLine(); ImGui::Button("Edit"); ImGui::SameLine(); ImGui::Button("Delete");
		ImGui::Separator();

		ImGui::BeginTable("ParamEditorTable", 4, ImGuiTableFlags_None);
		ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 80.0f);
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 120.0F);
		ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, 50.0f);
		ImGui::TableSetupColumn("##Interaction", ImGuiTableColumnFlags_WidthStretch, 100);
		ImGui::TableHeadersRow();

		for (i32 i = 0; i < 6; ++i)
		{
			ImGui::TableNextColumn();
			String type = AnimationParameterTypeToString(data[i].Type);

			ImGui::SetNextItemWidth(80.0f);
			if (ImGui::BeginCombo(("##ComboBox" + std::to_string(i)).c_str(), type.c_str()))
			{
				for (i32 paramType = 0; paramType < (i32)EAnimationParameterType::Count; ++paramType)
				{
					String typeName = AnimationParameterTypeToString((EAnimationParameterType)paramType);
					if (ImGui::Selectable(typeName.c_str()) == true)
					{
						data[i].Type = (EAnimationParameterType)paramType;
					}
				}
				ImGui::EndCombo();
			}

			ImGui::TableNextColumn();
			String str = data[i].Name;
			str.resize(512);
			ImGui::SetNextItemWidth(120.0F);
			ImGui::InputText(("##InputText" + std::to_string(i)).c_str(), str.data(), 512);
			if (ImGui::IsItemDeactivated())
			{
				data[i].Name = str;
			}

			ImGui::TableNextColumn();

			ImGui::Text("0.0");

			ImGui::TableNextColumn();


			ImGui::ArrowButton(("##UpButton" + std::to_string(i)).c_str(), ImGuiDir_Up); ImGui::SameLine();
			ImGui::ArrowButton(("##DownButton" + std::to_string(i)).c_str(), ImGuiDir_Down); ImGui::SameLine();

		}
		ImGui::EndTable();

		auto padding = ImGui::GetStyle().FramePadding;

		ImGui::Button("+", ImVec2(350.0f - (padding.x * 4), 0.0f));


		ImGui::EndChild();
		ImGui::Columns(1);

	}


	void AnimationView::Destroy()
	{
	}

	void AnimationView::OnEvent(IEvent& e)
	{
	}

	void AnimationView::CreateRootNode()
	{
		StateNodeGUI::StateNodeBuilder nodeBuilder;
		nodeBuilder.SetInitLocation(JVector2(250, 100));
		nodeBuilder.SetName("Root");
		nodeBuilder.SetNodeFlags(
			StateNodeGUI::EStateNodeFlags::No_Remove | 
			StateNodeGUI::EStateNodeFlags::No_ReName | 
			StateNodeGUI::EStateNodeFlags::RootNode);
		mNodeEditor->CreateNode(nodeBuilder);
	}

	void AnimationView::CreateAnimationClipNode()
	{
		JVector2 offset = mNodeEditor->GetOffset();
		ImVec2 mousePos = ImGui::GetMousePos();
		JVector2 winPos = mNodeEditor->GetWindowPos();
		StateNodeGUI::StateNodeBuilder nodeBuilder;
		nodeBuilder.SetInitLocation(JVector2(mousePos.x - winPos.x, mousePos.y - winPos.y) - offset);
		nodeBuilder.SetName("AnimationClip");
		mNodeEditor->CreateNode(nodeBuilder);
	}

	void AnimationView::UpdateScene()
	{

	}

}