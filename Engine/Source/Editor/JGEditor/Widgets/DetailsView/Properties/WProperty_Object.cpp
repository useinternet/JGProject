#include "PCH/PCH.h"
#include "WProperty_Object.h"
#include "WidgetComponents/WText.h"
#include "External/imgui/imgui.h"

void WProperty_Object::Construct(const HArguments& inArgs)
{


	WProperty::Construct(inArgs);
}

void WProperty_Object::OnContent(HGUIBuilder& inBuilder)
{
	//
	inBuilder.PushGenerateNativeGUI(SharedWrap(this), HOnGenerateNativeGUI::CreateLambda([](const HWidgetContext& widgetContext)
		{
			ImGui::Text("Test Object Content Area");
		}));
}
