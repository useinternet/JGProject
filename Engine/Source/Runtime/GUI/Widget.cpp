#include "PCH/PCH.h"
#include "Widget.h"

void WWidget::SetName(PName inName)
{
	_widgetName = inName;
}

PName WWidget::GetName() const
{
	return _widgetName;
}

const HGuid& WWidget::GetGuid() const
{
	return _widgetGuid;
}

void WWidget::Construct()
{
	_bIsOpen    = false;
	_widgetGuid = HGuid::New();
	OnConstruct();
}
