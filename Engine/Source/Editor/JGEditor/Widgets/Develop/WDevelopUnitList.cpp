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
	return Allocate<WDevelopItem>(SharedWrap(this));
}

PDevelopUnitItem::~PDevelopUnitItem()
{
	HPlatform::Deallocate(_developUnit);
}

void PDevelopUnitItem::OnSelected()
{
	JG_LOG(TEST, ELogLevel::Info, "Click Item : %s", Name);
}

void WDevelopItem::Construct()
{
	WWidgetComponent::Construct();

	NameLabel = Allocate<WText>();
	DevelopUnitNameLabel = Allocate<WText>();
	ResetButton  = Allocate<WButton>();
	DeleteButton = Allocate<WButton>();

	ResetButton->Text  = "Reset";
	DeleteButton->Text = "Delete";
}

void WDevelopItem::OnGUIBuild(HGUIBuilder& inBuilder)
{
	if (_ownerItem == nullptr)
	{
		return;
	}

	NameLabel->Text = PString("Name : ") + _ownerItem->Name;
	DevelopUnitNameLabel->Text = PString("DevelopUnit : ") + _ownerItem->DevelopUnitName;

	inBuilder.BeginVertical();
	{
		inBuilder.PushWidgetComponent(NameLabel);
		inBuilder.PushWidgetComponent(DevelopUnitNameLabel);

		inBuilder.BeginHorizontal();
		{
			inBuilder.PushWidgetComponent(ResetButton);
			inBuilder.PushWidgetComponent(DeleteButton);
		}
		inBuilder.EndHorizontal();
	}
	inBuilder.EndVertical();
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

