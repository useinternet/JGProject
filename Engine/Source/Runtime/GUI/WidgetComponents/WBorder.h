#pragma once
#include "WidgetComponents/WidgetComponent.h"

class WBorder : public WWidgetComponent
{
protected:
	JG_DECLARE_DELEGATE(HOnMouseLeftClick);
	JG_DECLARE_DELEGATE(HOnMouseRightClick);

	HOnMouseLeftClick  _onMouseLeftClick;
	HOnMouseRightClick _onMouseRightClick;

	HAttribute<HLinearColor> _backGroundColor;
	EStretchMode _stretchMode = EStretchMode::None;

	HRect _cacheBorderRect;
public:
	struct HArguments
	{
		HOnMouseLeftClick  OnMouseLeftClick;
		HOnMouseRightClick OnMouseRightClick;
		HAttribute<HLinearColor> BackGroundColor;
		EStretchMode StretchMode;

		HArguments()
		{
			StretchMode = EStretchMode::None;
			BackGroundColor.SetValue(HLinearColor(0.26f, 0.59f, 0.98f, 0.31f));
		}
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