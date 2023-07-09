#pragma once
#include "WidgetComponents/WidgetComponent.h"

class WText : public WWidgetComponent
{
public:
	struct HArguments
	{
		HAttribute<PString> Text;
	};

public:
	HAttribute<PString> _text;

public:
	WText() : WWidgetComponent() {}
	virtual ~WText() = default;

	void Construct(const HArguments& inArgs);

	void SetText(const PString& inText);
protected:
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override;
};