#include "pch.h"
#include "Button.h"
#include "Text.h"
#include "Components/DrawComponent.h"
using namespace std;

namespace GUI
{
	void Button::Awake()
	{
		m_Image = CreateComponent<ImageComponent>(TT("ImageComponent"));
		m_Text  = CreateElement<Text>(TT("Text"));
		m_Text->GetTransform()->SetSize(GetTransform()->GetSize());
		m_Text->GetTransform()->SetAnchor(AnchorPreset::Full_Stretch);
		m_TMCom = m_Text->FindComponent<TextMeshComponent>();
		m_TMCom->SetTextHAlign(TextHAlign_Center);
		m_TMCom->SetTextVAlign(TextVAlign_Center);

		auto& style = GUIIF::GetStyle();

		m_BtColors[BtState_Normal]     = style.GUIColors[GUIStyle_ButtonNormal];
		m_BtColors[BtState_Hightlight] = style.GUIColors[GUIStyle_ButtonHightlight];
		m_BtColors[BtState_Pressed]    = style.GUIColors[GUIStyle_ButtonPressed];
		m_BtColors[BtState_Selected]   = style.GUIColors[GUIStyle_ButtonSelected];
		m_BtColors[BtState_Disable]    = style.GUIColors[GUIStyle_ButtonDisable];

		for (int i = 0; i < BtState_Count; ++i)
		{
			m_BtImages[i] = TT("none");
		}
		GetTransform()->SetSize(style.ButtonSize);
	}
	void Button::Start()
	{
		ChangeState(m_CurrState);
	}
	void Button::Tick()
	{

	}
	void Button::Destroy()
	{

	}
	void Button::OnMouseButtonDown(KeyCode bt)
	{
	
		if (m_CurrState == BtState_Disable) return;
		if (bt != KeyCode::LeftMouseButton) return;
		ChangeState(BtState_Pressed);
	}
	void Button::OnMouseButtonUp(KeyCode bt)
	{
		if (m_CurrState == BtState_Disable) return;
		if (bt != KeyCode::LeftMouseButton) return;
		ChangeState(BtState_Normal);

	}
	void Button::OnMouseEnter()
	{
		if (m_CurrState == BtState_Disable) return;
		ChangeState(BtState_Hightlight);
	}
	void Button::OnMouseExit()
	{
		if (m_CurrState == BtState_Disable) return;
		if (m_PrevState == BtState_Hightlight) ChangeState(BtState_Normal);
	}
	void Button::OnMouseClick(KeyCode bt)
	{
		if (m_CurrState == BtState_Disable) return;
		if (bt != KeyCode::LeftMouseButton) return;
		if (m_OnClick) m_OnClick();
		ChangeState(BtState_Selected);
	}
	void Button::OnFocusExit()
	{
		if (m_CurrState == BtState_Disable) return;
		ChangeState(BtState_Normal);
	}
	void Button::SetText(const std::wstring& btText)
	{
		m_TMCom->SetText(btText);
	}
	void Button::SetTextColor(const JColor& color)
	{
		m_TMCom->SetColor(color);
	}
	void Button::SetAllImage(const std::wstring& path)
	{
		for (int i = 0; i < BtState_Count; ++i)
		{
			m_BtImages[i] = path;
		}
		m_Image->SetImage(path);
	}
	void Button::SetNormalImage(const std::wstring& path)
	{
		m_BtImages[BtState_Normal] = path;
		if (m_CurrState == BtState_Normal)
			m_Image->SetImage(path);
	}
	void Button::SetPressedImage(const std::wstring& path)
	{
		m_BtImages[BtState_Pressed] = path;
		if (m_CurrState == BtState_Pressed)
			m_Image->SetImage(path);
	}
	void Button::SetSelectedImage(const std::wstring& path)
	{
		m_BtImages[BtState_Selected] = path;
		if (m_CurrState == BtState_Selected)
			m_Image->SetImage(path);
	}
	void Button::SetHightlightImage(const std::wstring& path)
	{
		m_BtImages[BtState_Hightlight] = path;
		if (m_CurrState == BtState_Hightlight)
			m_Image->SetImage(path);

	}
	void Button::SetDisableImage(const std::wstring& path)
	{
		m_BtImages[BtState_Disable] = path;
		if (m_CurrState == BtState_Disable)
			m_Image->SetImage(path);

	}
	const std::wstring& Button::GetText() const
	{
		return m_TMCom->GetText();
	}
	const JColor& Button::GetTextColor() const
	{
		return m_TMCom->GetColor();
	}
	void Button::ChangeState(BtState state)
	{
		m_CurrState = state;
		if (m_CurrState != m_PrevState)
		{
			m_Image->SetColor(m_BtColors[state]);
			m_Image->SetImage(m_BtImages[state]);
			m_PrevState = m_CurrState;
		}
	}
}