#pragma once

#include "GUI/JGUIObject/JGUIWindow.h"


JGUI_MAIN_WINFORM(JGUIForm)
class JGUITitleBar;
class JGUIRectangle;
class JGUIText;
class JGUIResizeBox;

class JGUIForm : public JGUIWindow
{
	JGUITitleBar*  m_Title = nullptr;

	JGUIResizeBox* m_ResizeBox = nullptr;

	//TestCode
	JGUIText* text = nullptr;
protected:
	virtual void Start() override;
	virtual void Tick(const JGUITickEvent& e) override;
	virtual void Resize(const JGUIResizeEvent& e) override;
	virtual void KeyDown(const JGUIKeyDownEvent& e) override;
	virtual void KeyUp(const JGUIKeyUpEvent& e) override;
	virtual void Char(const JGUICharEvent& e) override;
};


