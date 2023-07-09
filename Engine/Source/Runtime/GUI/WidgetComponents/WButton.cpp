#include "PCH/PCH.h"
#include "WButton.h"
#include "Builder/GUIBuilder.h"
#include "External/imgui/imgui.h"

void WButton::Construct(const HArguments& inArgs)
{
	_text = inArgs.Text;
	_size = inArgs.Size;
	_stretchMode = inArgs.StretchMode;
	_onClick	 = inArgs.OnClick;
}

void WButton::OnGUIBuild(HGUIBuilder& inBuilder)
{
	inBuilder.PushGenerateNativeGUI(SharedWrap(this), HOnGenerateNativeGUI::Create(SharedWrap(this), [&](const HWidgetContext& widgetContext)
	{
		HVector2 buttonSize = _size.GetValue();
		switch (_stretchMode.GetValue())
		{
		case EStretchMode::Vertical:
			buttonSize.y = ImGui::GetContentRegionAvail().y;
			break;

		case EStretchMode::Horizontal:
			buttonSize.x = ImGui::GetContentRegionAvail().x;
			break;
		}
	

		bool bIsClick = ImGui::Button(_text.GetValue().GetCStr(), buttonSize);

		if (bIsClick == true)
		{
			_onClick.ExecuteIfBound();
		}
	}));
}

void WButton::OnContextMenuBuild(HContextMenuBuilder& inBuilder)
{

}
