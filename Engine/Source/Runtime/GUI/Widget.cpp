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

void WWidget::SetWidgetFlags(EWidgetFlags flags)
{
	_widgetFlags = flags;
}

void WWidget::AddWidgetFlags(EWidgetFlags flags)
{
	_widgetFlags = _widgetFlags | flags;
}

void WWidget::RemoveWidgetFlags(EWidgetFlags flags)
{
	_widgetFlags = (EWidgetFlags)(_widgetFlags & (~flags));
}

EWidgetFlags WWidget::GetWidgetFlags() const
{
	return _widgetFlags;
}


void WWidget::Construct()
{
	_bIsOpen    = false;
	_widgetGuid = HGuid::New();
	OnConstruct();
}
