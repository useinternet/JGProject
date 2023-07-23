#pragma once
#include "Builder/GUIBuilder.h"

struct ImGuiContext;
class IContextMenuBuild;
class PImGuiBuild final : public IMemoryObject, public IGUIBuild
{
	enum class EBuildHistory
	{
		None,
		Horizontal,
		Vertical,
		Widget
	};

	struct HWidgetComponentCacheData
	{
		bool bAttendance = false;
		bool bHover      = false;
		PSharedPtr<IContextMenuBuild> ContextMenuBuild;
	};

	struct HWidgetCacheData
	{
		bool bAttendance   = false;
		bool bOpenedWidget = false;

		HVector2 ContentRegionSize;
		PSharedPtr<WWidget> ParentWidget;

		PSharedPtr<IContextMenuBuild> ContextMenuBuild;
	};

	struct HBuildCacheData
	{
		HVector2 CursorPos;
		HHashMap<HGuid, HWidgetComponentCacheData> WidgetCompCahceDatas;
		HHashMap<HGuid, HWidgetCacheData> WidgetCacheDatas;
	};

	struct HBuildHistory
	{
		EBuildHistory BuildHistroy;
		PSharedPtr<WWidget> Widget;
		
		HBuildHistory()
			: BuildHistroy(EBuildHistory::None)
			, Widget(nullptr) {}

		HBuildHistory(EBuildHistory inBuildHistory, PSharedPtr<WWidget> inWidget)
			: BuildHistroy(inBuildHistory)
			, Widget(inWidget) {}

	};

	struct HBuildContext
	{
		HStack<HBuildHistory> HistoryStack;
		HBuildCacheData*	  CacheData;
		HVector2			  CurrentWindowRegionSize;
		PSharedPtr<WWidget>   ParentWidget;

		bool bIsHandledGUIEvents[(int32)EGUIEvent::Count];

		int32 FixedWidth;
		int32 FixedHeight;
		
		bool bOpenContextMenu;
		bool bIsDirtyMousePos;
		bool bLastCompInLayout;

		void PushBuildHistroy(const HBuildHistory& inBuildHistory)
		{
			HistoryStack.push(inBuildHistory);
		}

		const HBuildHistory& CurrentBuildHistory() const
		{
			if (HistoryStack.empty())
			{
				static HBuildHistory InvalidHistroy;
				return InvalidHistroy;
			}

			return HistoryStack.top();
		}

		HBuildHistory& CurrentBuildHistory()
		{
			if (HistoryStack.empty())
			{
				static HBuildHistory InvalidHistroy;
				return InvalidHistroy;
			}

			return HistoryStack.top();
		}

		void PopBuildHistory()
		{
			if (HistoryStack.empty())
			{
				return;
			}

			HistoryStack.pop();
		}
	};

	HList<HQueue<HGUIBuilder::HCommandData>> _commandQueues;
	HBuildCacheData _buildCacheData;
public:
	ImGuiContext* ImGuiContext = nullptr;

public:
	virtual void PushData(const HQueue<HGUIBuilder::HCommandData>& inCommandQueue) override;
	virtual void Build() override;
	virtual void Reset() override;
private:
	void OnBuild(HBuildContext& inBuildContext);
	bool OnBuild(HBuildContext& inBuildContext, const HGUIBuilder::HCommandData& inCommandData);

	bool OnBuildBeginHorizontal(HBuildContext& inBuildContext, HGUIBuilder::PHorizontalCommandValue* inCV);
	bool OnBuildEndHorizontal(HBuildContext& inBuildContext);

	bool OnBuildBeginVertical(HBuildContext& inBuildContext, HGUIBuilder::PVerticalCommandValue* inCV);
	bool OnBuildEndVertical(HBuildContext& inBuildContext);

	bool OnBuildPushChildWidget(HBuildContext& inBuildContext, HGUIBuilder::PChildWidgetCommandValue* inCV);
	bool OnBuildBeginWidget(HBuildContext& inBuildContext, HGUIBuilder::PWidgetCommandValue* inCV);
	bool OnBuildEndWidget(HBuildContext& inBuildContext);
	bool OnBuildBeginChildWidget(HBuildContext& inBuildContext, HGUIBuilder::PChildWidgetCommandValue* inCV);
	bool OnBuildEndChildWidget(HBuildContext& inBuildContext);


	bool OnBuildWidgetComponent(HBuildContext& inBuildContext, HGUIBuilder::PWidgetComponentCommandValue* inCV);
	bool OnBuildGenerateNativeGUI(HBuildContext& inBuildContext, HGUIBuilder::PGenerateNativeGUICommandValue* inCV);

	void OnContextMenu(HBuildContext& inBuildContext, PSharedPtr<WWidget> inWidget);
	void OnContextMenu(HBuildContext& inBuildContext, PSharedPtr<WWidgetComponent> inWidgetComponent);
};