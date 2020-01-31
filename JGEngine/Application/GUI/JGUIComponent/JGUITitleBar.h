#pragma once
#include "GUI/JGUIObject/JGUIComponent.h"






class JGUIPanel;
class JGUICloseButton;
class JGUIButton;
class JGUIMaximizeButton;
class JGUIMinimizeButton;
class JGUITitleBar : public JGUIComponent
{
protected:
	virtual void Awake() override;
	virtual void Resize(const JGUIResizeEvent& e) override;
	virtual void MouseBtDown(const JGUIKeyDownEvent& e) override;
	virtual void MouseBtUp(const JGUIKeyUpEvent& e) override;
private:
	void SettingElement();
private:
	JGUIPanel*          m_Panel   = nullptr;
	JGUICloseButton*    m_CloseBt = nullptr;
	JGUIMaximizeButton* m_MaxBt   = nullptr;
	JGUIMinimizeButton* m_MinBt   = nullptr;

	bool m_IsGrap = false;


	JVector2Int m_Delta;
};