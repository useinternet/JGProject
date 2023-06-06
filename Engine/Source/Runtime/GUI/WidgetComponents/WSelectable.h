#pragma once
#include "WidgetComponents/WBorder.h"


class WSelectable : public WBorder
{
	JG_DECLARE_DELEGATE(HOnSelected);

	HOnSelected OnSelect;
	bool bSelect = false;

public:
	struct HArguments
	{
		HOnSelected  OnSelect;
		EStretchMode StretchMode;

		HArguments() : StretchMode(EStretchMode::None) {}
	};

public:
	WSelectable(const HArguments& Args);
	WSelectable() : WBorder() {}
	virtual ~WSelectable() = default;

protected:
	void Construct(const HArguments& Args);


private:
	void OnLeftMouseClick();
};