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
	buildContext.CacheData         = &_buildCacheData;
	buildContext.CurrentWidget     = nullptr;
	buildContext.bOpenWidget       = true;
	buildContext.bLastCompInLayout = false;
	buildContext.FixedWidth = INDEX_NONE;
	buildContext.FixedHeight = INDEX_NONE;

	for (int32 i = 0; i < (int32)EGUIEvent::Count; ++i)
	{
		buildContext.bIsHandledGUIEvents[i] = false;
	}

	ImVec2 CurrentMousePos = ImGui::GetMousePos();
	if (_buildCacheData.CursorPos.x != CurrentMousePos.x ||
		_buildCacheData.CursorPos.y != CurrentMousePos.y)
	{
		buildContext.bIsDirtyMousePos = true;
	}
	else
	{
		buildContext.bIsDirtyMousePos = false;
	}

	_buildCacheData.CursorPos = HVector2(CurrentMousePos.x, CurrentMousePos.y);

	OnBuild(buildContext);
}

void PImGuiBuild::Reset()
{
	_commandQueues.clear();

	HHashMap<HGuid, HWidgetComponentCacheData> tempWidgetCompCahceDatas = _buildCacheData.WidgetCompCahceDatas;
	for (const HPair<HGuid, HWidgetComponentCacheData>& pair : tempWidgetCompCahceDatas)
	{
		HGuid guid = pair.first;
		if (pair.second.bAttendance == false)
		{
			_buildCacheData.WidgetCompCahceDatas.erase(guid);
		}
		else
		{
			_buildCacheData.WidgetCompCahceDatas[guid].bAttendance = false;
		}
	}
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
	case HGUIBuilder::ECommand::PushGenerateNatvieGUI:
		return OnBuildGenerateNativeGUI(inBuildContext, ON_BUILD_COMMAND_ARG(HGUIBuilder::PGenerateNativeGUICommandValue)));
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

	EWidgetFlags widgetFlags = widget->GetWidgetFlags();

	PString titleName = widget->GetName().ToString();
	titleName += PString("##") + widget->GetGuid().ToString();

	inBuildContext.bOpenWidget = true;
	inBuildContext.PushBuildHistroy(EBuildHistory::Widget);

	if (widgetFlags & EWidgetFlags::ChildWidget)
	{
		bool bBorder = widgetFlags & EWidgetFlags::ChildWidget_Border;
		ImGui::BeginChild(widget->GetGuid().GetHashCode(), ImVec2(0,0), bBorder);
	}
	else 
	{
		ImGui::Begin(titleName.GetCStr(), &inBuildContext.bOpenWidget);
	}

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

	EWidgetFlags widgetFlags = inBuildContext.CurrentWidget->GetWidgetFlags();

	if (widgetFlags & EWidgetFlags::ChildWidget)
	{
		ImGui::EndChild();
	}
	else
	{
		ImGui::End();

		if (inBuildContext.bOpenWidget == false)
		{
			GGUIGlobalSystem::GetInstance().CloseWidget(inBuildContext.CurrentWidget->GetGuid());
			inBuildContext.bOpenWidget = true;
		}
	}

	return true;
}

bool PImGuiBuild::OnBuildWidgetComponent(HBuildContext& inBuildContext, HGUIBuilder::PWidgetComponentCommandValue* inCV)
{
	if (inCV == nullptr || inCV->WidgetComponent == nullptr)
	{
		return false;
	}

	HGuid guid  = inCV->WidgetComponent->GetGuid();
	inBuildContext.CacheData->WidgetCompCahceDatas[guid].bAttendance = true;

	ImGui::PushID((int32)guid.GetHashCode());
	ImGui::BeginGroup();
	IGUIEventReceiver* guiEventReceiver = inCV->WidgetComponent.GetRawPointer();

	HGUIBuilder guiBuilder;
	inCV->WidgetComponent->OnGUIBuild(guiBuilder);
	if (guiBuilder.GetCommandQueue().empty() == false)
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

	OnGUIEvent(inBuildContext, guid, guiEventReceiver);
	ImGui::EndGroup();
	ImGui::PopID();

	if (inBuildContext.bLastCompInLayout == false && inBuildContext.CurrentBuildHistory() == EBuildHistory::Horizontal)
	{
		ImGui::SameLine();
	}

	return true;
}

bool PImGuiBuild::OnBuildGenerateNativeGUI(HBuildContext& inBuildContext, HGUIBuilder::PGenerateNativeGUICommandValue* inCV)
{
	if (inCV == nullptr || inCV->WidgetComponent == nullptr)
	{
		return false;
	}

	HGuid guid = inCV->WidgetComponent->GetGuid();
	inBuildContext.CacheData->WidgetCompCahceDatas[guid].bAttendance = true;

	ImGui::PushID((int32)guid.GetHashCode());
	ImGui::BeginGroup();

	inCV->OnGenerateGUI.ExecuteIfBound(HWidgetContext());

	OnGUIEvent(inBuildContext, guid, inCV->WidgetComponent.GetRawPointer());
	ImGui::EndGroup();
	ImGui::PopID();

	return true;
}

void PImGuiBuild::OnGUIEvent(HBuildContext& inBuildContext, const HGuid& guid, IGUIEventReceiver* inEventReceiver)
{
	if (ImGui::IsItemHovered())
	{
		if (inBuildContext.CacheData->WidgetCompCahceDatas[guid].bHover == false)
		{
			inBuildContext.CacheData->WidgetCompCahceDatas[guid].bHover = true;
			inEventReceiver->OnMouseEnter();
		}

		inEventReceiver->OnMouseHover();

		if (inBuildContext.bIsDirtyMousePos)
		{
			inEventReceiver->OnMouseMove();
		}

		if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			inEventReceiver->OnMouseLButtonDown();
		}
		else if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
		{
			inEventReceiver->OnMouseRButtonDown();
		}
		else if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
		{
			inEventReceiver->OnMouseMButtonDown();
		}

		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			inEventReceiver->OnMouseLButtonUp();
		}
		else if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
		{
			inEventReceiver->OnMouseRButtonUp();
		}
		else if (ImGui::IsMouseReleased(ImGuiMouseButton_Middle))
		{
			inEventReceiver->OnMouseMButtonUp();
		}

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			inEventReceiver->OnMouseLButtonClick();
		}
		else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			inEventReceiver->OnMouseRButtonClick();
		}
		else if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle))
		{
			inEventReceiver->OnMouseMButtonClick();
		}
	}
	else if (inBuildContext.CacheData->WidgetCompCahceDatas[guid].bHover)
	{
		inBuildContext.CacheData->WidgetCompCahceDatas[guid].bHover = false;
		inEventReceiver->OnMouseLeave();
	}
}
