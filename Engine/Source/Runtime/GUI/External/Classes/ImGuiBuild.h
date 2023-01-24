#pragma once
#include "Builder/GUIBuilder.h"


// 이거 GUI로 옮기기
// 
struct ImGuiContext;
class PImGuiBuild final : public IMemoryObject, public IGUIBuild
{
	HList<HQueue<HGUIBuilder::HCommandData>> _commandQueues;
	
	PSharedPtr<WWidget> _currentWidget;
	bool _bOpenWidget;
public:
	ImGuiContext* ImGuiContext = nullptr;

public:
	virtual void PushData(const HQueue<HGUIBuilder::HCommandData>& inCommandQueue) override;
	virtual void Build() override;
	virtual void Reset() override;
private:
	void OnBuild(const HGUIBuilder::HCommandData& inCommandData);
	void OnBuildBeginWidget(HGUIBuilder::PWidgetCommandValue* inCV);
	void OnBuildEndWidget();
	void OnBuildWidgetComponent(HGUIBuilder::PWidgetComponentCommandValue* inCV);
};