#include "pch.h"
#include "Slider.h"
#include "Components/DrawComponent.h"
#include "Windows/Window.h"
using namespace std;


namespace JGUI
{
	void Slider::Awake()
	{
		GUIStyle& style = GUIIF::GetStyle();
		JVector2 defaultSize = JVector2(style.FieldWidth, style.FieldHeight);
		GetTransform()->SetSize(defaultSize);

		m_NormalHandleColor = style.GUIColors[GUIStyle_SliderHandle];


		m_BackGround = GetOwner()->CreateElement<Element>(TT("BackGround"));
		m_BackGroundScript = m_BackGround->CreateComponent<NativeScriptComponent>();
		m_BackGroundImage  = m_BackGround->CreateComponent<ImageComponent>();
		m_BackGround->GetTransform()->SetSize(defaultSize);
		m_BackGround->GetTransform()->SetAnchor(AnchorPreset::Full_Stretch);
		m_BackGroundImage->SetColor(style.GUIColors[GUIStyle_SliderBackGround]);


		m_FillArea   = GetOwner()->CreateElement<Element>(TT("FillArea"));
		m_FillImage  = m_FillArea->CreateComponent<ImageComponent>();
		m_FillImage->SetColor(style.GUIColors[GUIStyle_SliderFillArea]);
		m_FillImage->SetInteractionActive(false);
		m_FillArea->GetTransform()->SetSize(0.0f, defaultSize.y);
		m_FillArea->GetTransform()->SetAnchor(AnchorPreset::Left_VStretch);




		m_Handle       = GetOwner()->CreateElement<Element>(TT("Handle"));
		m_HandleScript = m_Handle->CreateComponent<NativeScriptComponent>();
		m_HandleImage  = m_Handle->CreateComponent<ImageComponent>();
		m_HandleImage->SetColor(style.GUIColors[GUIStyle_SliderHandle]);
		m_Handle->GetTransform()->SetSize(10, defaultSize.y);
		//m_Handle->GetTransform()->SetPivot(0.5f, 0.5f);
		m_Handle->GetTransform()->SetAnchor(AnchorPreset::Left_VStretch);
		m_Handle->GetTransform()->SetLocalLocation(0, 0);
















		m_HandleScript->OnMouseButtonDown = [&](KeyCode code)
		{
			if (code != KeyCode::LeftMouseButton) return;
			m_HandlePressed = true;
			m_HandleImage->SetColor(m_PressedHandleColor);
			m_PrevMouseX = GetOwnerWindow()->GetMousePos().x;
		};
		m_HandleScript->OnMouseButtonUp = [&](KeyCode code)
		{
			if (code != KeyCode::LeftMouseButton) return;
			m_HandlePressed = false;
			m_HandleImage->SetColor(m_NormalHandleColor);
			m_PrevMouseX = 0.0f;
		};
		m_HandleScript->OnFocusExit = [&]()
		{
			m_HandlePressed = false;
		};

	}
	void Slider::Start()
	{
		SetValue(m_Value);

	}
	void Slider::Tick()
	{
		if (m_HandlePressed)
		{
			float mouseX     = GetOwnerWindow()->GetMousePos().x;
			JRect sliderRect = GetTransform()->GetRect();


			float delta = mouseX - sliderRect.left;
			float ratio = delta / sliderRect.Width();
			SetValue((m_MaxValue - m_MinValue) * ratio + m_MinValue);
		}
	}
	void Slider::Destroy()
	{
	}
	void Slider::SetValue(float value)
	{
		m_Value = value;
		m_Value = std::min<float>(m_MaxValue, m_Value);
		m_Value = std::max<float>(m_MinValue, m_Value);



		float ratio  = (m_Value - m_MinValue) / (m_MaxValue - m_MinValue);
		Adjust(ratio);
	}

	void Slider::SetHandleColor(const JColor& color)
	{
		m_NormalHandleColor = color;
		if (!m_HandlePressed)
		{
			m_HandleImage->SetColor(color);
		}

	}
	void Slider::SetPressedHandleColor(const JColor& color)
	{
		m_PressedHandleColor = color;
		if (m_HandlePressed)
		{
			m_HandleImage->SetColor(color);
		}
	}
	void Slider::SetBackGroundColor(const JColor& color)
	{
		m_BackGroundImage->SetColor(color);
	}
	void Slider::SetFillAreaColor(const JColor& color)
	{
		m_FillImage->SetColor(color);
	}
	const JColor& Slider::GetHandleColor() const {
		return m_NormalHandleColor;
	}
	const JColor& Slider::GetPressedHandleColor() const {
		return m_PressedHandleColor;
	}
	const JColor& Slider::GetBackGroundColor() const
	{
		return m_BackGroundImage->GetColor();
	}
	const JColor& Slider::GetFillAreaColor() const
	{
		return m_FillImage->GetColor();
	
	}
	void Slider::Adjust(float ratio)
	{
		JVector2 sliderSize = GetTransform()->GetSize();
		ratio = std::max<float>(ratio, 0.0f);
		ratio = std::min<float>(ratio, 1.0f);
		float localX = sliderSize.x * ratio;


		// 
		auto handleLocation = m_Handle->GetTransform()->GetLocalLocation();
		m_Handle->GetTransform()->SetLocalLocation(localX, handleLocation.y);

		auto fillAreaSize = m_FillArea->GetTransform()->GetSize();
		m_FillArea->GetTransform()->SetSize(localX, fillAreaSize.y);
	}
}
