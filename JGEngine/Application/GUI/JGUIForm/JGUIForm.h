#pragma once

#include "GUI/JGUIObject/JGUIWindow.h"







class JGENGINE_API JGUIForm : public JGUIWindow
{
	friend class JGUIWinForm;
	friend class JGUIShellForm;
protected:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Resize(const JGUIResizeEvent& e) override;
	virtual void Tick(const  JGUITickEvent& e) override;
	virtual void KeyDown(const JGUIKeyDownEvent& e) override;
	virtual void KeyUp(const JGUIKeyUpEvent& e) override;
	virtual void MouseWheel(const JGUIMouseWheelEvent& e) override;
public:



public:


private:

};





