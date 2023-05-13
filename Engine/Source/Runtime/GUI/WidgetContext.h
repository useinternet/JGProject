#pragma once
#include "Core.h"

class WWidgetComponent;
JG_DECLARE_DELEGATE_ONEPARAM(HPushWidgetComponent, PSharedPtr<WWidgetComponent>)

class HWidgetContext
{
public:
	HVector2 ContentSize;

	HPushWidgetComponent PushWidgetComponent;
};