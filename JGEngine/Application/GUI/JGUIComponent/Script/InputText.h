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

public:
	void SetBackGroundImage(const std::string& sourceImagePath);
	void SetFont(const std::string& fontname);
	void SetTextColor(const JColor& color);
	void SetSelectBlockColor(const JColor& color);

	const std::string& GetString() const;
	const std::string& GetBackGroundImage() const;
	const std::string& GetFontName() const;
	const JColor& GetTextColor() const;
	const JColor& GetSelectBlockColor() const;

	JGUIElement* GetTextArea() const {
		return m_TextArea;
	}
private:
	void  Setting();
	int   GetTextIndex(const JVector2& pos);
	float GetTextLocalPos(const uint32_t index);
	void  CalcSelectBox();
	void  ResetSelectBox();
private:

	JGUIImageRenderer* m_InputBackGround = nullptr;
	JGUIShapeRenderer* m_InputBackGroundBorder = nullptr;
	JGUIElement*       m_TextArea = nullptr;
	JGUIElement*       m_TextMask = nullptr;
	JGUIImageRenderer* m_IStick = nullptr;
	JGUIShapeRenderer* m_SelectBlock = nullptr;
	Text*       m_Text    = nullptr;
	std::string m_TextStr = "";


	//
	float m_FontSize = 18.0f;
	float m_InputGap = 2.0f;





	// Text
	float m_TextWidth = 1000.0f;


	// IStick
	float    m_IStickFlashTime = 0.5f;
	float    m_IStickFlashTick = 0.0f;
	uint32_t m_CurrentStickIndex = 1;

	// SelectBlock
	bool m_IsSelect = false;
	int  m_StartSelectIndex = -1;
	int  m_EndSelectIndex   = -1;


	// KeyInput

	float m_FirstInputDelay = 0.5f;
	float m_TickInputDelay  = 0.1f;
	float m_FirstKeyInputTick    = 0.0f;
	float m_KeyInputTick = 0.0f;
};