#pragma once
#include "Widget.h"
#include "JGEditorDefine.h"

class WDevelopWidget;
class WDevelopUnitContent : public WWidget
{
public:
	struct HArguments
	{
		PWeakPtr<WDevelopWidget> OwnerWidget;
	};

private:
	PWeakPtr<WDevelopWidget> _ownerWidget;

public:
	void Construct(const HArguments& InArgs);

protected:
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override;
	virtual void OnContextMenuBuild(HContextMenuBuilder& inBuilder) override {}
};