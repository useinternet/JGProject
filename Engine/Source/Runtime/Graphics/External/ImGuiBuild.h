#pragma once
#include "Builder/GUIBuilder.h"
#include "JGGraphicsDefine.h"

class PImGuiBuild final : public IMemoryObject, public IGUIBuild
{
	HList<HQueue<HGUIBuilder::HCommandData>> _commandQueues;
public:
	virtual void PushData(const HQueue<HGUIBuilder::HCommandData>& inCommandQueue) override;
	virtual void Build() override;
	virtual void Reset() override;

private:
	void OnBuild(const HGUIBuilder::HCommandData& inCommandData);
	void OnBuildBeginWidget(HGUIBuilder::PWidgetCommandValue* inCV);
	void OnBuildEndWidget();
	void OnBuildText(HGUIBuilder::PTextCommandValue* inCV);
};