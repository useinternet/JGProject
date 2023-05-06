#include "PCH/PCH.h"
#include "WText.h"
#include "Builder/GUIBuilder.h"
#include "External/imgui/imgui.h"


void WText::OnGUIBuild(HGUIBuilder& inBuilder)
{
	inBuilder.PushGenerateNativeGUI(SharedWrap(this), HOnGenerateNativeGUI::Create(SharedWrap(this), [&](const HWidgetContext& widgetContext)
	{
		PString Value;
		Text.GetValue(Value);
		ImGui::Text(Value.GetCStr());
	}));
}

