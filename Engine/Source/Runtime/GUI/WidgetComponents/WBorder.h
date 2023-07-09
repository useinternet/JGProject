#pragma once
#include "WidgetComponents/WidgetComponent.h"

class WBorder : public WWidgetComponent
{
protected:
	JG_DECLARE_DELEGATE(HOnMouseLeftClick);
	JG_DECLARE_DELEGATE(HOnMouseRightClick);
	JG_DECLARE_DELEGATE(HOnMouseHovered);
	JG_DECLARE_DELEGATE(HOnMouseEnter);
	JG_DECLARE_DELEGATE(HOnMouseLeave);


	HOnMouseLeftClick  _onMouseLeftClick;
	HOnMouseRightClick _onMouseRightClick;
	HOnMouseHovered	   _onMouseHovered;
	HOnMouseEnter _onMouseEnter;
	HOnMouseLeave _onMouseLeave;

	HAttribute<HLinearColor> _backGroundColor;
	EStretchMode _stretchMode = EStretchMode::None;

	HRect _cacheBorderRect;

	bool _bHover;
public:
	struct HArguments
	{
		HOnMouseLeftClick  OnMouseLeftClick;
		HOnMouseRightClick OnMouseRightClick;
		HOnMouseHovered	   OnMouseHovered;
		HOnMouseEnter	   OnMouseEnter;
		HOnMouseLeave	   OnMouseLeave;

		HAttribute<HLinearColor> BackGroundColor;
		EStretchMode StretchMode;

		HArguments();

	};
public:
	WBorder(const HArguments& args);
	WBorder() : WWidgetComponent() {}
	virtual ~WBorder() = default;

public:
	void SetBackGroundColor(const HAttribute<HLinearColor>& inColor);

	HLinearColor GetBackGroundColor() const;


protected:
	void Construct(const HArguments& args);

protected:
	virtual void OnContent(HGUIBuilder& inBuilder) {}
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override;
};