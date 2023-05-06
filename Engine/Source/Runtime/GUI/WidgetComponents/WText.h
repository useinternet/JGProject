#pragma once
#include "WidgetComponents/WidgetComponent.h"

class WText : public WWidgetComponent
{
public:
	HAttribute<PString> Text;

public:
	WText() : WWidgetComponent() {}
	virtual ~WText() = default;

protected:
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override;
};