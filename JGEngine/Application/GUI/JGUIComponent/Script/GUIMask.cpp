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
	if (m_MaskRenderer && GetTransform()->IsDirty())
	{
		m_MaskRenderer->SetClipFlags(JGUI_Clip_Flag_X | JGUI_Clip_Flag_Y);
		m_MaskRenderer->SetClipRect(GetTransform()->GetRect());
		m_MaskRenderer->GetTransform()->SendDirty();
	}


}

void GUIMask::Bind(JGUIRenderer* renderer)
{
	m_MaskRenderer = renderer;

}
