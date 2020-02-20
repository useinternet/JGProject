#pragma once

#include "GUI/JGUIObject/JGUIWindow.h"


JGUI_MAIN_WINFORM(JGUIForm)


class JGUIForm : public JGUIWindow
{
	class JGUITitleBar*       m_Title     = nullptr;
	class JGUIResizeBox*      m_ResizeBox = nullptr;
	class JGUIEmptyRectangle* m_Border    = nullptr;
	class JGUIMenu*           m_Menu      = nullptr;

	//TestCode
	class JGUIText* m_Txt = nullptr;
protected:
	virtual void Start() override;
	virtual void Tick(const JGUITickEvent& e) override;
	virtual void Resize(const JGUIResizeEvent& e) override;
	virtual void KeyUp(const JGUIKeyUpEvent& e) override;
};





