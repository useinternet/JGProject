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
class JGDevelopUnitListData;

class PDevelopUnitItem
	: public IMemoryObject
	, public IListItem
{
	PWeakPtr<WList> _ownerList;
	JGDevelopUnit*  _developUnit;
	PSharedPtr<JGDevelopUnitListData> _developUnitListData;

	PSharedPtr<WDevelopItem> _cacheWidget;

public:
	PString Name;
	PString DevelopUnitName;
	PString OrgDllName;
	PString DllName;
public:
	PDevelopUnitItem(PSharedPtr<WList> OwnerList, PSharedPtr<JGDevelopUnitListData> InDevelopUnitListData, JGDevelopUnit* DevelopUnit);
	virtual ~PDevelopUnitItem();

	void Reload();
	JGDevelopUnit* GetDevelopUnit() const;
	PSharedPtr<JGDevelopUnitListData> GetDevelopUnitListData() const;
};

class WDevelopItem : public WWidgetComponent
{
public:
	JG_DECLARE_DELEGATE(HOnSelected, PSharedPtr<WDevelopItem>);

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
	PSharedPtr<WButton> _reloadButton;
	PSharedPtr<WButton> _deleteButton;
public:
	virtual ~WDevelopItem() = default;

	void Construct(const HArguments& inArgs);

protected:
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override;

	void OnClickedReload();
	void OnClickedDelete();
};

class WDevelopUnitList : public WWidget
{
public:
	struct HArguments
	{
		PSharedPtr<JGDevelopUnitListData> DevelopUnitListData;
		WList::HOnSelectChanged OnSelectChanged;
	};

private:
	PSharedPtr<JGDevelopUnitListData> _developUnitListData;

	PSharedPtr<WList> _developUnitList;
	HList<PSharedPtr<PDevelopUnitItem>> _listItems;

	PSharedPtr<WDUTComboBox> _dutComboBox;
	PSharedPtr<WButton> _onAddItemButton;

	WList::HOnSelectChanged _onSelectChanged;
public:
	WDevelopUnitList() = default;
	virtual ~WDevelopUnitList() = default;

	void Construct(const HArguments& inArgs);
	void OnSelectedItemChanged(PSharedPtr<IListItem> inItem, bool inSelected);
	PSharedPtr<WWidgetComponent> OnGenerateWidgetComponent(PSharedPtr<IListItem> inItem);

protected:
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override;
	void OnAddItem();
};