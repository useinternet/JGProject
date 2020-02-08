#pragma once

#include "GUI/JGUIObject/JGUIWindow.h"


JGUI_MAIN_WINFORM(JGUIForm)
class JGUITitleBar;
class JGUIRectangle;
class JGUIText;
class JGUIResizeBox;
class JGUIWinTexture;
class JGUIInputText;

class JGUIForm : public JGUIWindow
{
	JGUIText* text = nullptr;
	JGUIInputText* inputText = nullptr;
protected:
	virtual void Start() override;
	virtual void KeyUp(const JGUIKeyUpEvent& e) override;

};





