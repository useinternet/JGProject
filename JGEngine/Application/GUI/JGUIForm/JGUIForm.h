#pragma once

#include "GUI/JGUIObject/JGUIWindow.h"


JGUI_MAIN_WINFORM(JGUIForm)


class JGUITitleBar;
class JGUIText;
class JGUITestForm;
class JGUIForm : public JGUIWindow
{
	JGUITitleBar* title = nullptr;
	JGUIText* text = nullptr;
	class JGUIButton* bt = nullptr;

	JGUIText* debugText = nullptr;
	JGUIPanel* debug = nullptr;
	JGUIPanel* textBox = nullptr;

	JGUITestForm* testForm = nullptr;
protected:

	virtual void Start() override;
	virtual void Resize(const JGUIResizeEvent& e) override;
	virtual void Tick(const JGUITickEvent& e) override;
	virtual void KeyDown(const JGUIKeyDownEvent& e) override;
	virtual void KeyUp(const JGUIKeyUpEvent& e) override;
	virtual void Char(const JGUICharEvent& e) override;
};

class JGUITestForm : public JGUIWindow
{
	JGUITitleBar* title = nullptr;

protected:
	virtual void Start() override;
	virtual void Resize(const JGUIResizeEvent& e) override;
};