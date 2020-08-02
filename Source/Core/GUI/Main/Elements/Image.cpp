#include "pch.h"
#include "Image.h"
#include "Components/DrawComponent.h"


using namespace std;


namespace GUI
{
	void Image::Awake()
	{
		m_ImageCom = CreateComponent<ImageComponent>(TT("ImageComponent"));
	}
	void Image::Start()
	{
	}
	void Image::Tick()
	{
	}
	void Image::Destroy()
	{
	}
	void Image::SetImage(const std::wstring& path)
	{
		m_ImageCom->SetImage(path);
	}
	void Image::SetImage(GE::Texture* t)
	{
		m_ImageCom->SetImage(t);
	}
	void Image::SetColor(const JColor& color)
	{
		m_ImageCom->SetColor(color);
	}
	const JColor& Image::GetColor() const
	{
		return m_ImageCom->GetColor();
	}
}

