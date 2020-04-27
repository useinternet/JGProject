#pragma once
#include "GUI/JGUIComponent/JGUIElementComponent.h"



class JGENGINE_API ContextMenu : public JGUIScriptComponent
{


protected:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Destroy() override;
	virtual void Tick(float tick) override;



};