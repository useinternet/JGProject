#include "PCH/PCH.h"
#include "ImGuiBuild.h"
#include "Widget.h"
#include "WidgetComponents/WidgetComponent.h"
#include "GUI.h"
#include "External/imgui/imgui.h"
#include "External/imgui/imgui_internal.h"

#define ON_BUILD_COMMAND_ARG(CommandName) RawFastCast<##CommandName>(inCommandData.CommandValue.GetRawPointer()

void PImGuiBuild::PushData(const HQueue<HGUIBuilder::HCommandData>& inCommandQueue)
{
	_commandQueues.push_back(inCommandQueue);
}

void PImGuiBuild::Build()
{
	HBuildContext buildContext;

	buildContext.CurrentWidget     = nullptr;
	buildContext.bOpenWidget       = true;
	buildContext.bLastCompInLayout = false;
	buildContext.FixedWidth = INDEX_NONE;
	buildContext.FixedHeight = INDEX_NONE;

	OnBuild(buildContext);
}

void PImGuiBuild::Reset()
{
	_commandQueues.clear();
}

void PImGuiBuild::OnBuild(HBuildContext& inBuildContext)
{
	for (HQueue<HGUIBuilder::HCommandData>& commandQueue : _commandQueues)
	{
		while (commandQueue.empty() == false)
		{
			HGUIBuilder::HCommandData commandData = commandQueue.front(); commandQueue.pop();
			if (commandQueue.empty() == false &&
			   (commandQueue.front().Command == HGUIBuilder::ECommand::EndHorizontal ||
				commandQueue.front().Command == HGUIBuilder::ECommand::EndVertical))
			{
				inBuildContext.bLastCompInLayout = true;
			}
			else
			{
				inBuildContext.bLastCompInLayout = false;
			}

			OnBuild(inBuildContext, commandData);
		}
	}
}

bool PImGuiBuild::OnBuild(HBuildContext& inBuildContext, const HGUIBuilder::HCommandData& inCommandData)
{
	switch (inCommandData.Command)
	{
	case HGUIBuilder::ECommand::BeginHorizontal:
		return OnBuildBeginHorizontal(inBuildContext, ON_BUILD_COMMAND_ARG(HGUIBuilder::PHorizontalCommandValue)));
	case HGUIBuilder::ECommand::BeginVertical:
		return OnBuildBeginVertical(inBuildContext, ON_BUILD_COMMAND_ARG(HGUIBuilder::PVerticalCommandValue)));
	case HGUIBuilder::ECommand::EndHorizontal:
		return OnBuildEndHorizontal(inBuildContext);
	case HGUIBuilder::ECommand::EndVertical:
		return OnBuildEndVertical(inBuildContext);
	case HGUIBuilder::ECommand::BeginWidget:
		return OnBuildBeginWidget(inBuildContext, ON_BUILD_COMMAND_ARG(HGUIBuilder::PWidgetCommandValue)));
	case HGUIBuilder::ECommand::EndWidget:
		return OnBuildEndWidget(inBuildContext);
	case HGUIBuilder::ECommand::PushWidgetComponent:
		return OnBuildWidgetComponent(inBuildContext, ON_BUILD_COMMAND_ARG(HGUIBuilder::PWidgetComponentCommandValue)));
	}

	return false;
}

bool PImGuiBuild::OnBuildBeginHorizontal(HBuildContext& inBuildContext, HGUIBuilder::PHorizontalCommandValue* inCV)
{
	inBuildContext.PushBuildHistroy(EBuildHistory::Horizontal);
	ImGui::BeginGroup();
	return true;
}

bool PImGuiBuild::OnBuildEndHorizontal(HBuildContext& inBuildContext)
{
	if (inBuildContext.CurrentBuildHistory() != EBuildHistory::Horizontal)
	{
		JG_LOG(GUI, ELogLevel::Error, "Fail GUI Build: Mismatch Begin/End Horizontal");
		return false;
	}

	inBuildContext.PopBuildHistory();
	ImGui::EndGroup();
	return true;
}

bool PImGuiBuild::OnBuildBeginVertical(HBuildContext& inBuildContext, HGUIBuilder::PVerticalCommandValue* inCV)
{
	inBuildContext.PushBuildHistroy(EBuildHistory::Vertical);
	inBuildContext.FixedHeight = inCV->FixedHeight;
	ImGui::BeginGroup();
	return true;
}

bool PImGuiBuild::OnBuildEndVertical(HBuildContext& inBuildContext)
{
	if (inBuildContext.CurrentBuildHistory() != EBuildHistory::Vertical)
	{
		JG_LOG(GUI, ELogLevel::Error, "Fail GUI Build: Mismatch Begin/End Vertical");
		return false;
	}

	inBuildContext.PopBuildHistory();
	ImGui::EndGroup();

	ImGuiWindow* window = ImGuiContext->CurrentWindow;
	if (window != nullptr)
	{
		if (inBuildContext.CurrentBuildHistory() == EBuildHistory::Horizontal)
		{
			ImGui::SameLine();
		}
	}
	return true;
}

bool PImGuiBuild::OnBuildBeginWidget(HBuildContext& inBuildContext, HGUIBuilder::PWidgetCommandValue* inCV)
{
	if (inCV == nullptr)
	{
		return false;
	}

	PSharedPtr<WWidget> widget = inCV->Widget;
	if (widget == nullptr)
	{
		JG_LOG(GUI, ELogLevel::Error, "Fail GUI Build: Widget is null");
		return false;
	}

	PString titleName = widget->GetName().ToString();
	titleName += PString("##") + widget->GetGuid().ToString();

	inBuildContext.bOpenWidget = true;
	inBuildContext.PushBuildHistroy(EBuildHistory::Widget);
	ImGui::Begin(titleName.GetCStr(), &inBuildContext.bOpenWidget);

	inBuildContext.CurrentWidget = widget;
	return true;
}

bool PImGuiBuild::OnBuildEndWidget(HBuildContext& inBuildContext)
{
	if (inBuildContext.CurrentBuildHistory() != EBuildHistory::Widget)
	{
		JG_LOG(GUI, ELogLevel::Error, "Fail GUI Build: Mismatch Begin/End Widget");
		return false;
	}

	inBuildContext.PopBuildHistory();

	ImGui::End();
	
	if (inBuildContext.bOpenWidget == false)
	{
		GGUIGlobalSystem::GetInstance().CloseWidget(inBuildContext.CurrentWidget->GetGuid());
		inBuildContext.bOpenWidget = true;
	}

	return true;
}

bool PImGuiBuild::OnBuildWidgetComponent(HBuildContext& inBuildContext, HGUIBuilder::PWidgetComponentCommandValue* inCV)
{
	if (inCV == nullptr || inCV->WidgetComponent == nullptr)
	{
		return false;
	}

	HGuid guid = inCV->WidgetComponent->GetGuid();
	ImGui::PushID((int32)guid.GetHashCode());
	ImGui::BeginGroup();
	IImGuiWidgetComponentGenerator* imGuiWidgetGenerater = inCV->WidgetComponent.GetRawPointer();
	IGUIEventReceiver* guiEventReceiver = inCV->WidgetComponent.GetRawPointer();

	HGUIBuilder guiBuilder;
	imGuiWidgetGenerater->OnGUIBuild(guiBuilder);
	if (guiBuilder.GetCommandQueue().empty())
	{
		imGuiWidgetGenerater->GenerateImGuiWidgetComponent(HWidgetContext());
	}
	else
	{
		PImGuiBuild imGuiBuild;
		imGuiBuild.ImGuiContext = ImGuiContext;
		
		imGuiBuild.PushData(guiBuilder.GetCommandQueue());
		imGuiBuild.OnBuild(inBuildContext);
	}

	//ImGui::OpenPopupEx(0, ImGuiPopupFlags_MouseButtonRight);
	//if (ImGui::BeginPopupContextWindow() == true)
	//{
	//	ImGui::EndPopup();
	//}

	if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
	{
		guiEventReceiver->OnMouseLButtonDown();
	}
	else if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
	{
		guiEventReceiver->OnMouseRButtonDown();
	}
	else if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
	{
		guiEventReceiver->OnMouseMButtonDown();
	}
	else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
	{
		guiEventReceiver->OnMouseLButtonDown();
	}
	else if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
	{
		guiEventReceiver->OnMouseRButtonDown();
	}
	else if (ImGui::IsMouseReleased(ImGuiMouseButton_Middle))
	{
		guiEventReceiver->OnMouseMButtonDown();
	}
	else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		guiEventReceiver->OnMouseLButtonClick();
	}
	else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
	{
		guiEventReceiver->OnMouseRButtonClick();
	}
	else if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle))
	{
		guiEventReceiver->OnMouseMButtonClick();
	}
	else if (ImGui::IsItemHovered())
	{
		guiEventReceiver->OnMouseHover();
		// mouse pos 캐싱해두었다가 조금이라도 움직이면 onmousemove
		// hover가 false가되면 leave
	}

	ImGui::PopID();

	if (inBuildContext.bLastCompInLayout == false && inBuildContext.CurrentBuildHistory() == EBuildHistory::Horizontal)
	{
		ImGui::SameLine();
	}

	return true;
}
