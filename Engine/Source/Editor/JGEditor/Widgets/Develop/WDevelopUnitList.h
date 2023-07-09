#pragma once
#include "Widget.h"
#include "JGEditorDefine.h"
#include "WidgetComponents/WList.h"
#include "WidgetComponents/WSelectable.h"

class JGDevelopUnit;
class WDUTComboBox;
class WButton;
class WDevelopItem;
class WText;

class PDevelopUnitItem
	: public IMemoryObject
	, public IListItem
{
	PWeakPtr<WList> _ownerList;
	JGDevelopUnit*  _developUnit;
	PSharedPtr<WDevelopItem> _cacheWidget;

public:
	PString Name;
	PString DevelopUnitName;

public:
	PDevelopUnitItem(PSharedPtr<WList> OwnerList, JGDevelopUnit* DevelopUnit);
	virtual ~PDevelopUnitItem();

protected:
	PSharedPtr<WWidgetComponent> CreateWidgetComponent() override;
	virtual void OnSelected() override;
	virtual void OnDeselected() override;
};

class WDevelopItem : public WSelectable
{
public:
	JG_DECLARE_DELEGATE_ONEPARAM(HOnSelected, PSharedPtr<WDevelopItem>);

	struct HArguments
	{
		PWeakPtr<WList> OwnerList;
		PSharedPtr<PDevelopUnitItem> Item;
	};


private:
	PWeakPtr<WList> _ownerList;
	PSharedPtr<PDevelopUnitItem> _ownerItem;

	PSharedPtr<WText> _nameLabel;
	PSharedPtr<WText> _developUnitNameLabel;
	PSharedPtr<WButton> _resetButton;
	PSharedPtr<WButton> _deleteButton;
public:
	WDevelopItem(const HArguments& inArgs);
	virtual ~WDevelopItem() = default;
protected:
	void Construct(const HArguments& inArgs);
	virtual void OnContent(HGUIBuilder& inBuilder) override;

	void OnSelected();
};

class WDevelopUnitList : public WWidget
{
	PSharedPtr<WList> _developUnitList;
	HList<PSharedPtr<PDevelopUnitItem>> _listItems;

	PSharedPtr<WDUTComboBox> _dutComboBox;
	PSharedPtr<WButton> _onAddItemButton;
public:
	virtual ~WDevelopUnitList() = default;

protected:
// IMemoryObject
	virtual void Construct() override;
// ~// IMemoryObject


	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override;
	void OnAddItem();
	void OnItemSelected();
};