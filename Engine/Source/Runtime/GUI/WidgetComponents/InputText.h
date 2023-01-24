#pragma once
#include "WidgetComponents/WidgetComponent.h"

class WInputText : public WWidgetComponent
{

public:
	WInputText() : WWidgetComponent() {}
	virtual ~WInputText() = default;

protected:
	virtual void GenerateImGuiWidgetComponent(const HWidgetContext& inWidgetContext) override;
};