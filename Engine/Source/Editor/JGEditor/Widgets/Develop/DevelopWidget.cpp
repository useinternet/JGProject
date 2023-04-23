#include "PCH/PCH.h"
#include "DevelopWidget.h"
#include "Misc/DevelopUnit.h"
#include "WidgetComponents/Text.h"
#include "WidgetComponents/InputText.h"
#include "WidgetComponents/InputScalar.h"
#include "WidgetComponents/Button.h"


// 실행기
// DevelopUnitList

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
		// 프로세스
		PSharedPtr<WText> Text = Allocate<WText>();
		Text->Text = "WDevelopUnitExecuter";
		inBuilder.PushWidgetComponent(Text);
	}

	virtual void OnContextMenuBuild(HContextMenuBuilder& inBuilder) override
	{

	}
};

class WDevelopUnitList : public WWidget
{
public:
	WDevelopUnitList() : WWidget()
	{
		SetWidgetFlags(EWidgetFlags::ChildWidget_Border);
	}
protected:
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override
	{
		PSharedPtr<WText> Text = Allocate<WText>();
		Text->Text = "WDevelopUnitList";
		inBuilder.PushWidgetComponent(Text);
	}

	virtual void OnContextMenuBuild(HContextMenuBuilder& inBuilder) override
	{

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
