#pragma once
#include "Element.h"


namespace GUI
{
	class GUI_API Button : public Element
	{
		enum BtState
		{
			BtState_Normal,
			BtState_Pressed,
			BtState_Selected,
			BtState_Hightlight,
			BtState_Disable,
			BtState_Count
		};
	protected:
		virtual void Awake()   override;
		virtual void Start()   override;
		virtual void Tick()    override;
		virtual void Destroy() override;

		virtual void OnMouseButtonDown(KeyCode bt) override;
		virtual void OnMouseButtonUp(KeyCode bt) override;
		virtual void OnMouseEnter() override;
		virtual void OnMouseExit() override;

		virtual void OnMouseClick(KeyCode bt) override;

		virtual void OnFocusExit() override;
	public:
		void BindOnClick(const std::function<void()>& func) {
			m_OnClick = func;
		}
		void SetText(const std::wstring& btText);
		void SetTextColor(const JColor& color);

		void SetAllImage(const std::wstring& path);
		void SetNormalImage(const std::wstring& path);
		void SetPressedImage(const std::wstring& path);
		void SetSelectedImage(const std::wstring& path);
		void SetHightlightImage(const std::wstring& path);
		void SetDisableImage(const std::wstring& path);

		const std::wstring& GetText() const;
		const JColor& GetTextColor() const;
	private:
		void ChangeState(BtState state);
	private:
		class ImageComponent*    m_Image = nullptr;
		class TextMeshComponent* m_TMCom = nullptr;
		class Text*              m_Text = nullptr;

		BtState      m_CurrState = BtState_Normal;
		BtState      m_PrevState = BtState_Count;
		JColor       m_BtColors[BtState_Count];
		std::wstring m_BtImages[BtState_Count];

		std::function<void()> m_OnClick;
	};
}