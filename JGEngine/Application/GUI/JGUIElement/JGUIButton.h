#pragma once



#include "GUI/JGUIObject/JGUIElement.h"

class JGUIImage;
enum EJGUIBtState
{
	JGUI_BtState_Normal = 0,
	JGUI_BtState_Hightlight,
	JGUI_BtState_Pressed,
	JGUI_BtState_Selected,
	JGUI_BtState_Disabled,
	JGUI_BtState_Count
};


class JGUIButton : public JGUIElement
{
protected:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Resize(const JGUIResizeEvent& e) override;
	virtual void MouseBtUp(const JGUIKeyUpEvent& e) override;
	virtual void MouseBtDown(const JGUIKeyDownEvent& e) override;
	virtual void MouseMove(const JGUIMouseMoveEvent& e) override;
	virtual void MouseLeave() override;
	virtual void Tick(const JGUITickEvent& e) override;
	virtual void OnClick() {
	
		if (m_OnClick)
		{
			ENGINE_LOG_INFO("OnClick");
			m_OnClick();
		}
	}
public:
	void SetBtImage(const std::string& path);
	void SetBtColor(EJGUIBtState state, const JColor& color);
	JColor GetBtColor(EJGUIBtState state);
	void BindOnClick(const std::function<void()>& func);
	void Reset();
private:
	JGUIImage*     m_Image = nullptr;
	JColor         m_BtColor[JGUI_BtState_Count];
	EJGUIBtState   m_BtState = JGUI_BtState_Normal;
	bool           m_IsClick = false;
	std::function<void()> m_OnClick;
	std::string m_SourceImage = "none";
};


class JGUICloseButton : public JGUIButton
{
protected:
	virtual void Start() override;
protected:
	virtual void OnClick() override;

};

class JGUIMaximizeButton : public JGUIButton
{
protected:
	virtual void Start() override;
protected:
	virtual void OnClick() override;
};


class JGUIMinimizeButton : public JGUIButton
{
protected:
	virtual void Start() override;
protected:
	virtual void OnClick() override;

};