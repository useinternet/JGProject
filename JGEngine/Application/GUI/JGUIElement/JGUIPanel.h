#pragma once


#include "GUI/JGUIObject/JGUIElement.h"


class JGUIImage;
class JGUIPanel : public JGUIElement
{
protected:
	virtual void Awake() override;
	virtual void Resize(const JGUIResizeEvent& e) override;
public:
	void SetColor(const JColor& color);
private:
	JGUIImage* m_Rect = nullptr;
};

