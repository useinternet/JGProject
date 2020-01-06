#pragma once

#include "GUI/JGUIObject/JGUIWindow.h"


JGUI_MAIN_WINFORM(JGUIForm)


class JGUITitleBar;
class JGUIForm : public JGUIWindow
{
	JGUITitleBar* title = nullptr;
protected:
	virtual void Start() override;
	virtual void Tick(const JGUITickEvent& e) override;
	virtual void KeyDown(const JGUIKeyDownEvent& e) override;
	virtual void KeyUp(const JGUIKeyUpEvent& e) override;
};