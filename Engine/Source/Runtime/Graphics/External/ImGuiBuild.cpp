#include "PCH/PCH.h"
#include "ImGuiBuild.h"
#include "Widget.h"
#include "GUI.h"
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
	_currentWidget = nullptr;
	_bOpenWidget = true;
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
	case HGUIBuilder::ECommand::Separator:
		ImGui::Separator();
		break;
	case HGUIBuilder::ECommand::Text:
		OnBuildText(RawFastCast<HGUIBuilder::PTextCommandValue>(inCommandData.CommandValue.GetRawPointer()));
		break;
	case HGUIBuilder::ECommand::Button:
		break;
	}
}

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
	titleName += PString("##") + widget->GetGuid().ToString();

	ImGui::Begin(titleName.GetCStr(), &_bOpenWidget);

	_currentWidget = widget;
}

void PImGuiBuild::OnBuildEndWidget()
{
	ImGui::End();
	if (_bOpenWidget == false)
	{
		GGUIGlobalSystem::GetInstance().CloseWidget(_currentWidget->GetGuid());
		_bOpenWidget = true;
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

void PImGuiBuild::OnBuildButton(HGUIBuilder::PButtonCommandValue* inCV)
{
	if (inCV == nullptr)
	{
		return;
	}

	ImGui::Button(inCV->Label.GetCStr(), ImVec2(inCV->Size.x, inCV->Size.y));
}
