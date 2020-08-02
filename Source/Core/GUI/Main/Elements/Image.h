#pragma once


#include "Element.h"



namespace GUI
{
	class GUI_API Image : public Element
	{
	protected:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Tick()  override;
		virtual void Destroy()  override;
	public:
		void SetImage(const std::wstring& path);
		void SetImage(GE::Texture* t);
		void SetColor(const JColor& color);


		const JColor& GetColor() const;
	private:
		class ImageComponent* m_ImageCom = nullptr;
	};
}