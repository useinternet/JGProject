#pragma once
#include "GUI/JGUIComponent/JGUIElementComponent.h"

class JGUIImageRenderer;
class JGUIShapeRenderer;
class Text;
class JGENGINE_API InputText : public JGUIScriptComponent
{
protected:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Destroy() override;
	virtual void Tick(float tick) override;



private:
	void Setting();
	int  GetTextPos(const JVector2& pos);
private:

	JGUIImageRenderer* m_InputBackGround = nullptr;
	JGUIShapeRenderer* m_InputBackGroundBorder = nullptr;
	JGUIElement*       m_TextArea = nullptr;
	JGUIElement*       m_TextMask = nullptr;
	JGUIImageRenderer* m_IStick = nullptr;
	JGUIShapeRenderer* m_SelectBlock = nullptr;
	Text* m_Text = nullptr;
	std::string m_TextStr = "";
	//
	float m_FontSize = 18.0f;
	float m_InputGap = 2.0f;





	// Text
	float m_TextWidth = 1000.0f;


	// IStick
	float m_IStickFlashTime = 0.5f;
	float m_IStickFlashTick = 0.0f;


	// SelectBlock
	bool m_IsSelect = false;
	int  m_StartSelectIndex = 0;
	int  m_EndSelectIndex   = 0;

};