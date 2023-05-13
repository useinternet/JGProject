#include "PCH/PCH.h"
#include "WDUTComboBox.h"
#include "Builder/GUIBuilder.h"
#include "WidgetComponents/WText.h"
#include "Misc/DevelopUnit.h"
#include "External/imgui/imgui.h"
#include "External/imgui/imgui_internal.h"

PDUTComboItem::PDUTComboItem(PSharedPtr<WDUTComboBox> inOwnerCombo, PSharedPtr<JGClass> inClass)
{
	_ownerCombo = inOwnerCombo;
	_class      = inClass;
	if (_class != nullptr)
	{
		_cacheStr = _class->GetClassType()->GetName().ToString();
	}
}

const PString& PDUTComboItem::GetLabel() const
{
	if (_ownerCombo.IsValid() == false || _class == nullptr)
	{
		return PString::None();
	}

	return _cacheStr;
}

void PDUTComboItem::OnSelected()
{


}

PSharedPtr<WDUTComboBox> PDUTComboItem::GetOwnerCombo() const
{
	return _ownerCombo.Pin();
}

PSharedPtr<JGClass> PDUTComboItem::GetClass() const
{
	return _class;
}


void WDUTComboBox::Construct()
{
	PSharedPtr<JGClass> Class = StaticClass<JGDevelopUnit>();
	if (Class == nullptr)
	{
		return;
	}

	HList<PSharedPtr<JGClass>> ChildClasses = Class->GetChildClasses(true);
	for (PSharedPtr<JGClass> Class : ChildClasses)
	{
		if (Class == nullptr)
		{
			continue;
		}

		PSharedPtr<PDUTComboItem> comboItem = Allocate<PDUTComboItem>(SharedWrap(this), Class);

		_dUTItemList.push_back(comboItem);
	}

	SetItemList(_dUTItemList);
}

PSharedPtr<JGClass> WDUTComboBox::GetSelectedClass() const
{
	PSharedPtr<PDUTComboItem> selectedItem = Cast<PDUTComboItem>(GetSelectedItem());
	if (selectedItem != nullptr)
	{
		return selectedItem->GetClass();
	}

	return nullptr;
}
