#include "PCH/PCH.h"
#include "Widget.h"

WWidget::WWidget()
{
	_widgetName = NAME_NONE;
}

void WWidget::SetName(const PName& inName)
{
	_widgetName = inName;
}

const PName& WWidget::GetName() const
{
	return _widgetName;
}