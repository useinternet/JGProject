#pragma once
#include "GUI/JGUIObject/JGUIComponent.h"


class JGUIRectangle;
class JGUICollider : public JGUIComponent
{
public:
	virtual void DebugOn();
	virtual void DebugOff();
	virtual bool CheckInPoint(const JVector2Int& p);

protected:

};


class JGUIBoxCollider : public JGUICollider
{
protected:
	virtual void Awake() override;
	virtual void Tick(const JGUITickEvent& e) override;
	virtual void Resize(const JGUIResizeEvent& e) override;
public:
	virtual void DebugOn() override;
	virtual void DebugOff() override;
	virtual bool CheckInPoint(const JVector2Int& p) override;
protected:
	JGUIRectangle* m_Rectangle = nullptr;
	bool m_IsDebug = false;
};
