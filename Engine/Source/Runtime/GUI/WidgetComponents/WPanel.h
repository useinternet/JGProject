#pragma once
#include "WidgetComponents/WidgetComponent.h"

class WPanel : public WWidgetComponent
{
public:
	JG_DECLARE_DELEGATE_RETVAL(PSharedPtr<WWidgetComponent>, HOnContent);
	HOnContent Content;

protected:
	JG_DECLARE_DELEGATE(HOnMouseLeftClick);
	JG_DECLARE_DELEGATE(HOnMouseRightClick);

	HOnMouseLeftClick  OnMouseLeftClick;
	HOnMouseRightClick OnMouseRightClick;

public:
	WPanel() : WWidgetComponent() {}
	virtual ~WPanel() = default;

protected:
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override;
};