#include "PCH/PCH.h"
#include "DevelopWidget.h"
#include "WidgetComponents/Text.h"

void WDevelopWidget::OnOpen()
{
	TestText = Allocate<WText>();
	TestText->Text.SetValue(PString("TestTest1234566"));
}

void WDevelopWidget::OnClose()
{
}

void WDevelopWidget::OnGUIBuild(HGUIBuilder& inBuilder)
{
	inBuilder.PushWidgetComponent(TestText);
}

void WDevelopWidget::OnMenuBuild(HMenuBuilder& inBuilder)
{
}
