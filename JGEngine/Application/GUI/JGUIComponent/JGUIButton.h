#pragma once



#include "GUI/JGUIObject/JGUIComponent.h"

class JGUIRectangle;
enum EJGUIBtState
{
	JGUI_BtState_Normal = 0,
	JGUI_BtState_Hightlight,
	JGUI_BtState_Pressed,
	JGUI_BtState_Selected,
	JGUI_BtState_Disabled,
	JGUI_BtState_Count
};
class JGUIButton : public JGUIComponent
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
public:
	void SetBtImage(const std::string& path);
	void SetBtColor(EJGUIBtState state, const JColor& color);
private:
	JGUIRectangle* m_Rectangle = nullptr;
	JColor         m_BtColor[JGUI_BtState_Count];
	EJGUIBtState   m_BtState = JGUI_BtState_Normal;
	bool           m_IsClick = false;
	std::string m_SourceImage = "none";
};