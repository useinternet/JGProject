#include "pch.h"
#include "JGUICollider.h"
#include "JGUIRectTransform.h"
#include "JGUIShape.h"
#include "JGUIPanel.h"
#include "GUI/JGUIObject/JGUIWindow.h"

using namespace std;

void JGUICollider::DebugOn()  { }
void JGUICollider::DebugOff() { }

bool JGUICollider::CheckInPoint(const JVector2Int& p)
{
	return false;
}



void JGUIBoxCollider::Awake()
{
	m_Rectangle = CreateJGUIComponent<JGUIRectangle>("JGUIRectangle");
	m_Rectangle->SetColor({ 0.0f, 1.0f,0.0f, 1.0f });
	m_Rectangle->SetActive(false);
}
void JGUIBoxCollider::Tick(const JGUITickEvent& e)
{
	if(m_IsDebug)
	{
		if (CheckInPoint(JGUI::GetMousePos(GetOwnerWindow()->GetRootWindowHandle())))
		{
			m_Rectangle->SetColor({ 1.0f,0.0f,0.0f,0.5f });
		}
		else
		{
			m_Rectangle->SetColor({ 0.0f, 1.0f,0.0f, 0.5f });
		}
	}
}
void JGUIBoxCollider::Resize(const JGUIResizeEvent& e)
{
	m_Rectangle->GetTransform()->SetSize(e.width, e.height);
}
void JGUIBoxCollider::DebugOn()
{
	m_IsDebug = true;
	m_Rectangle->SetActive(true);
}
void JGUIBoxCollider::DebugOff()
{
	m_IsDebug = false;
	m_Rectangle->SetActive(false);
}
bool JGUIBoxCollider::CheckInPoint(const JVector2Int& p)
{
	if (IsActive() == false) return false;

	JGUIRect rect = GetTransform()->GetRect();

	if (rect.left < p.x  && rect.top < p.y &&
		rect.right > p.x && rect.bottom > p.y)
	{
		return true;
	}
	return false;

}