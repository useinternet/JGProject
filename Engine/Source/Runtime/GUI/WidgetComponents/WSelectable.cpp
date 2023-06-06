#include "PCH/PCH.h"
#include "WSelectable.h"
#include "Builder/GUIBuilder.h"
#include "External/imgui/imgui.h"


WSelectable::WSelectable(const WSelectable::HArguments& InArgs)
{
	Construct(InArgs);
}

void WSelectable::Construct(const WSelectable::HArguments& InArgs)
{
	WBorder::HArguments Args;
	Args.OnMouseLeftClick = WBorder::HOnMouseLeftClick::Create(SharedWrap(this), JG_DELEGATE_FN_BIND(WSelectable::OnLeftMouseClick));
	Args.StretchMode = InArgs.StretchMode;
	Args.BackGroundColor.SetValue(HLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
	WBorder::Construct(Args);
}

void WSelectable::OnLeftMouseClick()
{
	bool bPrevSelect = bSelect;
	bSelect = true;

	if (bPrevSelect != bSelect)
	{
		OnSelect.ExecuteIfBound();
	}

	JG_LOG(GUI, ELogLevel::Info, "Click Select (%d)", GetGuid().GetHashCode());
}