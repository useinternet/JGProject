#pragma once
#include "WidgetComponents/WPanel.h"


class WSelectable : public WPanel
{
	JG_DECLARE_DELEGATE(HOnSelected);

	HOnSelected OnSelect;

public:
	virtual ~WSelectable() = default;
};