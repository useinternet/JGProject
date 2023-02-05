#pragma once
#include "WidgetContext.h"

class HGUIBuilder;
class IImGuiWidgetComponentGenerator
{
public:
//	virtual void OnGUIBuild(HGUIBuilder& inBuilder) = 0;
	virtual void GenerateImGuiWidgetComponent(const HWidgetContext& widgetContext) = 0;
};