#include "PCH/PCH.h"
#include "WidgetComponent.h"

WWidgetComponent::WWidgetComponent()
{
	_guid = HGuid::New();
}

const HGuid& WWidgetComponent::GetGuid() const
{
	return _guid;
}
