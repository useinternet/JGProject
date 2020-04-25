#pragma once


#include "JGUIComponent.h"


class JGUIObject;
class JGUIWindow;
class JGUIWindowComponent : public JGUIComponent
{
	friend JGUIObject;
private:
	virtual void JGUIAwake() override;
	virtual void JGUIStart() override;
	virtual void JGUIDestroy() override;
	virtual void JGUITick(float tick) override;
	virtual void JGUIChangeParent(JGUIObject* prevParent) override;
public:
	JGUIWindow* GetOwner() const;
};