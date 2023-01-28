#include "PCH/PCH.h"
#include "DevelopWidget.h"
#include "WidgetComponents/Text.h"
#include "WidgetComponents/InputText.h"
#include "WidgetComponents/InputScalar.h"
#include "WidgetComponents/Button.h"

void WDevelopWidget::OnOpen()
{
	TestText = Allocate<WText>();
	TestText->Text.SetValue(PString("Test Element"));
}

void WDevelopWidget::OnClose()
{

}

void WDevelopWidget::OnGUIBuild(HGUIBuilder& inBuilder)
{

}

void WDevelopWidget::OnMenuBuild(HMenuBuilder& inBuilder)
{
}
