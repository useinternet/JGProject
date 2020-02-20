#pragma once



#include "GUI/JGUIObject/JGUIComponent.h"


class JGUIRectangle;
class JGUIResizeBox : public JGUIComponent
{
	enum EResizeDirection
	{
		UP_NS = 4,
		DOWN_NS = 8,
		RIGHT_WE = 2,
		LEFT_WE = 1,
		RIGHTUP_NE = 6,
		RIGHTDOWN_SE = 10,
		LEFTUP_NW = 5,
		LEFTDOWN_SW = 9
	};

public:
	bool  IsResizing() const { return m_Resizing; }
	void  PositionAdjustment();
protected:
	virtual void Awake() override;
	virtual void Tick(const JGUITickEvent& e) override;
	virtual void Resize(const JGUIResizeEvent& e) override;
	virtual void MouseBtDown(const JGUIKeyDownEvent& e) override;
private:
	void CheckCursorDirection();
	void ResizeByDirection();
private:
	// 콜라이더 2개
	JGUIRectangle*      m_Rectangle = nullptr;
	bool                m_Resizing = false;
	EResizeDirection    m_Direction;
	JVector2Int         m_TempMousePos;

	float m_Thick = 1.5f;
	float m_Gap   = 10.0f;
};

