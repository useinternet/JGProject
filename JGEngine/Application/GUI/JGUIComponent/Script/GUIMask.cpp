#include "pch.h"
#include "GUIMask.h"

#include "GUI/JGUIObject/JGUIElement.h"
#include "GUI/JGUIObject/JGUIWindow.h"

#include "GUI/JGUIComponent/Element/JGUIRenderer.h"

void GUIMask::Awake()
{

}

void GUIMask::Start()
{



}

void GUIMask::Destroy()
{
}

void GUIMask::Tick(float tick)
{
	if (!m_MaskRenderers.empty() && GetTransform()->IsDirty())
	{
		for (auto& renderer : m_MaskRenderers)
		{
			renderer->SetClipFlags(JGUI_Clip_Flag_X | JGUI_Clip_Flag_Y);
			renderer->SetClipRect(GetTransform()->GetRect());
			renderer->GetTransform()->SendDirty();
		}



	}


}

void GUIMask::Bind(JGUIRenderer* renderer)
{
	m_MaskRenderers.clear();
	m_MaskRenderers.push_back(renderer);

}

void GUIMask::Bind(JGUIElement* element)
{
	m_MaskRenderers.clear();

	TracingImageRenderer(element);
}

void GUIMask::TracingImageRenderer(JGUIElement* element)
{
	JGUIRenderer* renderer = element->FindComponent<JGUIImageRenderer>();
	if(renderer == nullptr)  renderer = element->FindComponent<JGUIShapeRenderer>();
	if (renderer)
	{
		m_MaskRenderers.push_back(renderer);
	}

	for (uint32_t i = 0; i < element->GetChildCount(); ++i)
	{
		TracingImageRenderer(element->GetChild(i));
	}



}
