#include "pch.h"
#include "Dropdown.h"
#include "Windows/Window.h"
#include "Components/DrawComponent.h"
#include "Components/LayerComponent.h"
#include "Components/Script/Border.h"
#include "Button.h"
using namespace std;



namespace JGUI
{
	void Dropdown::Awake()
	{
		GUIStyle& style = GUIIF::GetStyle();
		GetTransform()->SetSize(style.FieldWidth, style.FieldHeight);




		m_SelectedItem = GetOwner()->CreateElement<Element>(TT("SelectedItem"))->CreateComponent<DropdownItem>();
		m_SelectedItem->GetTransform()->SetSize(GetTransform()->GetSize());
		m_SelectedItem->GetTransform()->SetAnchor(AnchorPreset::Full_Stretch);



		m_ItemBundle = GetOwner()->CreateElement<Element>(TT("ItemBundle"), ElementFlag_TopMost);
		m_ItemBundle->GetTransform()->SetSize(GetTransform()->GetSize());
		m_ItemBundle->GetTransform()->SetAnchor(AnchorPreset::Full_Stretch);
		m_ItemBundle->CreateComponent<LayerComponent>();


		m_OpenBt = GetOwner()->CreateElement<Element>(TT("OpenButton"))->CreateComponent<Button>();
		m_OpenBt->GetOwner()->CreateComponent<ImageComponent>();
		m_OpenBt->GetTransform()->SetSize(style.FieldHeight, style.FieldHeight);
		m_OpenBt->GetTransform()->SetAnchor(AnchorPreset::Right_Top);
		m_OpenBt->GetTransform()->SetPivot(1.0f, 0.0f);
		m_OpenBt->BindOnClick([&]()
		{
			if (m_IsOpen) CloseItem();
			else OpenItem();
		});
		m_OpenBt->SetAllImage(style.GUIImages[GUIStyle_Resource_DownFillArrow]);
		auto openBtScript = m_OpenBt->GetOwner()->CreateComponent<NativeScriptComponent>();
		openBtScript->OnFocusExit = [&]()
		{
			m_IsFocusExit = true;
		};






		m_CheckImage = GetOwner()->CreateElement<Element>(TT("Check"))->CreateComponent<ImageComponent>();
		m_CheckImage->SetColor(JColor::Black());
		m_CheckImage->SetImage(style.GUIImages[GUIStyle_Resource_Check]);
		m_CheckImage->GetTransform()->SetSize(style.FieldHeight, style.FieldHeight);
		m_CheckImage->GetTransform()->SetAnchor(AnchorPreset::Left_Top);
		m_CheckImage->GetOwner()->SetActive(false);


		OnResize = [&](float width, float height)
		{
			m_OpenBt->GetTransform()->SetSize(height, height);
			m_CheckImage->GetTransform()->SetSize(height, height);
		};

		CloseItem();
	}

	void Dropdown::Start()
	{
	}

	void Dropdown::Tick()
	{
		if (m_IsFocusExit && !m_IsFocusMyItem)
		{
			CloseItem();
			m_IsFocusExit = false;
		}




	}

	void Dropdown::Destroy()
	{
	}

	void Dropdown::AddItem(const std::wstring& item)
	{
		size_t itemCnt = m_Items.size();
		auto size = GetTransform()->GetSize();

		auto dropdownItem = m_ItemBundle->CreateElement<Element>(item)->CreateComponent<DropdownItem>();
		dropdownItem->GetTransform()->SetSize(size);
		dropdownItem->GetTransform()->SetPivot(0.0f, (itemCnt + 1) * -1.0f);
		dropdownItem->GetTransform()->SetAnchor(AnchorPreset::Full_Stretch);
		m_Items.push_back(dropdownItem);
		dropdownItem->Owner = this;
		dropdownItem->Text->SetText(item);
	
		if (!m_IsOpen) dropdownItem->GetOwner()->SetActive(false);
	}

	void Dropdown::RemoveItem(const std::wstring& item)
	{
		auto iter = std::find_if(m_Items.begin(), m_Items.end(), 
			[&](DropdownItem* _item) -> bool
		{
			return _item->Text->GetText() == item;
		});
		if (iter != m_Items.end())
		{
			if (m_SelectedItem->Text->GetText() == item)
			{
				m_CheckImage->GetOwner()->SetActive(false);
				m_CheckImage->GetTransform()->SetParent(GetTransform());
				m_SelectedItem->Text->SetText(TT(""));
			}
			GetOwner()->DestroyElement((*iter)->GetOwner());
			m_Items.erase(iter);
		}
	}
	void Dropdown::RemoveItem(uint32_t idx)
	{
		DropdownItem* item = m_Items[idx];
		if (m_SelectedItem->Text->GetText() == item->Text->GetText())
		{
			m_CheckImage->GetOwner()->SetActive(false);
			m_CheckImage->GetTransform()->SetParent(GetTransform());
			m_SelectedItem->Text->SetText(TT(""));
		}
		GetOwner()->DestroyElement(item->GetOwner());
		m_Items.erase(m_Items.begin() + idx);
	}
	void Dropdown::RemoveAllItem()
	{
		m_CheckImage->GetOwner()->SetActive(false);
		m_CheckImage->GetTransform()->SetParent(GetTransform());
		for (auto& item : m_Items)
		{
			m_ItemBundle->DestroyElement(item->GetOwner());
		}

		m_Items.clear();
	}
	void Dropdown::ReleaseSelectItem()
	{
		m_CheckImage->GetOwner()->SetActive(false);
		m_CheckImage->GetTransform()->SetParent(GetTransform());
		m_SelectedItem->Text->SetText(TT(""));

	}
	void Dropdown::SelectItem(const std::wstring& item)
	{
		auto iter = std::find_if(m_Items.begin(), m_Items.end(),
			[&](DropdownItem* _item) -> bool
		{
			return _item->Text->GetText() == item;
		});
		if (iter != m_Items.end())
		{
			m_CheckImage->GetOwner()->SetActive(true);
			m_CheckImage->GetTransform()->SetParent((*iter)->GetTransform());
			m_SelectedItem->Text->SetText((*iter)->Text->GetText());
		}
	}
	void Dropdown::SelectItem(uint32_t idx)
	{
		if (m_Items.size() <= idx) return;
		m_CheckImage->GetOwner()->SetActive(true);
		m_CheckImage->GetTransform()->SetParent(m_Items[idx]->GetTransform());
		m_SelectedItem->Text->SetText(m_Items[idx]->Text->GetText());
	}
	const std::wstring& Dropdown::SelectedItem() const
	{
		return m_SelectedItem->Text->GetText();
	}
	int Dropdown::SelectedItemIndex() const
	{
		if (m_SelectedItem == nullptr) return -1;
		uint32_t size = m_Items.size();

		for (uint32_t i = 0; i < size; ++i)
		{
			if (m_Items[i]->Text->GetText() == m_SelectedItem->Text->GetText())
			{
				return i;
			}
		}
		return -1;
	}
	void Dropdown::OpenItem()
	{
		for (auto& item : m_Items)
		{
			item->GetOwner()->SetActive(true);
		}
		m_IsOpen = true;
	}
	void Dropdown::CloseItem()
	{
	
		for (auto& item : m_Items)
		{
			item->GetOwner()->SetActive(false);
		}
		m_IsOpen = false;
	}
}

namespace JGUI
{
	void Dropdown::DropdownItem::Awake()
	{
		GUIStyle& style = GUIIF::GetStyle();
		auto size = GetTransform()->GetSize();
		Image = GetOwner()->CreateComponent<ImageComponent>();
		Image->GetTransform()->SetSize(GetTransform()->GetSize());
		Image->GetTransform()->SetAnchor(AnchorPreset::Full_Stretch);


		Text = GetOwner()->CreateElement<Element>(TT("Text"))->CreateComponent<TextMeshComponent>();
		Text->GetTransform()->SetSize(GetTransform()->GetSize());
		Text->GetTransform()->SetAnchor(AnchorPreset::Full_Stretch);
		Text->SetTextVAlign(TextVAlign_Center);
		Text->SetColor(JColor::Black());

		_Border = GetOwner()->CreateElement<Element>(TT("Border"))->CreateComponent<Border>();
		_Border->GetOwner()->CreateComponent<ShapeComponent>()->SetColor(JColor::Black());
		_Border->SetThickness(0.5f);
		_Border->GetTransform()->SetSize(GetTransform()->GetSize());
		_Border->GetTransform()->SetAnchor(AnchorPreset::Full_Stretch);

		OnMouseButtonDown = [&](KeyCode code)
		{
			if (Owner == nullptr) return;
			Owner->m_IsFocusMyItem = true;
		};
		OnMouseButtonUp = [&](KeyCode code)
		{
			if (Owner == nullptr) return;
			Owner->m_IsFocusMyItem = false;
		};
		OnMouseClick = [&](KeyCode code)
		{
			if (code != KeyCode::LeftMouseButton) return;
			if (Owner == nullptr) return;
			Owner->SelectItem(Text->GetText());
			//Owner->CloseItem();
		};



		OnResize = [&](float width, float height)
		{
			if(Owner == nullptr) Text->GetTransform()->SetLocalLocation(10.0f, 0.0f);
			else  Text->GetTransform()->SetLocalLocation(height + 10.0f, 0.0f);
			
		};
	}

	void Dropdown::DropdownItem::Start()
	{
		

	}

	void Dropdown::DropdownItem::Tick()
	{
	}

	void Dropdown::DropdownItem::Destroy()
	{
	}

}