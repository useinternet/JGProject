#pragma once

#include "GUI/JGUIObject/JGUIWindow.h"


JGUI_MAIN_WINFORM(JGUIForm)

class JGUIRectangle;
class JGUIPanel;
class JGUIButton;
class JGUIForm : public JGUIWindow
{
	JGUIWindow* window = nullptr;
	JGUIPanel* rect1 = nullptr;
	JGUIButton* rect2 = nullptr;
protected:
	virtual void Start() override;
	virtual void Tick(const JGUITickEvent& e) override;
	virtual void KeyDown(const JGUIKeyDownEvent& e) override;
	virtual void KeyUp(const JGUIKeyUpEvent& e) override;
};