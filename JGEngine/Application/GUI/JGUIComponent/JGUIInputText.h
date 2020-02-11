#pragma once
#include "GUI/JGUIObject/JGUIComponent.h"


class JGUIPanel;
class JGUIText;
class JGUIRectangle;


/* Text 밀리게 하기

1. Shader 안에서 특정 Pos 이후 에는 그리지 않게한다.
2. Text 클래스 안에서 특정 Pos만큼 왼쪽으로 이동


*/
class JGUIInputText : public JGUIComponent
{
protected:
	virtual void Awake() override;
	virtual void Start() override;


	virtual void MouseBtDown(const JGUIKeyDownEvent& e) override;
	virtual void MouseBtUp(const JGUIKeyUpEvent& e) override;
	virtual void KeyDown(const JGUIKeyDownEvent& e) override;
	virtual void KeyUp(const JGUIKeyUpEvent& e) override;
	virtual void Char(const JGUICharEvent& e) override;
	virtual void FocusEnter(const JGUIFocusEnterEvent& e) override;
    virtual void OnFocus() override;
	virtual void FocusExit(const JGUIFocusExitEvent& e) override;
	virtual void Resize(const JGUIResizeEvent& e) override;

public:
	void SetTextSize(float size);
private:
	void Setting();
	void InputText(const std::string& str);
	void RemoveText();


	void ReviseFlashPosByMousePos();
	void ReviseFlashPosByChar();
	void FlashMove(const JGUIKeyDownEvent& e);
	void Reset();

private:
	JGUIPanel*     m_TextBackGround = nullptr;
	JGUIRectangle* m_SelectedBox    = nullptr;
	JGUIRectangle* m_Flash = nullptr;
	JGUIText*      m_Text  = nullptr;



	float         m_PressedTick = 0.0f;
	std::string   m_CurrentChar;
	int           m_CurrentFlashTextIndex = 0;
	float  m_TextSize = 16;
	float  m_GapRatio = 1.1f;



	float  m_FlashTick      = 0.0f;
	float  m_FlashTime      = 0.5f;
	float  m_FlashThickness = 2.5f;
	bool   m_FlashMove      = false;


	JVector2Int m_PrevMousePos;

	int m_SelectedStartTextIndex = INT_MAX;
	int m_SelectedEndTextIndex   = -2;
	bool     m_Selected = false;
};