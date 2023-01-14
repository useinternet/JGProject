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
