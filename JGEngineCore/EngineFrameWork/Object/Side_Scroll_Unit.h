#pragma once
#include"Unit.h"


class ENGINE_EXPORT Side_Scroll_Unit : public Unit
{
private:
	enum EFallingCheck
	{
		None,
		Down,
		Up
	};
	enum EDirectionCheck
	{
		Right,
		Left
	};
	bool m_bStill = false;
	bool m_bCompulsoryDirection = false;
public:
	Side_Scroll_Unit();
	virtual ~Side_Scroll_Unit();

	virtual void DefineDirection() override;
	virtual void DefineMove() override;

	bool IsStill();
	bool IsRight();
	bool IsLeft();
	bool IsFalling();
	bool IsFallingDown();
	bool IsFallingUp();
	void NotifyCompulsoryRight();
	void NotifyCompulsoryLeft();
	bool IsCompulsoryDirection();
	void NotifySolveCompulsoryDirection();
	void PushUp(const float ContantForce);


};