#include "PCH/PCH.h"
#include "WText.h"
#include "Builder/GUIBuilder.h"
#include "External/imgui/imgui.h"

void WText::Construct(const HArguments& inArgs)
{
	_text = inArgs.Text;
}

void WText::SetText(const PString& inText)
{
	_text = inText;
}

void WText::OnGUIBuild(HGUIBuilder& inBuilder)
{
	inBuilder.PushGenerateNativeGUI(SharedWrap(this), HOnGenerateNativeGUI::Create(SharedWrap(this), [&](const HWidgetContext& widgetContext)
	{
		PString Value;
		_text.GetValue(Value);
		ImGui::Text(Value.GetCStr());
	}));
}

