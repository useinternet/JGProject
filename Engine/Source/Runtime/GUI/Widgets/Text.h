#pragma once
#include "Widgets/WidgetComponent.h"


class WText : public WWidgetComponent
{

public:
	HAttribute<PString> Text;

public:
	WText() : WWidgetComponent() {}
	virtual ~WText() = default;
};