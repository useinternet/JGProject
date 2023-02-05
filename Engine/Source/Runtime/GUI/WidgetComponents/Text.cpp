#include "PCH/PCH.h"
#include "Text.h"
#include "Builder/GUIBuilder.h"
#include "External/imgui/imgui.h"


void WText::OnGUIBuild(HGUIBuilder& inBuilder)
{
	inBuilder.PushGenerateNativeGUI(SharedWrap(this), POnGenerateNativeGUI::Create<WText>(SharedWrap(this), [&](const HWidgetContext& widgetContext)
	{
		PString Value;
		Text.GetValue(Value);
		ImGui::Text(Value.GetCStr());
	}));
}
