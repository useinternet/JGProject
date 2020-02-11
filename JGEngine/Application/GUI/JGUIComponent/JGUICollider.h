#pragma once
#include "GUI/JGUIObject/JGUIComponent.h"


class JGUIRectangle;
class JGUICollider : public JGUIComponent
{
	friend class JGUIObject;
protected:
	EJGUI_Colider m_ColiderType;
public:
	virtual void DebugOn();
	virtual void DebugOff();
	virtual bool CheckInPoint(const JVector2Int& p);

	EJGUI_Colider GetColiderType() const {
		return m_ColiderType;
	}
};


class JGUIBoxCollider : public JGUICollider
{
protected:
	virtual void Awake() override;
public:
	virtual void DebugOn() override;
	virtual void DebugOff() override;
	virtual bool CheckInPoint(const JVector2Int& p) override;
protected:
	bool m_IsDebug = false;
};


class JGUIEmptyBoxColider : public JGUICollider
{
protected:
	virtual void Awake() override;
public:
	void SetThick(float thick) { m_Thick = thick; }

public:
	virtual bool CheckInPoint(const JVector2Int& p);
private:
	float m_Thick = 0.0f;
};


