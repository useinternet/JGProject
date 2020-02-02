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
	m_ColiderType = JGUI_Component_Colider_Box;
}

void JGUIBoxCollider::DebugOn()
{
	m_IsDebug = true;
}
void JGUIBoxCollider::DebugOff()
{
	m_IsDebug = false;
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

void JGUIEmptyBoxColider::Awake()
{
	m_ColiderType = JGUI_Component_Colider_EmptyBox;
}

bool JGUIEmptyBoxColider::CheckInPoint(const JVector2Int& p)
{
	if (IsActive() == false) return false;
	JGUIRect rect = GetTransform()->GetRect();
	bool out_result = false;
	if (rect.left < p.x && rect.top < p.y &&
		rect.right > p.x && rect.bottom > p.y)
	{
		out_result = true;
	}

	bool in_result = false;
	if (rect.left + m_Thick < p.x && rect.top + m_Thick < p.y &&
		rect.right - m_Thick > p.x && rect.bottom - m_Thick > p.y)
	{
		in_result = true;
	}

	return (!in_result && out_result);
}
