#pragma once
#include "Core.h"
#include "GUIDefine.h"


class HGUIBuilder;
class WWidgetComponent : public IMemoryObject
{
public:
	JG_DECLARE_DELEGATE_RETVAL(EWidgetVisibility, PIsVisibility);
	JG_DECLARE_DELEGATE_RETVAL(bool, PIsEnable);

	PIsVisibility IsVisiblity;
	PIsEnable	  IsEnable;

public:
	WWidgetComponent();
	virtual ~WWidgetComponent() = default;
	virtual void BuildWidget(HGUIBuilder& inBuilder) {}
};