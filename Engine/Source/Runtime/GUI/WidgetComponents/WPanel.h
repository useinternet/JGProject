#pragma once
#include "WidgetComponents/WidgetComponent.h"

class WPanel : public WWidgetComponent
{
	// OnContents
	JG_DECLARE_DELEGATE_RETVAL(PSharedPtr<WWidgetComponent>, HOnContent);
	JG_DECLARE_DELEGATE(HOnMouseLeftClick);
	JG_DECLARE_DELEGATE(HOnMouseRightClick);


	HOnContent Content;
	HOnMouseLeftClick  OnMouseLeftClick;
	HOnMouseRightClick OnMouseRightClick;

public:
	WPanel() : WWidgetComponent() {}
	virtual ~WPanel() = default;

protected:
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override;
};