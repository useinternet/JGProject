#pragma once


#include "GUI/JGUIObject/JGUIComponent.h"



namespace RE
{
	class RenderItem;
	class InstanceRenderItem;
}
class JGUIPanel;
class JGUIShape : public JGUIComponent
{

protected:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Tick(const JGUITickEvent& e) override;
	virtual void Destroy() override;
	virtual void Resize(const JGUIResizeEvent& e) override;
public:
	virtual void SetActive(bool active) override;
public:
	virtual void SetParent(JGUIComponent* parent) override;


	void SetColor(const JColor& color);
protected:
	void DestroyRI();
	void FindPanel();
	virtual void CreateRI();
protected:
	JGUIPanel*              m_OwnerPanel = nullptr;
	RE::RenderItem*         m_RenderItem = nullptr;
	RE::InstanceRenderItem* m_Instance   = nullptr;
	JColor                  m_Color = { 1.0f,1.0f,1.0f,1.0f };



	JVector2 m_PrevWindowSize;
	uint64_t m_Priority = 0;
};

class JGUIRectangle : public JGUIShape
{
public:
	void SetImage(const std::string& texture_image);
protected:
	virtual void CreateRI() override;


private:
	bool m_isImage = false;
};
