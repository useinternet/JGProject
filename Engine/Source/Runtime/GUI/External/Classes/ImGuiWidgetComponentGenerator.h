#pragma once
#include "WidgetContext.h"

class IImGuiWidgetComponentGenerator
{
public:
	virtual void GenerateImGuiWidgetComponent(const HWidgetContext& widgetContext) = 0;
};