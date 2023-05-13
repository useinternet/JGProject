#include "PCH/PCH.h"
#include "WDevelopWidget.h"
#include "WDUTComboBox.h"
#include "Misc/DevelopUnit.h"
#include "WidgetComponents/WText.h"
#include "WidgetComponents/WInputText.h"
#include "WidgetComponents/WInputScalar.h"
#include "WidgetComponents/WButton.h"
#include "WidgetComponents/WList.h"

/* DeveopUnitTest ���
* �׽�Ʈ ȯ�� 
* ����/�ε�
* 
*/

class WDevelopUnitList;
class WDevelopItem;
class PDevelopUnitItem;

class WDevelopUnitExecuter : public WWidget
{
	PSharedPtr<WInputText> _inputText;
public:
	WDevelopUnitExecuter() : WWidget()
	{
		SetWidgetFlags(EWidgetFlags::ChildWidget_Border);
	}
protected:

	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override
	{
		// ���μ��� 
		PSharedPtr<WText> Text = Allocate<WText>();
		Text->Text = "WDevelopUnitExecuter";
		inBuilder.PushWidgetComponent(Text);
	}

	virtual void OnContextMenuBuild(HContextMenuBuilder& inBuilder) override
	{

	}
};

class WDevelopItem : public WWidgetComponent
{
	PSharedPtr<PDevelopUnitItem> _ownerItem;

	PSharedPtr<WText> NameLabel;
	PSharedPtr<WText> DevelopUnitNameLabel;
	PSharedPtr<WButton> ResetButton;

public:
	WDevelopItem(PSharedPtr<PDevelopUnitItem> inItem) : _ownerItem(inItem) {}
private:
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override 
	{
		// ���� ���
		// �̸�,  ������ X ��ư <- ����
		// Ÿ�� : DUT_Graphics ����..
	}
};

class PDevelopUnitItem 
	: public IMemoryObject
	, public IListItem
{
	PWeakPtr<WDevelopUnitList> _ownerWidget;
public:
	// Item ����
	PString Name;
	PString DevelopUnitName;

	

public:
	PDevelopUnitItem(PSharedPtr<WDevelopUnitList> OwnerWidget) : _ownerWidget(OwnerWidget) {}
	virtual ~PDevelopUnitItem() = default;

protected:
	virtual PSharedPtr<WWidgetComponent> CreateWidgetComponent() override
	{
		return Allocate<WDevelopItem>(SharedWrap(this));
	}

	virtual void OnSelected() override
	{

	}

};



class WDevelopUnitList : public WWidget
{
	// �׽�Ʈ ���μ��� �߰�
	// �׽�Ʈ ���μ��� ����
	PSharedPtr<WList> _developUnitList;
	HList<PSharedPtr<IListItem>> _listItems;

	
	PSharedPtr<WDUTComboBox> _dutComboBox;
	PSharedPtr<WButton> _onAddItemButton;

public:
	virtual void Construct() override 
	{
		WWidget::Construct();

		SetWidgetFlags(EWidgetFlags::ChildWidget_Border);

		_dutComboBox = Allocate<WDUTComboBox>();
		_onAddItemButton = Allocate<WButton>();

		_dutComboBox->SetSelectedItemIndex(0);

		_onAddItemButton->Text = "+";
		_onAddItemButton->OnClick = WButton::HOnClick::Create(SharedWrap(this), JG_DELEGATE_FN_BIND(WDevelopUnitList::OnAddItem));
		_onAddItemButton->StretchMode = EStretchMode::Horizontal;
	}
protected:
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override
	{
		inBuilder.BeginHorizontal();
		inBuilder.PushWidgetComponent(_dutComboBox);
		inBuilder.PushWidgetComponent(_onAddItemButton);
		inBuilder.EndHorizontal();
	}
private:
	void OnAddItem()
	{
		JG_LOG(DevelopUnit, ELogLevel::Info, "Click");
	}
};

class WDevelopUnitContent : public WWidget
{
public:
	WDevelopUnitContent() : WWidget()
	{
		SetWidgetFlags(EWidgetFlags::ChildWidget_Border);
	}
protected:
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override
	{
		PSharedPtr<WText> Text = Allocate<WText>();
		Text->Text = "WDevelopUnitContent";
		inBuilder.PushWidgetComponent(Text);
	}

	virtual void OnContextMenuBuild(HContextMenuBuilder& inBuilder) override
	{

	}
};


void WDevelopWidget::OnOpen()
{
	if (_developUnitExecuter == nullptr)
	{
		_developUnitExecuter = Allocate<WDevelopUnitExecuter>();
	}

	if (_developUnitList == nullptr)
	{
		_developUnitList = Allocate<WDevelopUnitList>();
	}

	if (_developUnitContent == nullptr)
	{
		_developUnitContent = Allocate<WDevelopUnitContent>();
	}
}

void WDevelopWidget::OnClose()
{
	//Reset();
}

void WDevelopWidget::OnUpdate()
{
	//// dll

	//try
	//{
	//	if (_developUnit != nullptr)
	//	{
	//		_developUnit->Update();
	//	}
	//}
	//catch (...)
	//{
	//	Reset();
	//}
}

void WDevelopWidget::OnGUIBuild(HGUIBuilder& inBuilder)
{
	inBuilder.BeginVertical();
	{
		inBuilder.PushChildWidget(_developUnitExecuter, HPushChildWidgetArguments(0.0f, 30));

		inBuilder.BeginHorizontal();
		{
			inBuilder.PushChildWidget(_developUnitList,   HPushChildWidgetArguments(200, 0.0f));
			inBuilder.PushChildWidget(_developUnitContent, HPushChildWidgetArguments(0.0f, 0.0f));
		}
		inBuilder.EndHorizontal();
	}
	inBuilder.EndVertical();

}

void WDevelopWidget::OnMenuBuild(HMenuBuilder& inBuilder)
{

}

void WDevelopWidget::OnContextMenuBuild(HContextMenuBuilder& inBuilder)
{

}

//void WDevelopWidget::SetDevelopUnit(const PString& inDevelopUnitName)
//{
//	Reset();
//	CreateDevelopUnit(inDevelopUnitName);
//}
//
//void WDevelopWidget::CreateDevelopUnit(const PString& inDevelopUnitName)
//{
//	if (_developUnit != nullptr)
//	{
//		return;
//	}
//
//	PString dllName = inDevelopUnitName + ".dll";
//
//	_dllInstance = HPlatform::LoadDll(dllName);
//
//	if (_dllInstance == nullptr)
//	{
//		JG_LOG(JGEditor, ELogLevel::Error, "Fail Load DevelopUnit(%s)", inDevelopUnitName);
//		return;
//	}
//
//	//HPlatformFunction<void, GCoreSystem*> linkModuleFunc = HPlatform::LoadFuncInDll<void, GCoreSystem*>(_dllInstance, JG_LINK_DEVELOPUNIT_FUNCTION_NAME);
//
//	HPlatformFunction<JGDevelopUnit*> createDevelopUnit = HPlatform::LoadFuncInDll<JGDevelopUnit*>(_dllInstance, JG_CREATE_DEVELOPUNIT_FUNCTION_NAME);
//	if(createDevelopUnit.IsVaild() == false)
//	{
//		JG_LOG(JGEditor, ELogLevel::Error, "Fail Load DevelopUnit(%s)", inDevelopUnitName);
//		return;
//	}
//
//	_developUnit = createDevelopUnit();
//	_developUnit->Startup();
//
//	OnCreateDevelopUnit();
//}
//
//void WDevelopWidget::Reset()
//{
//	if (_developUnit == nullptr)
//	{
//		return;
//	}
//
//	OnDestoryDevelopUnit();
//
//	_developUnit->Shutdown();
//
//	HPlatform::Deallocate(_developUnit);
//}
