#pragma once

#include "JGUIComponent.h"

class JGUICanvas;
class JGENGINE_API JGUICollider : public JGUIComponent
{
public:
	virtual bool CheckInPoint(const JVector2Int& p);

protected:
	virtual void Awake() override;
	virtual void ChangeParent(JGUIObject* prev_parent) override;
	void FindCanvas();
protected:
	EJGUI_Collider m_ColiderType;
	JGUICanvas*    m_OwnerCanvas = nullptr;
};


class JGENGINE_API JGUIBoxCollider : public JGUICollider
{
protected:
	virtual void Awake() override;
public:
	virtual bool CheckInPoint(const JVector2Int& p) override;
protected:
	bool m_IsDebug = false;
};