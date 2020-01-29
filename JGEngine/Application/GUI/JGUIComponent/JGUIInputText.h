#pragma once
#include "GUI/JGUIObject/JGUIComponent.h"



class JGUIText;
class JGUIRectangle;
class JGUIPanel;
class JGUIInputText : public JGUIComponent
{
	JGUIText*      m_Text       = nullptr;
	JGUIPanel*     m_BackGround = nullptr;
	JGUIRectangle* m_PosArrow   = nullptr;



};