#include "pch.h"
#include "InputText.h"
#include "GUI/JGUIObject/JGUIElement.h"
#include "GUI/JGUIObject/JGUIWindow.h"

#include "GUI/JGUIComponent/Element/JGUIRenderer.h"

#include "GUI/JGUIComponent/Script/Text.h"
#include "GUI/JGUIComponent/Script/GUIMask.h"
using namespace std;

void InputText::Awake()
{
	m_InputBackGround       = GetOwner()->CreateJGUIComponent<JGUIImageRenderer>();
	m_InputBackGroundBorder = GetOwner()->CreateJGUIElement("InputBackGroundBorder")->CreateJGUIComponent<JGUIShapeRenderer>();
	m_TextArea              = GetOwner()->CreateJGUIElement("TextArea");
	
	//m_TextArea->CreateJGUIComponent<JGUIImageRenderer>()->SetColor(JColor(0.5f,0.5f,0.5f,1.0f));
	m_Text                  = m_TextArea->CreateJGUIElement("Text")->CreateJGUIComponent<Text>();
	m_TextArea->CreateJGUIComponent<GUIMask>()->Bind(m_Text);
	m_IStick                = m_Text->GetOwner()->CreateJGUIElement("InputStick")->CreateJGUIComponent<JGUIImageRenderer>();
	m_SelectBlock           = m_Text->GetOwner()->CreateJGUIElement("SelectBlock")->CreateJGUIComponent<JGUIShapeRenderer>();



	m_InputBackGroundBorder->CreateEmptyRectangle(0.5f);
	GetTransform()->AttachTransform(m_InputBackGroundBorder->GetTransform());


	m_IStick->SetColor(JColor::Black());
	m_IStick->GetOwner()->SetActive(false);

	m_SelectBlock->SetColor(JColor::Blue());
	GetOwner()->BindResizeFunc([&](const JGUIResizeEvent& e)
	{
		Setting();

	});


	m_TextArea->RegisterCollider(JGUI_Collider_Box);


	m_TextArea->BindOnFocusFunc([&]()
	{
		m_IStickFlashTick += GetTick();
		if (m_IStickFlashTick >= m_IStickFlashTime)
		{
			m_IStickFlashTick = 0.0f;
			if (m_IStick->GetOwner()->IsActive()) m_IStick->GetOwner()->SetActive(false);
			else m_IStick->GetOwner()->SetActive(true);
		}

		//  왼쪽 이나 오른쪽으로 벗어날 시 
		//  그 차이 만큼 텍스트 이동
		//  파란색 상자는 
	});

	m_TextArea->BindFocusEnterFunc([&](const JGUIFocusEnterEvent& e)
	{
		m_IStick->GetOwner()->SetActive(true);

	});

	m_TextArea->BindFocusExitFunc([&](const JGUIFocusExitEvent& e)
	{
		m_IStickFlashTick = 0.0f;
		m_IStick->GetOwner()->SetActive(false);
	});

	m_TextArea->BindCharFunc([&](const JGUICharEvent& e)
	{

		if (e.str == "\r")
		{
			return;
		}
		// 지우기
		else if (e.str == "\b")
		{
			if (m_TextStr.empty()) return;
			m_TextStr.pop_back();
		}
		else
		{
			m_TextStr += e.str;
		}
		m_Text->SetText(m_TextStr);

		float width = m_Text->GetTextWidth();


		m_IStick->GetTransform()->SetLocalPosition(width, 0.0f);


		auto area_size = m_TextArea->GetTransform()->GetSize();

		if (width > area_size.x)
		{
			float delta = width - area_size.x;
			m_Text->GetTransform()->SetLocalPosition(-delta, 0.0f);
		}
		else
		{
			m_Text->GetTransform()->SetLocalPosition(0.0f, 0.0f);
		}

	});

	m_TextArea->BindMouseBtDownFunc([&](const JGUIKeyDownEvent& e)
	{
		if (!m_IsSelect && e.Code == KeyCode::LeftMouseButton)
		{
			 
			m_IsSelect = true;

		}
	});


	Setting();
	m_Text->GetTransform()->SetSize(m_TextWidth, m_FontSize);
	m_Text->SetColor(JColor::Black());
}

void InputText::Start()
{
}

void InputText::Destroy()
{
}

void InputText::Tick(float tick)
{
}

void InputText::Setting()
{
	auto size = GetTransform()->GetSize();

	m_FontSize = size.y - (m_InputGap * 2);

	m_TextArea->GetTransform()->SetSize(size.x - (2 * m_InputGap), m_FontSize);
	m_TextArea->GetTransform()->SetLocalPosition(m_InputGap, m_InputGap);


	m_IStick->GetTransform()->SetSize(1.0f, m_FontSize);
	m_SelectBlock->GetTransform()->SetSize(0.0f, m_FontSize);
	m_Text->SetFontSize(m_FontSize);
}

int InputText::GetTextPos(const JVector2& pos)
{
	JVector2 start_pos = m_Text->GetTransform()->GetLocalPosition();







	return 0;
}
