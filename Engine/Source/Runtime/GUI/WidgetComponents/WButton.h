#pragma once
#include "WidgetComponents/WidgetComponent.h"

class WButton : public WWidgetComponent
{
	
public:
	JG_DECLARE_DELEGATE(HOnClick);
	
	HAttribute<PString>  Text;
	HAttribute<HVector2> Size;
	HAttribute<EStretchMode> StretchMode;
	HOnClick OnClick;

public:
	WButton() : WWidgetComponent() {}
	virtual ~WButton() = default;

protected:
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override;
	virtual void OnContextMenuBuild(HContextMenuBuilder& inBuilder) override;
};