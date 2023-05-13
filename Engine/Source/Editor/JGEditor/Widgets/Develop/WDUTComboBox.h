#pragma once
#include "WidgetComponents/WComboBox.h"

class PDUTComboItem;
class WDUTComboBox;
class PDUTComboItem : public IMemoryObject, public IComboItem
{
	PWeakPtr<WDUTComboBox> _ownerCombo;
	PSharedPtr<JGClass>    _class;
	PString _cacheStr;
public:
	PDUTComboItem(PSharedPtr<WDUTComboBox> inOwnerCombo, PSharedPtr<JGClass> inClass);
	virtual ~PDUTComboItem() = default;

	virtual const PString& GetLabel() const override;
	virtual void OnSelected() override;

	PSharedPtr<WDUTComboBox> GetOwnerCombo() const;
	PSharedPtr<JGClass> GetClass() const;
};

class WDUTComboBox : public WComboBox
{
	HList<PSharedPtr<PDUTComboItem>> _dUTItemList;

public:
	virtual ~WDUTComboBox() = default;

	virtual void Construct() override;

	PSharedPtr<JGClass> GetSelectedClass() const;
};