#pragma once
#include "GUI/JGUIObject/JGUIComponent.h"


class JGUIPanel;
class JGUIText;
class JGUIRectangle;

class JGUIInputText : public JGUIComponent
{
protected:
	virtual void Awake() override;
	virtual void Start() override;

	virtual void KeyDown(const JGUIKeyDownEvent& e) override;
	virtual void KeyUp(const JGUIKeyUpEvent& e) override;
	virtual void Char(const JGUICharEvent& e) override;
	virtual void FocusEnter(const JGUIFocusEnterEvent& e) override;
    virtual void OnFocus() override;
	virtual void FocusExit(const JGUIFocusExitEvent& e) override;
	virtual void Resize(const JGUIResizeEvent& e) override;
private:
	void Setting();
private:
	JGUIPanel* m_BackGound      = nullptr;
	JGUIPanel* m_TextBackGround = nullptr;
	JGUIRectangle* m_Flash = nullptr;
	JGUIText*      m_Text = nullptr;



	float         m_PressedTick = 0.0f;
	std::string   m_CurrentChar;
	

	float  m_FlashTick   = 0.0f;
	float  m_FlashTime   = 0.5f;
	float  m_FlashThickness = 5.0f;
};