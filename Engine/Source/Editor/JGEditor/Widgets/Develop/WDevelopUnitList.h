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
	void OnSelected() override;
};

class WDevelopItem : public WSelectable
{
	PSharedPtr<PDevelopUnitItem> _ownerItem;


	struct HArguments
	{
		PSharedPtr<PDevelopUnitItem> Item;

	};

	PSharedPtr<WText> NameLabel;
	PSharedPtr<WText> DevelopUnitNameLabel;
	PSharedPtr<WButton> ResetButton;
	PSharedPtr<WButton> DeleteButton;

public:
	WDevelopItem(PSharedPtr<PDevelopUnitItem> inItem) : _ownerItem(inItem) {}
	virtual ~WDevelopItem() = default;
protected:
	virtual void Construct() override;
	virtual void OnContent(HGUIBuilder& inBuilder) override;
private:
	//virtual void OnGUIBuild(HGUIBuilder& inBuilder) override;
};

class WDevelopUnitList : public WWidget
{
	// 테스트 프로세스 추가
	// 테스트 프로세스 관리
	PSharedPtr<WList> _developUnitList;
	HList<PSharedPtr<PDevelopUnitItem>> _listItems;


	PSharedPtr<WDUTComboBox> _dutComboBox;
	PSharedPtr<WButton> _onAddItemButton;
public:
	virtual ~WDevelopUnitList() = default;

protected:
	virtual void Construct() override;
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override;
	void OnAddItem();
};