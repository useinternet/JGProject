#pragma once
#include "Components/NativeScriptComponent.h"




namespace JGUI
{
	class GUI_API Button : public NativeScriptComponent
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
	public:
		void BindOnClick(const std::function<void()>& func) {
			m_OnClick = func;
		}
		void SetAllImage(const std::wstring& path);
		void SetNormalImage(const std::wstring& path);
		void SetPressedImage(const std::wstring& path);
		void SetSelectedImage(const std::wstring& path);
		void SetHightlightImage(const std::wstring& path);
		void SetDisableImage(const std::wstring& path);
	private:
		void ChangeState(BtState state);
	private:
		class ImageComponent* m_Image = nullptr;

		BtState      m_CurrState = BtState_Normal;
		BtState      m_PrevState = BtState_Count;
		JColor       m_BtColors[BtState_Count];
		std::wstring m_BtImages[BtState_Count];

		std::function<void()> m_OnClick;
	};



}