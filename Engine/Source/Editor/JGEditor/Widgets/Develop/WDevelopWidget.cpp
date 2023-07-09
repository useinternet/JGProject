#include "PCH/PCH.h"
#include "WDevelopWidget.h"
#include "WDUTComboBox.h"
#include "Misc/DevelopUnit.h"
#include "WDevelopUnitList.h"
#include "WidgetComponents/WText.h"
#include "WidgetComponents/WInputText.h"
#include "WidgetComponents/WInputScalar.h"
#include "WidgetComponents/WButton.h"
#include "WidgetComponents/WList.h"

class WDevelopUnitExecuter : public WWidget
{
public:
	struct HArguments
	{

	};

private:
	PSharedPtr<WInputText> _inputText;

public:
	void Construct(const HArguments& InArgs) 
	{
		SetWidgetFlags(EWidgetFlags::ChildWidget_Border);
	}
protected:

	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override
	{
		// 프로세스 
		//PSharedPtr<WText> Text = Allocate<WText>();
		//Text->Text = "WDevelopUnitExecuter";
		//inBuilder.PushWidgetComponent(Text);
	}

	virtual void OnContextMenuBuild(HContextMenuBuilder& inBuilder) override
	{

	}
};

class WDevelopUnitContent : public WWidget
{
public:
	struct HArguments
	{

	};

public:
	void Construct(const HArguments& InArgs)
	{
		SetWidgetFlags(EWidgetFlags::ChildWidget_Border);
	}

protected:
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override
	{
		//PSharedPtr<WText> Text = Allocate<WText>();
		//Text->Text = "WDevelopUnitContent";
		//inBuilder.PushWidgetComponent(Text);
	}

	virtual void OnContextMenuBuild(HContextMenuBuilder& inBuilder) override
	{

	}
};

void WDevelopWidget::OnOpen()
{
	if (_developUnitExecuter == nullptr)
	{
		_developUnitExecuter = NewWidgetComponent<WDevelopUnitExecuter>();
	}

	if (_developUnitList == nullptr)
	{
		_developUnitList = NewWidgetComponent<WDevelopUnitList>();
	}

	if (_developUnitContent == nullptr)
	{
		_developUnitContent = NewWidgetComponent<WDevelopUnitContent>();
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
			inBuilder.PushChildWidget(_developUnitList,   HPushChildWidgetArguments(350, 0.0f));
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
