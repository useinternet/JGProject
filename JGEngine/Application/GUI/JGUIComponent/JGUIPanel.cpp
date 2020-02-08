#include "pch.h"
#include "JGUIPanel.h"
#include "JGUIRectTransform.h"
#include "JGUIShape.h"
#include "JGUICollider.h"
#include "Object/ReObject/RenderItem.h"
#include "GUI/JGUIObject/JGUIWindow.h"

using namespace std;

void JGUIPanel::Awake()
{
	// RegisterCollider(JGUI_Component_Colider_Box, JGUI_ComponentInteractionFlag_None);
	m_Rect = CreateJGUIComponent<JGUIRectangle>("PanelRectangle");
	m_Rect->GetTransform()->SetSize(GetTransform()->GetSize());
	m_Rect->SetColor({ 0.7f,0.7f,0.7f, 0.3f });
}

void JGUIPanel::Resize(const JGUIResizeEvent& e)
{
	m_Rect->GetTransform()->SetSize(e.width, e.height);
}

void JGUIPanel::SetColor(const JColor& color)
{
	m_Rect->SetColor(color);
}

uint64_t JGUIPanel::GetPriority() const
{
	return m_Rect->GetPriority();
}




