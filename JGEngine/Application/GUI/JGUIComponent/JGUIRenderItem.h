#pragma once


#include "GUI/JGUIObject/JGUIComponent.h"



namespace RE
{
	class Texture;
	class RenderItem;
	class InstanceRenderItem;
	class FixedGShaderModuleClone;
}
class JGUIPanel;
class JGUIRenderItem : public JGUIComponent
{

public:

	virtual void SetActive(bool is_active) override;

protected:
	
	virtual void Destroy() override;
public:
	void RenderUpdate();
public:
	// void SetColor(const JColor& color);
	uint64_t GetRIPriority() const {
		return m_RIPriority;
	}
	void    SetRIPriority(uint64_t priority);
	void    SetDrawingWindow(JGUIWindow* win);
	RE::RenderItem* GetRI() {
		return m_RenderItem;
	}
	void SetOffset(JVector2 offset) {
		m_Offset = offset;
	}
	const JVector2& GetOffset() const {
		return m_Offset;
	}
protected:
	virtual void DestroyRI();
	virtual void CreateRI();

protected:
	RE::RenderItem*         m_RenderItem = nullptr;
	RE::InstanceRenderItem* m_Instance   = nullptr;
	JVector2                m_Offset = { 0,0 };

	JGUIWindow* m_DrawingWindow = nullptr;
	JVector2 m_PrevWindowSize;
	uint64_t m_RIPriority = 0;
	uint64_t m_RegisterID = 0;
};

