#pragma once
#include "Element.h"


namespace GUI
{
	class GUI_API TitleBar : public Element
	{

	protected:
		virtual void Awake()   override;
		virtual void Start()   override;
		virtual void Tick()    override;
		virtual void Destroy() override;
	public:
		void SetTitleName(const std::wstring& name);
	public:
		class ImageComponent*    m_Image = nullptr;
		class Text*              m_Text  = nullptr;
		class TextMeshComponent* m_TextComponent = nullptr;
		class LayerComponent* m_LayerComponent = nullptr;
	};
}