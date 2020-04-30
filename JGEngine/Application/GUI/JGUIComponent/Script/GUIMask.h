#pragma once
#include "GUI/JGUIComponent/JGUIElementComponent.h"


class JGUIRenderer;
class GUIMask : public JGUIScriptComponent
{
protected:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Destroy() override;
	virtual void Tick(float tick) override;
public:
	void Bind(JGUIRenderer* renderer);
private:
	JGUIRenderer* m_MaskRenderer = nullptr;
};