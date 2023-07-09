#include "PCH/PCH.h"
#include "WDevelopUnitList.h"
#include "WDUTComboBox.h"
#include "Misc/DevelopUnit.h"
#include "WidgetComponents/WText.h"
#include "WidgetComponents/WInputText.h"
#include "WidgetComponents/WInputScalar.h"
#include "WidgetComponents/WButton.h"
#include "WidgetComponents/WList.h"


PDevelopUnitItem::PDevelopUnitItem(PSharedPtr<WList> OwnerList, JGDevelopUnit* DevelopUnit)
	: _ownerList(OwnerList), _developUnit(DevelopUnit) {}

PSharedPtr<WWidgetComponent> PDevelopUnitItem::CreateWidgetComponent()
{
	if (_cacheWidget == nullptr)
	{
		WDevelopItem::HArguments args;
		args.Item = SharedWrap(this);
		_cacheWidget = Allocate<WDevelopItem>(args);
	}

	return _cacheWidget;
}

void PDevelopUnitItem::OnSelected()
{
	if (_cacheWidget == nullptr)
	{
		return;
	}

	_cacheWidget->SetSelected(true);
}

void PDevelopUnitItem::OnDeselected()
{
	if (_cacheWidget == nullptr)
	{
		return;
	}

	_cacheWidget->SetSelected(false);
}

PDevelopUnitItem::~PDevelopUnitItem()
{
	HPlatform::Deallocate(_developUnit);
}

WDevelopItem::WDevelopItem(const WDevelopItem::HArguments& inArgs)
{
	Construct(inArgs);
}

void WDevelopItem::Construct(const WDevelopItem::HArguments& inArgs)
{
	_ownerList = inArgs.OwnerList;
	_ownerItem = inArgs.Item;

	_nameLabel = Allocate<WText>();
	_developUnitNameLabel = Allocate<WText>();
	_resetButton  = Allocate<WButton>();
	_deleteButton = Allocate<WButton>();

	_resetButton->Text  = "Reset";
	_deleteButton->Text = "Delete";

	WSelectable::HArguments args;
	args.StretchMode = EStretchMode::Horizontal;
	args.OnSelected = WSelectable::HOnSelected::Create(SharedWrap(this), JG_DELEGATE_FN_BIND(WDevelopItem::OnSelected));

	WSelectable::Construct(args);
}

void WDevelopItem::OnContent(HGUIBuilder& inBuilder)
{
	if (_ownerItem == nullptr)
	{
		return;
	}

	_nameLabel->Text = PString("Name : ") + _ownerItem->Name;
	_developUnitNameLabel->Text = PString("DevelopUnit : ") + _ownerItem->DevelopUnitName;

	inBuilder.BeginVertical();
	{
		inBuilder.PushWidgetComponent(_nameLabel);
		inBuilder.PushWidgetComponent(_developUnitNameLabel);

		inBuilder.BeginHorizontal();
		{
			inBuilder.PushWidgetComponent(_resetButton);
			inBuilder.PushWidgetComponent(_deleteButton);
		}
		inBuilder.EndHorizontal();
	}
	inBuilder.EndVertical();
}

void WDevelopItem::OnSelected()
{
	if (_ownerList.IsValid() == false)
	{
		return;
	}

	_ownerList.Pin()->SelectItem(_ownerItem);
}

void WDevelopUnitList::Construct()
{
	WWidget::Construct();

	SetWidgetFlags(EWidgetFlags::ChildWidget_Border);

	_dutComboBox = Allocate<WDUTComboBox>();
	_developUnitList = Allocate<WList>();
	_onAddItemButton = Allocate<WButton>();

	_dutComboBox->SetSelectedItemIndex(0);

	_onAddItemButton->Text = "Create";
	_onAddItemButton->OnClick = WButton::HOnClick::Create(SharedWrap(this), JG_DELEGATE_FN_BIND(WDevelopUnitList::OnAddItem));
	_onAddItemButton->StretchMode = EStretchMode::Horizontal;
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

	PSharedPtr<PDevelopUnitItem> Item = Allocate<PDevelopUnitItem>(_developUnitList, developUnit);
	Item->Name = developUnit->GetName().ToString();
	Item->DevelopUnitName = className;

	_listItems.push_back(Item);

	_developUnitList->SetItemList(_listItems);

	developUnit->Startup();
}

