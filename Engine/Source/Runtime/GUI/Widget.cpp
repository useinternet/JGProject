#include "PCH/PCH.h"
#include "Widget.h"

WWidget::WWidget() : _bAllowUpdate(false)
{ }

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
