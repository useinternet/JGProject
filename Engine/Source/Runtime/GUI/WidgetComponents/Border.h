#pragma once
#include "WidgetComponents/WidgetComponent.h"

class WBorder : public WWidgetComponent
{
public:
	WBorder() : WWidgetComponent() {}
	virtual ~WBorder() = default;

protected:
	virtual void GenerateImGuiWidgetComponent(const HWidgetContext& inWidgetContext) override;
};