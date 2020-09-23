#pragma once
#include "Core.h"



namespace JGUI
{
	class ImageComponent;
}


class Viewport : public JGUI::Form
{

protected:
	virtual void Awake()   override;
	virtual void Start()   override;
	virtual void Tick()    override;
	virtual void Destroy() override;
private:


	GE::Scene* m_WorldScene = nullptr;
	JGUI::ImageComponent* m_ViewportImage = nullptr;
};