#include "PCH/PCH.h"
#include "WButton.h"
#include "Builder/GUIBuilder.h"
#include "External/imgui/imgui.h"

void WButton::OnGUIBuild(HGUIBuilder& inBuilder)
{
	inBuilder.PushGenerateNativeGUI(SharedWrap(this), HOnGenerateNativeGUI::Create(SharedWrap(this), [&](const HWidgetContext& widgetContext)
	{
		HVector2 buttonSize = Size.GetValue();
		switch (StretchMode.GetValue())
		{
		case EStretchMode::Vertical:
			buttonSize.y = widgetContext.ContentSize.y;
			break;

		case EStretchMode::Horizontal:
			buttonSize.x = widgetContext.ContentSize.x;
			break;
		}

		bool bIsClick = ImGui::Button(Text.GetValue().GetCStr(), buttonSize);

		if (bIsClick == true)
		{
			OnClick.ExecuteIfBound();
		}
	}));
}

void WButton::OnContextMenuBuild(HContextMenuBuilder& inBuilder)
{

}
