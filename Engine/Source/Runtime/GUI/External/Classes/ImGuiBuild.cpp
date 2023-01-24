#include "PCH/PCH.h"
#include "ImGuiBuild.h"
#include "Widget.h"
#include "WidgetComponents/WidgetComponent.h"
#include "GUI.h"
#include "External/imgui/imgui.h"

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
		// 오른쪽으로 이동
		// 왼쪽으로 이동
		break;
	case HGUIBuilder::ECommand::BeginVertical:
		// 
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
		OnBuildWidgetComponent(RawFastCast<HGUIBuilder::PWidgetComponentCommandValue>(inCommandData.CommandValue.GetRawPointer()));
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

	_bOpenWidget = true;
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

void PImGuiBuild::OnBuildWidgetComponent(HGUIBuilder::PWidgetComponentCommandValue* inCV)
{
	if (inCV == nullptr || inCV->WidgetComponent == nullptr)
	{
		return;
	}
	HGuid guid = inCV->WidgetComponent->GetGuid();
	ImGui::PushID((int32)guid.GetHashCode());

	IImGuiWidgetComponentGenerator* imGuiWidgetGenerater = inCV->WidgetComponent.GetRawPointer();
	imGuiWidgetGenerater->GenerateImGuiWidgetComponent(HWidgetContext());

	ImGui::PopID();
}
