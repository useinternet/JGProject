#pragma once

#include "GUI/JGUIComponent/JGUIElementComponent.h"


class JGENGINE_API WindowSizeAdjustment : public JGUIScriptComponent
{

protected:
	virtual void Awake() override;
	virtual void Destroy() override;
	virtual void Tick(float tick) override;
public:
	void PositionAdjustment();
private:
	void Setting();
private:
	class JGUIImageRenderer* m_Image = nullptr;
	JVector2Int m_PrevCurosrPos;
	bool        m_IsResizing  = false;

};