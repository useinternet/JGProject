#include "pch.h"
#include "Button.h"
#include "Components/DrawComponent.h"
using namespace std;


namespace JGUI
{

	void Button::Awake()
	{
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
		m_Image = GetOwner()->FindComponent<ImageComponent>();
		ChangeState(m_CurrState);
		OnMouseButtonDown = [&](KeyCode bt)
		{
			if (m_CurrState == BtState_Disable) return;
			if (bt != KeyCode::LeftMouseButton) return;
			ChangeState(BtState_Pressed);
		};
		OnMouseButtonUp = [&](KeyCode bt)
		{
			if (m_CurrState == BtState_Disable) return;
			if (bt != KeyCode::LeftMouseButton) return;
			ChangeState(BtState_Normal);
		};
		OnMouseEnter = [&]()
		{
			if (m_CurrState == BtState_Disable) return;
			if (m_CurrState == BtState_Pressed) return;
			ChangeState(BtState_Hightlight);
		};
		OnMouseExit = [&]()
		{
			if (m_CurrState == BtState_Disable) return;
			if (m_PrevState == BtState_Hightlight) ChangeState(BtState_Normal);
		};
		OnMouseClick = [&](KeyCode bt)
		{
			if (m_CurrState == BtState_Disable) return;
			if (bt != KeyCode::LeftMouseButton) return;
			if (m_OnClick) m_OnClick();
			ChangeState(BtState_Selected);
		};
		OnFocusExit = [&]()
		{
			if (m_CurrState == BtState_Disable) return;
			ChangeState(BtState_Normal);
		};

	}
	void Button::Tick()
	{
	
	}
	void Button::Destroy()
	{

	}

	void Button::SetAllImage(const std::wstring& path)
	{
		for (int i = 0; i < BtState_Count; ++i)
		{
			m_BtImages[i] = path;
		}
		if (m_Image == nullptr) return;
		m_Image->SetImage(path);
	}
	void Button::SetNormalImage(const std::wstring& path)
	{
		m_BtImages[BtState_Normal] = path;
		if (m_Image == nullptr) return;
		if (m_CurrState == BtState_Normal)
			m_Image->SetImage(path);
	}
	void Button::SetPressedImage(const std::wstring& path)
	{
		m_BtImages[BtState_Pressed] = path;
		if (m_Image == nullptr) return;
		if (m_CurrState == BtState_Pressed)
			m_Image->SetImage(path);
	}
	void Button::SetSelectedImage(const std::wstring& path)
	{
		m_BtImages[BtState_Selected] = path;
		if (m_Image == nullptr) return;
		if (m_CurrState == BtState_Selected)
			m_Image->SetImage(path);
	}
	void Button::SetHightlightImage(const std::wstring& path)
	{
		m_BtImages[BtState_Hightlight] = path;
		if (m_Image == nullptr) return;
		if (m_CurrState == BtState_Hightlight)
			m_Image->SetImage(path);

	}
	void Button::SetDisableImage(const std::wstring& path)
	{
		m_BtImages[BtState_Disable] = path;
		if (m_Image == nullptr) return;
		if (m_CurrState == BtState_Disable)
			m_Image->SetImage(path);

	}
	void Button::ChangeState(BtState state)
	{
		if (m_Image == nullptr) return;
		m_CurrState = state;
		if (m_CurrState != m_PrevState)
		{
			m_Image->SetColor(m_BtColors[state]);
			m_Image->SetImage(m_BtImages[state]);


			m_PrevState = m_CurrState;
		}
	}
}