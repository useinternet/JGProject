#include "PCH/PCH.h"
#include "WSelectable.h"
#include "Builder/GUIBuilder.h"
#include "GUIStyles/GenericGUIStyle.h"
#include "External/imgui/imgui.h"

void WSelectable::Construct(const WSelectable::HArguments& InArgs)
{
	_state     = EState::Default;
	_prevState = EState::Default;

	_onSelected = InArgs.OnSelected;

	WBorder::HArguments Args;
	Args.OnMouseLeftClick = WBorder::HOnMouseLeftClick::CreateSP(SharedWrap(this), &WSelectable::OnLeftMouseClick);
	Args.OnMouseHovered = WBorder::HOnMouseHovered::CreateSP(SharedWrap(this), &WSelectable::OnMouseHovered);
	Args.OnMouseLeave = WBorder::HOnMouseLeave::CreateSP(SharedWrap(this), &WSelectable::OnMouseLeave);
	Args.StretchMode = InArgs.StretchMode;
	Args.BackGroundColor = HAttribute<HLinearColor>::HOnProvideData::CreateSP(SharedWrap(this), &WSelectable::GetBorderBackgroundColor);
	Args.OnContent = InArgs.OnContent;

	WBorder::Construct(Args);
}

void WSelectable::SetSelected(bool bSelect)
{
	EState State     = GetState();
	if (State == EState::Disable)
	{
		return;
	}

	if (bSelect)
	{
		SetState(EState::Selected);
	}
	else
	{
		SetState(EState::Default);
	}
}

bool WSelectable::IsSelected() const
{
	return _state == EState::Selected;
}

WSelectable::EState WSelectable::GetState() const
{
	return _state;
}

WSelectable::EState WSelectable::GetPrevState() const
{
	return _prevState;
}

void WSelectable::SetState(WSelectable::EState inState)
{
	_prevState = _state;
	_state = inState;

	if (_prevState != _state)
	{
		if (_state == EState::Selected)
		{
			_onSelected.ExecuteIfBound();
		}
	}
}

HLinearColor WSelectable::GetBorderBackgroundColor() const
{
	switch (GetState())
	{
	case WSelectable::EState::Default: return JGGenericGUIStyle::Get().GetStyleColor(JGGenericGUIStyle::Border);
	case WSelectable::EState::Hover: return JGGenericGUIStyle::Get().GetStyleColor(JGGenericGUIStyle::BorderHovered);
	case WSelectable::EState::Selected: return JGGenericGUIStyle::Get().GetStyleColor(JGGenericGUIStyle::BorderActive);
	case WSelectable::EState::Disable: return JGGenericGUIStyle::Get().GetStyleColor(JGGenericGUIStyle::BorderDisable);
	}

	return HLinearColor();
}

void WSelectable::OnLeftMouseClick()
{
	SetSelected(!IsSelected());

	JG_LOG(GUI, ELogLevel::Info, "Click Select (%d)", GetGuid().GetHashCode());
}

void WSelectable::OnMouseHovered()
{
	if (GetState() == EState::Selected || GetState() == EState::Disable)
	{
		return;
	}

	SetState(EState::Hover);
}

void WSelectable::OnMouseLeave()
{
	if (GetState() == EState::Selected || GetState() == EState::Disable)
	{
		return;
	}

	SetState(EState::Default);
}