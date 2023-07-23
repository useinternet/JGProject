#include "PCH/PCH.h"
#include "WDevelopUnitList.h"
#include "WDUTComboBox.h"
#include "Misc/DevelopUnit.h"
#include "WidgetComponents/WText.h"
#include "WidgetComponents/WInputText.h"
#include "WidgetComponents/WInputScalar.h"
#include "WidgetComponents/WButton.h"
#include "WidgetComponents/WList.h"


PDevelopUnitItem::PDevelopUnitItem(PSharedPtr<WList> OwnerList, PSharedPtr<JGDevelopUnitListData> InDevelopUnitListData, JGDevelopUnit* DevelopUnit)
	: _ownerList(OwnerList), _developUnitListData(InDevelopUnitListData), _developUnit(DevelopUnit) {}

void PDevelopUnitItem::Reload()
{
	if (_developUnit)
	{
		HPlatform::Deallocate(_developUnit);
	}

	HGuid   dutGuid = HGuid::New();
	PString newDllName = OrgDllName + "_" + dutGuid.ToString();

	if (false == HFileHelper::CopyFileOrDirectory(OrgDllName, newDllName))
	{
		return;
	}

	DllName = newDllName;

	HJInstance dllInstance = HPlatform::LoadDll(DllName);
	if (dllInstance == nullptr)
	{
		JG_LOG(JGEditor, ELogLevel::Error, "Fail Load DevelopUnit(%s)", DllName);
		return;
	}

	HPlatformFunction<void, GCoreSystem*> linkModuleFunc = HPlatform::LoadFuncInDll<void, GCoreSystem*>(dllInstance, JG_LINK_DEVELOPUNIT_FUNCTION_NAME);
	if (linkModuleFunc.IsVaild() == false)
	{
		JG_LOG(JGEditor, ELogLevel::Error, "Fail Load DevelopUnit(%s)", DllName);
		return;
	}

	linkModuleFunc(&GCoreSystem::GetInstance());

	HPlatformFunction<JGDevelopUnit*> createDevelopUnit = HPlatform::LoadFuncInDll<JGDevelopUnit*>(dllInstance, JG_CREATE_DEVELOPUNIT_FUNCTION_NAME);
	if (createDevelopUnit.IsVaild() == false)
	{
		JG_LOG(JGEditor, ELogLevel::Error, "Fail Load DevelopUnit(%s)", DllName);
		return;
	}

	_developUnit = createDevelopUnit();
}

JGDevelopUnit* PDevelopUnitItem::GetDevelopUnit() const
{
	return _developUnit;
}

PSharedPtr<JGDevelopUnitListData> PDevelopUnitItem::GetDevelopUnitListData() const
{
	return _developUnitListData;
}

PDevelopUnitItem::~PDevelopUnitItem()
{
	HPlatform::Deallocate(_developUnit);
}

void WDevelopItem::Construct(const WDevelopItem::HArguments& inArgs)
{
	_ownerList = inArgs.OwnerList;
	_ownerItem = inArgs.Item;

	_nameLabel = NewWidgetComponent<WText>();
	_developUnitNameLabel = NewWidgetComponent<WText>();


	WButton::HArguments buttonArgs;
	buttonArgs.Text = "Reload";
	buttonArgs.OnClick = WButton::HOnClick::CreateSP(SharedWrap(this), &WDevelopItem::OnClickedReload);
	_reloadButton = NewWidgetComponent<WButton>(buttonArgs);

	buttonArgs.Text = "Delete";
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

void WDevelopItem::OnClickedDelete()
{
	// 말그대로 제거
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

void WDevelopUnitList::OnAddItem()
{
	PSharedPtr<JGClass> Class = _dutComboBox->GetSelectedClass();
	if (Class == nullptr)
	{
		return;
	}
	//DUT_Graphics_Dynamic
	PString className = Class->GetClassType()->GetName().ToString();
	PString dllName   = className + "_Dynamic.dll";
	if (dllName.StartWidth("JG"))
	{
		dllName.Remove(0, 2);
	}

	PString orgDllName = dllName;

	HGuid dutGuid = HGuid::New();
	PString newDllName = dllName + "_" + dutGuid.ToString();

	if (false == HFileHelper::CopyFileOrDirectory(dllName, newDllName))
	{
		return;
	}

	dllName = newDllName;

	HJInstance dllInstance = HPlatform::LoadDll(dllName);
	if (dllInstance == nullptr)
	{
		JG_LOG(JGEditor, ELogLevel::Error, "Fail Load DevelopUnit(%s)", dllName);
		return;
	}

	HPlatformFunction<void, GCoreSystem*> linkModuleFunc = HPlatform::LoadFuncInDll<void, GCoreSystem*>(dllInstance, JG_LINK_DEVELOPUNIT_FUNCTION_NAME);
	if (linkModuleFunc.IsVaild() == false)
	{
		JG_LOG(JGEditor, ELogLevel::Error, "Fail Load DevelopUnit(%s)", dllName);
		return;
	}

	linkModuleFunc(&GCoreSystem::GetInstance());

	HPlatformFunction<JGDevelopUnit*> createDevelopUnit  = HPlatform::LoadFuncInDll<JGDevelopUnit*>(dllInstance, JG_CREATE_DEVELOPUNIT_FUNCTION_NAME);
	if(createDevelopUnit.IsVaild() == false)
	{
		JG_LOG(JGEditor, ELogLevel::Error, "Fail Load DevelopUnit(%s)", dllName);
		return;
	}

	JGDevelopUnit* developUnit = createDevelopUnit();
	if (nullptr == developUnit)
	{
		return;
	}

	PSharedPtr<PDevelopUnitItem> Item = Allocate<PDevelopUnitItem>(_developUnitList, _developUnitListData, developUnit);
	Item->Name = developUnit->GetName().ToString();
	Item->DevelopUnitName = className;
	Item->OrgDllName = orgDllName;
	Item->DllName = dllName;

	_listItems.push_back(Item);

	_developUnitList->SetItemList(_listItems);

	developUnit->Startup();
}