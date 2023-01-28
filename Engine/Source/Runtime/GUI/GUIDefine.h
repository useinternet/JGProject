#pragma once

enum class EWidgetVisibility
{
	Visible,
	Hide,
};

enum class EReply
{
	Handled,
	UnHandled,
};

class IGUIEventReceiver
{
public:
	virtual EReply OnMouseMove() = 0;
	virtual EReply OnMouseHover() = 0;
	virtual EReply OnMouseLeave() = 0;
	virtual EReply OnMouseLButtonDown() = 0;
	virtual EReply OnMouseRButtonDown() = 0;
	virtual EReply OnMouseMButtonDown() = 0;
	virtual EReply OnMouseLButtonUp() = 0;
	virtual EReply OnMouseRButtonUp() = 0;
	virtual EReply OnMouseMButtonUp() = 0;
	virtual EReply OnMouseLButtonClick() = 0;
	virtual EReply OnMouseRButtonClick() = 0;
	virtual EReply OnMouseMButtonClick() = 0;
	virtual EReply OnMouseLButtonDoubleClick() = 0;
	virtual EReply OnMouseRButtonDoubleClick() = 0;
	virtual EReply OnMouseMButtonDoubleClick() = 0;
};