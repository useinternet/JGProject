#include "PCH/PCH.h"
#include "WDevelopUnitList.h"
#include "WDUTComboBox.h"
#include "Misc/DevelopUnit.h"
#include "WidgetComponents/WText.h"
#include "WidgetComponents/WInputText.h"
#include "WidgetComponents/WInputScalar.h"
#include "WidgetComponents/WButton.h"
#include "WidgetComponents/WList.h"

#include "Datas/DevelopUnitData.h"
#include "Datas/DevelopUnitListData.h"


PDevelopUnitItem::PDevelopUnitItem(PSharedPtr<WList> OwnerList, JGDevelopUnit* DevelopUnit)
	: _ownerList(OwnerList), _developUnit(DevelopUnit) {}

void PDevelopUnitItem::Reload()
{
	JGDevelopUnit::ReloadDevelopUnit(_developUnit);
}

void PDevelopUnitItem::Remove()
{
	JGDevelopUnit::UnloadDevelopUnit(_developUnit, true);
}

JGDevelopUnit* PDevelopUnitItem::GetDevelopUnit() const
{
	return _developUnit;
}

PDevelopUnitItem::~PDevelopUnitItem()
{
	HPlatform::Deallocate(_developUnit);
}

void WDevelopItem::Construct(const WDevelopItem::HArguments& inArgs)
{
	_ownerList = inArgs.OwnerList;
	_ownerItem = inArgs.Item;
	_onRemove  = inArgs.OnRemove;

	_nameLabel = NewWidgetComponent<WText>();
	_developUnitNameLabel = NewWidgetComponent<WText>();


	WButton::HArguments buttonArgs;
	buttonArgs.Text = "Reload";
	buttonArgs.OnClick = WButton::HOnClick::CreateSP(SharedWrap(this), &WDevelopItem::OnClickedReload);
	_reloadButton = NewWidgetComponent<WButton>(buttonArgs);

	buttonArgs.Text = "Remove";
	buttonArgs.OnClick = WButton::HOnClick::CreateSP(SharedWrap(this), &WDevelopItem::OnClickedRemove);
	_deleteButton = NewWidgetComponent<WButton>(buttonArgs);
}

void WDevelopItem::OnGUIBuild(HGUIBuilder& inBuilder)
{
	if (_ownerItem == nullptr)
	{
		return;
	}

	_nameLabel->SetText(PString("Name : ") + _ownerItem->Name);
	_developUnitNameLabel->SetText(PString("DevelopUnit : ") + _ownerItem->DevelopUnitName);

	inBuilder.BeginVertical();
	{
		inBuilder.PushWidgetComponent(_nameLabel);
		inBuilder.PushWidgetComponent(_developUnitNameLabel);

		inBuilder.BeginHorizontal();
		{
			inBuilder.PushWidgetComponent(_reloadButton);
			inBuilder.PushWidgetComponent(_deleteButton);
		}
		inBuilder.EndHorizontal();
	}
	inBuilder.EndVertical();
}

void WDevelopItem::OnClickedReload()
{
	if (_ownerItem != nullptr)
	{
		_ownerItem->Reload();
	}
	// Dll 제거 후 다시 연결
}

void WDevelopItem::OnClickedRemove()
{
	if (_ownerItem != nullptr)
	{
		_onRemove.ExecuteIfBound(_ownerItem);
		_ownerItem->Remove();
	}
}

void WDevelopUnitList::Construct(const WDevelopUnitList::HArguments& InArgs)
{
	SetWidgetFlags(EWidgetFlags::ChildWidget_Border);

	_developUnitListData = InArgs.DevelopUnitListData;
	_onSelectChanged = InArgs.OnSelectChanged;
	_dutComboBox     = NewWidgetComponent<WDUTComboBox>();
	_dutComboBox->SetSelectedItemIndex(0);

	WList::HArguments args;
	args.OnSelectChanged = WList::HOnSelectChanged::CreateSP(SharedWrap(this), &WDevelopUnitList::OnSelectedItemChanged);
	args.OnGenerateWidgetComponent = WList::HOnGenerateWidgetComponent::CreateSP(SharedWrap(this), &WDevelopUnitList::OnGenerateWidgetComponent);

	_developUnitList = NewWidgetComponent<WList>(args);

	WButton::HArguments buttonArgs;
	buttonArgs.Text = "Create";
	buttonArgs.OnClick = WButton::HOnClick::CreateSP(SharedWrap(this), &WDevelopUnitList::OnAddItem);
	buttonArgs.StretchMode = EStretchMode::Horizontal;

	_onAddItemButton = NewWidgetComponent<WButton>(buttonArgs);

	for (PSharedPtr<JGDevelopUnitData> unitData : _developUnitListData->DevelopUnitDatas)
	{
		PSharedPtr<JGClass> unitClass = StaticClass(unitData->UnitType);
		if (unitClass == nullptr)
		{
			continue;
		}

		JGDevelopUnit* devUnit = JGDevelopUnit::LoadDevelopUnit(unitClass, unitData->UnitGuid);
		CreateItem(devUnit);
	}
}

void WDevelopUnitList::OnSelectedItemChanged(PSharedPtr<IListItem> inItem, bool inSelected)
{
	_onSelectChanged.ExecuteIfBound(inItem, inSelected);
}

PSharedPtr<WWidgetComponent> WDevelopUnitList::OnGenerateWidgetComponent(PSharedPtr<IListItem> inItem)
{
	WDevelopItem::HArguments args;
	args.Item = Cast<PDevelopUnitItem>(inItem);
	args.OwnerList = _developUnitList;
	args.OnRemove = WDevelopItem::HOnRemove::CreateSP(SharedWrap(this), &WDevelopUnitList::OnRemoveItem);

	return NewWidgetComponent<WDevelopItem>(args);;
}

void WDevelopUnitList::OnGUIBuild(HGUIBuilder& inBuilder)
{
	inBuilder.PushWidgetComponent(_developUnitList);
	inBuilder.BeginHorizontal();
	inBuilder.PushWidgetComponent(_dutComboBox);
	inBuilder.PushWidgetComponent(_onAddItemButton);
	inBuilder.EndHorizontal();
}

void WDevelopUnitList::CreateItem(JGDevelopUnit* inDevelopUnit)
{
	if (inDevelopUnit == nullptr)
	{
		return;
	}

	PSharedPtr<JGClass> Class = inDevelopUnit->GetClass();

	PSharedPtr<PDevelopUnitItem> Item = Allocate<PDevelopUnitItem>(_developUnitList, inDevelopUnit);
	Item->Name = inDevelopUnit->GetName().ToString();
	Item->DevelopUnitName = Class->GetClassType()->GetName().ToString();

	_listItems.push_back(Item);
	_developUnitList->SetItemList(_listItems);
	_developUnitListData->AddUnitData(inDevelopUnit);
}

void WDevelopUnitList::OnAddItem()
{
	PSharedPtr<JGClass> Class = _dutComboBox->GetSelectedClass();
	if (Class == nullptr)
	{
		return;
	}

	JGDevelopUnit* developUnit = JGDevelopUnit::LoadDevelopUnit(Class);
	if (developUnit == nullptr)
	{
		return;
	}

	CreateItem(developUnit);
}

void WDevelopUnitList::OnRemoveItem(PSharedPtr<PDevelopUnitItem> inItem)
{
	int32 numItem = (int32)_listItems.size();
	for (int32 i = 0; i < numItem; ++i)
	{
		if (_listItems[i] == inItem)
		{
			_developUnitListData->RemoveUnitData(inItem->GetDevelopUnit());
			_listItems.erase(_listItems.begin() + i);
			break;
		}
	}
}