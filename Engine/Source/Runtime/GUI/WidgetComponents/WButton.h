#pragma once
#include "WidgetComponents/WidgetComponent.h"

class WButton : public WWidgetComponent
{
public:
	JG_DECLARE_DELEGATE(HOnClick);

	struct HArguments
	{
		HAttribute<PString>  Text;
		HAttribute<HVector2> Size;
		HAttribute<EStretchMode> StretchMode;
		HOnClick OnClick;
	};

public:
	HAttribute<PString>  _text;
	HAttribute<HVector2> _size;
	HAttribute<EStretchMode> _stretchMode;
	HOnClick _onClick;

public:
	WButton() : WWidgetComponent() {}
	virtual ~WButton() = default;

	void Construct(const HArguments& inArgs);

protected:
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override;
	virtual void OnContextMenuBuild(HContextMenuBuilder& inBuilder) override;
};