#pragma once
#include "WidgetComponents/WBorder.h"

class WSelectable : public WBorder
{
public:
	JG_DECLARE_DELEGATE(HOnSelected);
	using HOnContent = WBorder::HOnContent;

	enum class EState
	{
		Default,
		Hover,
		Selected,
		Disable
	};

	struct HArguments
	{
		HOnSelected  OnSelected;
		HOnContent   OnContent;
		EStretchMode StretchMode;

		HArguments() : StretchMode(EStretchMode::None) {}
	};

public:
	WSelectable() : WBorder() {}
	virtual ~WSelectable() = default;

	void Construct(const HArguments& Args);

	void SetSelected(bool bSelect);
	bool IsSelected() const;
private:
	EState GetState() const;
	EState GetPrevState() const;
	void SetState(EState inState);

	HLinearColor GetBorderBackgroundColor() const;

	void OnLeftMouseClick();
	void OnMouseHovered();
	void OnMouseLeave();

private:
	HOnSelected _onSelected;
	EState _state;
	EState _prevState;
};