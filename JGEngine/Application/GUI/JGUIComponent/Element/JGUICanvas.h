#pragma once


#include "GUI/JGUIComponent/JGUIElementComponent.h"


class JGENGINE_API JGUICanvas : public JGUIElementComponent
{
protected:
	virtual void Awake() override;
	virtual void Destroy() override;
	virtual void Tick(float tick) override;
public:
	uint64_t GetLayer() const {
		return m_Layer;
	}
	void SetLayer(uint64_t layer) {
		m_Layer = layer;
	}
	EJGUI_Canvas_Flags GetCanvasFlags() const {
		return m_CanvasFlags;
	}
	void SetCanvasFlags(EJGUI_Canvas_Flags flags) {
		m_CanvasFlags = flags;
	}
private:
	uint64_t           m_Layer = 0;
	EJGUI_Canvas_Flags m_CanvasFlags = JGUI_CanvasFlag_None;
};