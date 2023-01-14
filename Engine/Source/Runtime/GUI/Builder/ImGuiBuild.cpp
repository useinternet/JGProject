#include "PCH/PCH.h"
#include "ImGuiBuild.h"
#include "imgui/imgui.h"

void PImGuiBuild::OnBuild(const HGUIBuilder::HCommandData& inCommandData)
{
	switch (inCommandData.Command)
	{
	case HGUIBuilder::ECommand::BeginHorizontal:
		break;
	case HGUIBuilder::ECommand::BeginVertical:
		break;
	case HGUIBuilder::ECommand::EndHorizontal:
		break;
	case HGUIBuilder::ECommand::EndVertical:
		break;
	case HGUIBuilder::ECommand::PushWidget:
		break;
	case HGUIBuilder::ECommand::Text:
		OnBuildText(RawFastCast<HGUIBuilder::PTextCommandValue>(inCommandData.CommandValue.GetRawPointer()));
		break;
	case HGUIBuilder::ECommand::Button:
		break;
	}
}

void PImGuiBuild::OnBuildText(HGUIBuilder::PTextCommandValue* inCV)
{
	if (inCV == nullptr)
	{
		return;
	}

	ImGui::Text(inCV->Text.GetCStr());
}
