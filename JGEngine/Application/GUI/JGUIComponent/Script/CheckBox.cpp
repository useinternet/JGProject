#include "pch.h"
#include "CheckBox.h"
#include "GUI/JGUIObject/JGUIElement.h"
#include "GUI/JGUIObject/JGUIWindow.h"

#include "GUI/JGUIComponent/Element/JGUIRenderer.h"

#include "GUI/JGUIComponent/Script/Button.h"
using namespace std;

void CheckBox::Awake()
{
	m_CheckBox = GetOwner()->CreateJGUIComponent<JGUIImageRenderer>();
	m_CheckBt  = GetOwner()->CreateJGUIComponent<Button>();
	m_Border = GetOwner()->CreateJGUIElement("Border")->CreateJGUIComponent<JGUIShapeRenderer>();
	m_Border->CreateEmptyRectangle(0.5f);

	m_Check    = GetOwner()->CreateJGUIElement("Check")->CreateJGUIComponent<JGUIImageRenderer>();
	m_Check->SetImage("Check.png");
	m_Check->SetColor(JColor::Black());
	m_Check->GetOwner()->SetActive(false);
	
	m_Border->SetColor(JColor::Black());
	
	GetTransform()->AttachTransform(m_Check->GetTransform());
	GetTransform()->AttachTransform(m_Border->GetTransform());
	m_CheckBt->BindOnClick([&]()
	{
		if (m_BoolPtr == nullptr) return;
		if (*m_BoolPtr)
		{
			*m_BoolPtr = false;
		}
		else
		{
			*m_BoolPtr = true;
		}

	});

}

void CheckBox::Start()
{

}

void CheckBox::Destroy()
{
	m_BoolPtr = nullptr;
}

void CheckBox::Tick(float tick)
{
	if (m_BoolPtr == nullptr) return;
	bool boolvalue = *m_BoolPtr;
	if (!boolvalue && m_Check->GetOwner()->IsActive())
	{
		m_Check->GetOwner()->SetActive(false);
	}
	if(boolvalue && !m_Check->GetOwner()->IsActive())
	{
		m_Check->GetOwner()->SetActive(true);
	}
}

void CheckBox::SetBool(bool* bool_ptr)
{
	m_BoolPtr = bool_ptr;

}
