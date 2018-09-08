#include"Button.h"
#include"../../Components/StaticMesh2DComponent.h"
using namespace std;
Button::Button()
{
	RegisterComponentID(typeid(this));
}

Button::~Button()
{
}

void Button::CreateButton(const wstring& OrdinaryImagePath, const wstring& MouseButtonDownPath, const wstring& PutMouseImagePath)
{
	m_OrdinaryImage = make_unique<StaticMesh2DComponent>();
	AddChild(m_OrdinaryImage.get());
	m_OrdinaryImage->RegisterName(GetComponentName() + TT("_Ordinary"));

	if (MouseButtonDownPath != TT("None"))
	{
		m_MouseButtonDownImage = make_unique<StaticMesh2DComponent>();
		AddChild(m_MouseButtonDownImage.get());
		m_MouseButtonDownImage->RegisterName(GetComponentName() + TT("_MouseButtonDown"));
	}
	if (PutMouseImagePath != TT("None"))
	{
		m_PutMouseImage = make_unique<StaticMesh2DComponent>();
		AddChild(m_PutMouseImage.get());
		m_PutMouseImage->RegisterName(GetComponentName() + TT("_PutMouse"));
	}
}

void Button::SetBlend(const float a, EButtonImageType buttonType)
{
	switch (buttonType)
	{
	case EButtonImageType::Ordinary:
		m_OrdinaryImage->SetAlphaBlend(a);
		break;
	case EButtonImageType::MouseButtonDown:
		m_MouseButtonDownImage->SetAlphaBlend(a);
		break;
	case EButtonImageType::PutMouse:
		m_PutMouseImage->SetAlphaBlend(a);
		break;
	}
}

