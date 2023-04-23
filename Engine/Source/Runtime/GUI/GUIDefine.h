#pragma once
#include "CoreDefines.h"

JG_ENUM_FLAG(EWidgetFlags)
enum class EWidgetFlags : int32
{
	None,
	ChildWidget_Border = 0x001,
};

enum class EWidgetVisibility
{
	Visible,
	Hide,
};

enum class EGUIEvent
{
	None,
	MouseMove,
	MouseHover,
	MouseEnter,
	MouseLeave,
	MouseLButtonDown,
	MouseRButtonDown,
	MouseMButtonDown,
	MouseLButtonUp,
	MouseRButtonUp,
	MouseMButtonUp,
	MouseLButtonClick,
	MouseRButtonClick,
	MouseMButtonClick,
	MouseLDoubleClick,
	MouseRDoubleClick,
	MouseMDoubleClick,
	Count,
};

class IGUIEventReceiver
{
public:
	virtual void OnMouseMove() = 0;
	virtual void OnMouseHover() = 0;
	virtual void OnMouseEnter() = 0;
	virtual void OnMouseLeave() = 0;
	virtual void OnMouseLButtonDown() = 0;
	virtual void OnMouseRButtonDown() = 0;
	virtual void OnMouseMButtonDown() = 0;
	virtual void OnMouseLButtonUp() = 0;
	virtual void OnMouseRButtonUp() = 0;
	virtual void OnMouseMButtonUp() = 0;
	virtual void OnMouseLButtonClick() = 0;
	virtual void OnMouseRButtonClick() = 0;
	virtual void OnMouseMButtonClick() = 0;
	virtual void OnMouseLButtonDoubleClick() = 0;
	virtual void OnMouseRButtonDoubleClick() = 0;
	virtual void OnMouseMButtonDoubleClick() = 0;
};