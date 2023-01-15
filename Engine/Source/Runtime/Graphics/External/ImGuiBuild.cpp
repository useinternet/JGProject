#include "PCH/PCH.h"
#include "ImGuiBuild.h"
#include "Widget.h"
#include "imgui/imgui.h"



void PImGuiBuild::PushData(const HQueue<HGUIBuilder::HCommandData>& inCommandQueue)
{
	_commandQueues.push_back(inCommandQueue);
}

void PImGuiBuild::Build()
{
	for (HQueue<HGUIBuilder::HCommandData>& commandQueue : _commandQueues)
	{
		while (commandQueue.empty() == false)
		{
			OnBuild(commandQueue.front());
			commandQueue.pop();
		}
	}
}

void PImGuiBuild::Reset()
{
	_commandQueues.clear();
}


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
	case HGUIBuilder::ECommand::BeginWidget:
		OnBuildBeginWidget(RawFastCast<HGUIBuilder::PWidgetCommandValue>(inCommandData.CommandValue.GetRawPointer()));
		break;
	case HGUIBuilder::ECommand::EndWidget:
		OnBuildEndWidget();
		break;
	case HGUIBuilder::ECommand::PushWidgetComponent:
		break;
	case HGUIBuilder::ECommand::Text:
		OnBuildText(RawFastCast<HGUIBuilder::PTextCommandValue>(inCommandData.CommandValue.GetRawPointer()));
		break;
	case HGUIBuilder::ECommand::Button:
		break;
	}
}
bool test = true;
void PImGuiBuild::OnBuildBeginWidget(HGUIBuilder::PWidgetCommandValue* inCV)
{
	if (inCV == nullptr)
	{
		return;
	}

	PSharedPtr<WWidget> widget = inCV->Widget;
	if (widget == nullptr)
	{
		return;
	}

	PString titleName = widget->GetName().ToString();
	titleName += "##" + widget->GetGuid().to
	
}

void PImGuiBuild::OnBuildEndWidget()
{
	ImGui::End();
}

void PImGuiBuild::OnBuildText(HGUIBuilder::PTextCommandValue* inCV)
{
	if (inCV == nullptr)
	{
		return;
	}

	ImGui::Text(inCV->Text.GetCStr());
}
