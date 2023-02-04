#pragma once
#include "Builder/GUIBuilder.h"

struct ImGuiContext;
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
		bool bHover = false;
	};

	struct HBuildCacheData
	{
		HVector2 CursorPos;
		HHashMap<HGuid, HWidgetComponentCacheData> WidgetCompCahceDatas;
	};

	struct HBuildContext
	{
		HStack<EBuildHistory> HistoryStack;
		PSharedPtr<WWidget>   CurrentWidget;
		HBuildCacheData*	  CacheData;
		bool bIsHandledGUIEvents[(int32)EGUIEvent::Count];

		int32 FixedWidth;
		int32 FixedHeight;
		
		bool bOpenWidget;
		bool bIsDirtyMousePos;
		bool bLastCompInLayout;

		void PushBuildHistroy(EBuildHistory history)
		{
			HistoryStack.push(history);
		}

		EBuildHistory CurrentBuildHistory() const
		{
			if (HistoryStack.empty())
			{
				return EBuildHistory::None;
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

	bool OnBuildBeginWidget(HBuildContext& inBuildContext, HGUIBuilder::PWidgetCommandValue* inCV);
	bool OnBuildEndWidget(HBuildContext& inBuildContext);

	bool OnBuildWidgetComponent(HBuildContext& inBuildContext, HGUIBuilder::PWidgetComponentCommandValue* inCV);

	void OnGUIEvent(HBuildContext& inBuildContext, const HGuid& guid, IGUIEventReceiver* inEventReceiver);
};