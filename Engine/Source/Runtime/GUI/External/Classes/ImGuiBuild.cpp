#include "PCH/PCH.h"
#include "ImGuiBuild.h"
#include "WidgetContext.h"
#include "Widget.h"
#include "WidgetComponents/WidgetComponent.h"
#include "ImGuiContextMenuBuild.h"
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
	//buildContext.CurrentWidget     = nullptr;
	//buildContext.bOpenRootWidget   = true;
	buildContext.bOpenContextMenu  = false;
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

	HHashMap<HGuid, HWidgetCacheData> tempWidgetCahceDatas = _buildCacheData.WidgetCacheDatas;
	for (const HPair<HGuid, HWidgetCacheData>& pair : tempWidgetCahceDatas)
	{
		HGuid guid = pair.first;
		if (pair.second.bAttendance == false)
		{
			_buildCacheData.WidgetCacheDatas.erase(guid);
		}
		else
		{
			_buildCacheData.WidgetCacheDatas[guid].bAttendance = false;
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
	case HGUIBuilder::ECommand::PushChildWidget:
		return OnBuildPushChildWidget(inBuildContext, ON_BUILD_COMMAND_ARG(HGUIBuilder::PChildWidgetCommandValue)));
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
	inBuildContext.PushBuildHistroy(HBuildHistory(EBuildHistory::Horizontal, nullptr));
	ImGui::BeginGroup();

	return true;
}

bool PImGuiBuild::OnBuildEndHorizontal(HBuildContext& inBuildContext)
{
	if (inBuildContext.CurrentBuildHistory().BuildHistroy != EBuildHistory::Horizontal)
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
	inBuildContext.PushBuildHistroy(HBuildHistory(EBuildHistory::Vertical, nullptr));
	inBuildContext.FixedHeight = inCV->FixedHeight;
	ImGui::BeginGroup();
	return true;
}

bool PImGuiBuild::OnBuildEndVertical(HBuildContext& inBuildContext)
{
	if (inBuildContext.CurrentBuildHistory().BuildHistroy != EBuildHistory::Vertical)
	{
		JG_LOG(GUI, ELogLevel::Error, "Fail GUI Build: Mismatch Begin/End Vertical");
		return false;
	}

	inBuildContext.PopBuildHistory();
	ImGui::EndGroup();

	ImGuiWindow* window = ImGuiContext->CurrentWindow;
	if (window != nullptr)
	{
		if (inBuildContext.CurrentBuildHistory().BuildHistroy == EBuildHistory::Horizontal)
		{
			ImGui::SameLine();
		}
	}
	return true;
}

bool PImGuiBuild::OnBuildPushChildWidget(HBuildContext& inBuildContext, HGUIBuilder::PChildWidgetCommandValue* inCV)
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

	PSharedPtr<PImGuiBuild> guiBuild = Allocate<PImGuiBuild>();
	HGUIBuilder guiBuilder;

	OnBuildBeginChildWidget(inBuildContext, inCV);

	widget->OnGUIBuild(guiBuilder);

	guiBuild->ImGuiContext = ImGuiContext;
	guiBuild->PushData(guiBuilder.GetCommandQueue());
	guiBuild->OnBuild(inBuildContext);

	OnBuildEndChildWidget(inBuildContext);

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

	inBuildContext.PushBuildHistroy(HBuildHistory(EBuildHistory::Widget, widget));

	HWidgetCacheData& widgetCachaData = inBuildContext.CacheData->WidgetCacheDatas[widget->GetGuid()];

	widgetCachaData.bAttendance = true;
	widgetCachaData.bOpenedWidget = true;

	ImGui::Begin(titleName.GetCStr(), &(inBuildContext.CacheData->WidgetCacheDatas[widget->GetGuid()].bOpenedWidget));

	ImVec2 WindowContentRegionMin = ImGui::GetWindowContentRegionMin();
	ImVec2 WindowContentRegionMax = ImGui::GetWindowContentRegionMax();

	widgetCachaData.ContentRegionSize = HVector2(WindowContentRegionMax.x, WindowContentRegionMax.y) - HVector2(WindowContentRegionMin.x, WindowContentRegionMin.y);

	inBuildContext.ParentWidget = widget;
	return true;
}

bool PImGuiBuild::OnBuildEndWidget(HBuildContext& inBuildContext)
{
	HBuildHistory& currentBuildHistory = inBuildContext.CurrentBuildHistory();

	if (currentBuildHistory.BuildHistroy != EBuildHistory::Widget)
	{
		JG_LOG(GUI, ELogLevel::Error, "Fail GUI Build: Mismatch Begin/End Widget");
		return false;
	}

	OnContextMenu(inBuildContext, currentBuildHistory.Widget);

	ImGui::End();

	if (currentBuildHistory.Widget != nullptr)
	{
		HGuid widgetGuid = currentBuildHistory.Widget->GetGuid();
		if (inBuildContext.CacheData->WidgetCacheDatas[widgetGuid].bOpenedWidget == false)
		{
			GGUIGlobalSystem::GetInstance().CloseWidget(currentBuildHistory.Widget->GetGuid());
		}
	}

	inBuildContext.PopBuildHistory();
	inBuildContext.ParentWidget = nullptr;
	return true;
}

bool PImGuiBuild::OnBuildBeginChildWidget(HBuildContext& inBuildContext, HGUIBuilder::PChildWidgetCommandValue* inCV)
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

	inBuildContext.PushBuildHistroy(HBuildHistory(EBuildHistory::Widget, widget));

	HWidgetCacheData& widgetCachaData = inBuildContext.CacheData->WidgetCacheDatas[widget->GetGuid()];
	widgetCachaData.bAttendance   = true;
	widgetCachaData.bOpenedWidget = true;
	widgetCachaData.ParentWidget  = inBuildContext.ParentWidget;

	float32 widgetWidth  = 0.0f;
	float32 widgetHeight = 0.0f;
	if (widgetCachaData.ParentWidget != nullptr)
	{
		HWidgetCacheData& parentWidgetCacheData = inBuildContext.CacheData->WidgetCacheDatas[widgetCachaData.ParentWidget->GetGuid()];

		if (inCV->Args.WidthRatio > 0.0F)
		{
			widgetWidth = parentWidgetCacheData.ContentRegionSize.x * inCV->Args.WidthRatio;
		}

		if (inCV->Args.HeightRatio > 0.0F)
		{
			widgetHeight = parentWidgetCacheData.ContentRegionSize.y * inCV->Args.HeightRatio;
		}

		if (inCV->Args.FixedWidth > 0)
		{
			widgetWidth = (float32)inCV->Args.FixedWidth;
		}

		if (inCV->Args.FixedHeight > 0)
		{
			widgetHeight = (float32)inCV->Args.FixedHeight;
		}

	}

	bool bBorder = widgetFlags & EWidgetFlags::ChildWidget_Border;
	//
	ImGui::BeginChild((ImGuiID)widget->GetGuid().GetHashCode(), ImVec2(widgetWidth, widgetHeight), bBorder);

	ImVec2 WindowContentRegionMin = ImGui::GetWindowContentRegionMin();
	ImVec2 WindowContentRegionMax = ImGui::GetWindowContentRegionMax();

	widgetCachaData.ContentRegionSize = HVector2(WindowContentRegionMax.x, WindowContentRegionMax.y) - HVector2(WindowContentRegionMin.x, WindowContentRegionMin.y);

	inBuildContext.ParentWidget = widget;
	return true;
}

bool PImGuiBuild::OnBuildEndChildWidget(HBuildContext& inBuildContext)
{
	HBuildHistory& currentBuildHistory = inBuildContext.CurrentBuildHistory();
	PSharedPtr<WWidget> currentWidget  = currentBuildHistory.Widget;

	if (currentBuildHistory.BuildHistroy != EBuildHistory::Widget)
	{
		JG_LOG(GUI, ELogLevel::Error, "Fail GUI Build: Mismatch Begin/End Widget");
		return false;
	}

	OnContextMenu(inBuildContext, currentBuildHistory.Widget);
	ImGui::EndChild();

	inBuildContext.PopBuildHistory();
	currentBuildHistory = inBuildContext.CurrentBuildHistory();

	if (inBuildContext.bLastCompInLayout == false && currentBuildHistory.BuildHistroy == EBuildHistory::Horizontal)
	{
		ImGui::SameLine();
	}

	if (currentWidget != nullptr)
	{
		HWidgetCacheData& widgetCachaData = inBuildContext.CacheData->WidgetCacheDatas[currentWidget->GetGuid()];
		inBuildContext.ParentWidget = widgetCachaData.ParentWidget;
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
		PSharedPtr<PImGuiBuild> imGuiBuild = Allocate<PImGuiBuild>();
		imGuiBuild->ImGuiContext = ImGuiContext;

		imGuiBuild->PushData(guiBuilder.GetCommandQueue());
		imGuiBuild->OnBuild(inBuildContext);
	}

	OnContextMenu(inBuildContext, inCV->WidgetComponent);

	OnGUIEvent(inBuildContext, guid, guiEventReceiver);
	ImGui::EndGroup();
	ImGui::PopID();

	if (inBuildContext.bLastCompInLayout == false && inBuildContext.CurrentBuildHistory().BuildHistroy == EBuildHistory::Horizontal)
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

	HGuid widgetGuid = inBuildContext.ParentWidget->GetGuid();

	HWidgetContext widgetContext;
	widgetContext.ContentSize = inBuildContext.CacheData->WidgetCacheDatas[widgetGuid].ContentRegionSize;
	widgetContext.OnPostGUIContent.Add(SharedWrap(this), [&inBuildContext, this](HGUIBuilder& inBuilder)
		{
			if (inBuilder.GetCommandQueue().empty() == false)
			{
				PSharedPtr<PImGuiBuild> imGuiBuild = Allocate<PImGuiBuild>();
				imGuiBuild->ImGuiContext = ImGuiContext;

				imGuiBuild->PushData(inBuilder.GetCommandQueue());
				imGuiBuild->OnBuild(inBuildContext);
			}
		});

	inCV->OnGenerateGUI.ExecuteIfBound(widgetContext);

	OnContextMenu(inBuildContext, inCV->WidgetComponent);
	
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

void PImGuiBuild::OnContextMenu(HBuildContext& inBuildContext, PSharedPtr<WWidget> inWidget)
{
	if (inWidget.IsValid() == false)
	{
		return;
	}

	HGuid guid = inWidget->GetGuid();
	HWidgetCacheData& cacheData = inBuildContext.CacheData->WidgetCacheDatas[guid];
	if (cacheData.ContextMenuBuild.IsValid() == false)
	{
		cacheData.ContextMenuBuild = Allocate<PImGuiContextMenuBuild>();

		HContextMenuBuilder contextMenuBuilder;
		inWidget->OnContextMenuBuild(contextMenuBuilder);

		cacheData.ContextMenuBuild->PushData(contextMenuBuilder.GetMenuTrees());
	}
	if (inBuildContext.bOpenContextMenu == false)
	{
		if (cacheData.ContextMenuBuild->Build(guid))
		{
			inBuildContext.bOpenContextMenu = true;
		}
	}
}

void PImGuiBuild::OnContextMenu(HBuildContext& inBuildContext, PSharedPtr<WWidgetComponent> inWidgetComponent)
{
	if (inWidgetComponent.IsValid() == false)
	{
		return;
	}

	HGuid guid = inWidgetComponent->GetGuid();

	HWidgetComponentCacheData& cacheData = inBuildContext.CacheData->WidgetCompCahceDatas[guid];
	if (cacheData.ContextMenuBuild.IsValid() == false)
	{
		cacheData.ContextMenuBuild = Allocate<PImGuiContextMenuBuild>();

		HContextMenuBuilder contextMenuBuilder;
		inWidgetComponent->OnContextMenuBuild(contextMenuBuilder);

		cacheData.ContextMenuBuild->PushData(contextMenuBuilder.GetMenuTrees());
	}

	if (inBuildContext.bOpenContextMenu == false)
	{
		if (cacheData.ContextMenuBuild->Build(guid) == true)
		{
			inBuildContext.bOpenContextMenu = true;
		}
	}
}
