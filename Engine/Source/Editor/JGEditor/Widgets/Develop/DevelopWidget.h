#pragma once
#include "Widget.h"


class WText;
class WDevelopWidget : public WWidget
{
	PSharedPtr<WText> TestText;
public:
	WDevelopWidget() = default;
	virtual ~WDevelopWidget() = default;

	virtual void OnOpen() override;
	virtual void OnClose() override;
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override;
	virtual void OnMenuBuild(HMenuBuilder& inBuilder) override;
};