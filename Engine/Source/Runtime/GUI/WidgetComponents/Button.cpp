#include "PCH/PCH.h"
#include "Button.h"
#include "Builder/GUIBuilder.h"
#include "External/imgui/imgui.h"

void WButton::OnGUIBuild(HGUIBuilder& inBuilder)
{
	inBuilder.PushGenerateNativeGUI(SharedWrap(this), POnGenerateNativeGUI::Create<WButton>(SharedWrap(this), [&](const HWidgetContext& widgetContext)
	{
		bool bIsClick = ImGui::Button(
			Text.GetValue().GetCStr(),
			Size.GetValue());

		if (bIsClick == true)
		{
			OnClick.ExecuteIfBound();
		}
	}));
}
