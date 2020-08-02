#include "pch.h"
#include "TitleBar.h"
#include "Components/DrawComponent.h"
#include "Components/LayerComponent.h"
#include "Elements/Text.h"
using namespace std;

namespace GUI
{
	void TitleBar::Awake()
	{
		m_LayerComponent = CreateComponent<LayerComponent>(TT("LayerComponent"));
		m_Image = CreateComponent<ImageComponent>(TT("ImageComponent"));
		m_Text = CreateElement<Text>(TT("TitleName"), ElementFlag_None);
		


		m_LayerComponent->SetLayer(GUIDraw_Priority_TitleBar);


		auto& style = GUIIF::GetStyle();
		m_Image->SetColor(style.GUIColors[GUIStyle_TitleBar]);
		m_Image->SetInteractionActive(false);





		m_TextComponent = m_Text->FindComponent<TextMeshComponent>();
		m_TextComponent->SetTextVAlign(TextVAlign_Center);
		m_TextComponent->SetColor(style.GUIColors[GUIStyle_TitleText]);


		
		m_Text->GetTransform()->SetLocalLocation(style.WindowPadding.x, 0.0f);
		m_Text->GetTransform()->SetSize(GetTransform()->GetSize());
		m_Text->GetTransform()->SetPivot(0.0f, 0.0f);
		m_Text->GetTransform()->SetAnchor(AnchorPreset::Full_Stretch);
	}

	void TitleBar::Start()
	{

	}

	void TitleBar::Tick()
	{


	}

	void TitleBar::Destroy()
	{


	}

	void TitleBar::SetTitleName(const std::wstring& name)
	{
		m_TextComponent->SetText(name);
	}

}