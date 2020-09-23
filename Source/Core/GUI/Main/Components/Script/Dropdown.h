#pragma once
#include "Components/NativeScriptComponent.h"




namespace JGUI
{
	class ImageComponent;
	class TextMeshComponent;
	class Button;
	class Border;
	class GUI_API Dropdown : public NativeScriptComponent
	{
	protected:
		virtual void Awake()   override;
		virtual void Start()   override;
		virtual void Tick()    override;
		virtual void Destroy() override;
		
	public:
		void AddItem(const std::wstring& item);
		void RemoveItem(const std::wstring& item);
		void RemoveItem(uint32_t idx);
		void RemoveAllItem();
		void ReleaseSelectItem();
		void SelectItem(const std::wstring& item);
		void SelectItem(uint32_t idx);
		const std::wstring& SelectedItem() const;
		int SelectedItemIndex() const;
		void OpenItem();
		void CloseItem();
	private:
		class DropdownItem : public NativeScriptComponent
		{
			friend Dropdown;
		protected:
			virtual void Awake()   override;
			virtual void Start()   override;
			virtual void Tick()    override;
			virtual void Destroy() override;
		public:
			Dropdown*          Owner = nullptr;
			Border*            _Border = nullptr;
			ImageComponent*    Image = nullptr;
			TextMeshComponent* Text = nullptr;
		};
	private:

	private:
		ImageComponent* m_CheckImage   = nullptr;
		DropdownItem*   m_SelectedItem = nullptr;
		Button*         m_OpenBt       = nullptr;
		Element*        m_ItemBundle   = nullptr;
		std::vector<DropdownItem*> m_Items;

		// 포커스가 나가고, 
		bool m_IsOpen       = true;
		bool m_IsFocusExit   = false;
		bool m_IsFocusMyItem = false;
	
	};


}