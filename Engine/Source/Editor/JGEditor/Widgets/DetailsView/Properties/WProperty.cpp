#include "PCH/PCH.h"
#include "WProperty.h"
#include "WidgetComponents/WText.h"
#include "External/imgui/imgui.h"



void WProperty::Construct(const HArguments& inArgs)
{
	_propertyType = inArgs.PropertyType;
}

void WProperty::OnGUIBuild(HGUIBuilder& inBuilder)
{
	inBuilder.BeginHorizontal();

	OnTitle(inBuilder);
	inBuilder.PushGenerateNativeGUI(SharedWrap(this), HOnGenerateNativeGUI::CreateLambda([](const HWidgetContext& widgetContext)
		{
			ImGui::Dummy(ImVec2(50.0f, 0.0f));
		}));

	OnContent(inBuilder);
	inBuilder.EndHorizontal();
}

void WProperty::OnTitle(HGUIBuilder& inBuilder)
{
	PName propertyName = _propertyType.GetName();

	WText::HArguments args;
	args.Text = propertyName.ToString();

	inBuilder.PushWidgetComponent(NewWidgetComponent<WText>(args));
}