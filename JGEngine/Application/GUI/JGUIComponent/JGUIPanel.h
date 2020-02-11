#pragma once


#include "GUI/JGUIObject/JGUIComponent.h"


class JGUIRectangle;
class JGUIPanel : public JGUIComponent
{
protected:
	virtual void Awake() override;
	virtual void Resize(const JGUIResizeEvent& e) override;
public:
	void SetColor(const JColor& color);
private:
	JGUIRectangle* m_Rect = nullptr;
};

