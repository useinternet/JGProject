#include "PCH/PCH.h"
#include "Button.h"
#include "External/imgui/imgui.h"

void WButton::GenerateImGuiWidgetComponent(const HWidgetContext& inWidgetContext)
{
	bool bIsClick = ImGui::Button(
		Text.GetValue().GetCStr(),
		Size.GetValue());

	if (bIsClick == true)
	{
		OnClick.ExecuteIfBound();
	}
}
