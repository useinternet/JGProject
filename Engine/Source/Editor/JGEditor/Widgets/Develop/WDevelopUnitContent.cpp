#include "PCH/PCH.h"
#include "WDevelopUnitContent.h"
#include "WDevelopWidget.h"
#include "WDevelopUnitList.h"
#include "Misc/DevelopUnit.h"

void WDevelopUnitContent::Construct(const HArguments& InArgs)
{
	SetWidgetFlags(EWidgetFlags::ChildWidget_Border);

	_ownerWidget = InArgs.OwnerWidget;
}

void WDevelopUnitContent::OnGUIBuild(HGUIBuilder& inBuilder)
{
	if (_ownerWidget.IsValid())
	{
		PSharedPtr<PDevelopUnitItem> selectedItem = _ownerWidget.Pin()->GetSelectedDevelopUnit();
		if (selectedItem != nullptr)
		{
			JGDevelopUnit* developUnit = selectedItem->GetDevelopUnit();
			if (developUnit != nullptr)
			{
				inBuilder.PushWidgetComponent(developUnit->CreateContentWidgetComponent());
			}
		}
	}
}
