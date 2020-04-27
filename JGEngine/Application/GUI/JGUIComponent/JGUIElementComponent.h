#pragma once
#include "JGUIComponent.h"
#include "GUI/JGUIComponent/JGUITransform.h"


class JGUIObject;
class JGUIElement;
class JGUIWindow;
class JGENGINE_API JGUIElementComponent : public JGUIComponent
{
	friend JGUIObject;
private:
	virtual void JGUIAwake() override;
	virtual void JGUIStart() override;
	virtual void JGUIDestroy() override;
	virtual void JGUITick(float tick) override;
	virtual void JGUIChangeParent(JGUIObject* prevParent) override;
public:
	JGUIElement* GetOwner() const;
	JGUIWindow*  GetOwnerWindow() const;
};


class JGENGINE_API JGUIScriptComponent : public JGUIElementComponent
{
	friend JGUIElement;
private:
};