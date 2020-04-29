#include "pch.h"
#include "MainMenu.h"
#include "GUI/JGUIComponent/Element/JGUIRenderer.h"
#include "GUI/JGUIComponent/Element/JGUICanvas.h"

#include "GUI/JGUIObject/JGUIElement.h"
#include "GUI/JGUIObject/JGUIWindow.h"

#include "GUI/JGUIComponent/Script/Text.h"
#include "GUI/JGUIComponent/Script/MenuItem.h"
using namespace std;
JGUI_REGISTER_COMPONENT(MainMenu)
JGUI_REGISTER_COMPONENT(MainMenuItem)
void MainMenu::Awake()
{

	m_MainMenuItems = GetOwner()->CreateJGUIElement("MainMenuItems");
	auto canvas = m_MainMenuItems->CreateJGUIComponent<JGUICanvas>();
	canvas->SetCanvasFlags(JGUI_CanvasFlag_Overlay);
	m_Image = m_MainMenuItems->CreateJGUIComponent<JGUIImageRenderer>();
	m_Image->SetColor(MenuItem::HightlightColor);
	
	
	GetTransform()->AttachTransform(m_MainMenuItems->GetTransform());
	GetOwner()->BindResizeFunc([&](const JGUIResizeEvent& e)
	{
		Setting();
	});
}

void MainMenu::Start()
{
	
	Setting();


}

void MainMenu::Destroy()
{

}

void MainMenu::Tick(float tick)
{
	
	if (JGUI::GetKeyDown(GetOwnerWindow(), KeyCode::LeftMouseButton))
	{
		MainMenuItem* openItem = nullptr;

		for (int i = 0; i < GetItemCount(); ++i)
		{
			if (GetItem(i)->m_ItemCollection->IsOpenItems())
			{
				openItem = GetItem(i);
				break;
			}
		}
		if (openItem == nullptr) return;


		bool is_check = false;

		uint32_t itemCount = openItem->GetItemCount();

		for (int i = 0; i < itemCount; ++i)
		{
			is_check = CheckPoint(openItem->GetItem(i));
			if (is_check)
			{
				break;
			}
		}

		if (is_check) return;
		// 


		openItem->CloseItems();
	}


}

MainMenuItem* MainMenu::AddItem(const MainMenuItemDesc& desc)
{
	auto item = m_MainMenuItems->CreateJGUIElement(desc.name)->CreateJGUIComponent<MainMenuItem>();
	item->m_Desc = desc;
	item->Setting();

	return item;
}
MainMenuItem* MainMenu::GetItem(uint32_t idx) const
{
	return m_MainMenuItems->GetChild(idx)->FindComponent<MainMenuItem>();
}
uint32_t      MainMenu::GetItemCount() const
{
	return m_MainMenuItems->GetChildCount();
}
void MainMenu::ReceiveKeyDown(const JGUIKeyDownEvent& e)
{
	uint32_t itemCount = GetItemCount();
	for (uint32_t i = 0; i < itemCount; ++i)
	{
		GetItem(i)->ReceiveAuxKeyDown(e);
	}


}
void MainMenu::ReceiveKeyUp(const JGUIKeyUpEvent& e)
{
	uint32_t itemCount = GetItemCount();
	for (uint32_t i = 0; i < itemCount; ++i)
	{
		GetItem(i)->ReceiveAuxKeyUp(e);
	}


}
void MainMenu::Setting()
{
	
	float offset_x = 0.0f;
	uint32_t itemCount = GetItemCount();
	for (uint32_t i = 0; i < itemCount; ++i)
	{
		auto item = m_MainMenuItems->GetChild(i)->FindComponent<MainMenuItem>();
		float item_width = item->m_ContentsTextWidth + (2 * JGUI::Gap());

		item->GetTransform()->SetLocalPosition(offset_x, 0.0f);
		item->GetTransform()->SetSize(item_width, MenuItem::MenuItemDefaultHeight);
		offset_x += item_width;
	}

	float total_width = offset_x;
	float owner_width = GetTransform()->GetSize().x;

	if (total_width > owner_width)
	{
		float delta = total_width - owner_width;
		for (uint32_t i = 0; i < itemCount; ++i)
		{
			auto item = m_MainMenuItems->GetChild(i)->FindComponent<MainMenuItem>();
			float item_width = item->m_ContentsTextWidth + (2 * JGUI::Gap());
			item_width -= (delta * (item_width / total_width));

			item->GetTransform()->SetLocalPosition(offset_x, 0.0f);
			item->GetTransform()->SetSize(item_width, MenuItem::MenuItemDefaultHeight);

			offset_x += item_width;
		}
	}


}

bool MainMenu::CheckPoint(MenuItem* item)
{
	bool check = item->GetOwner()->GetCollider()->CheckInPoint(GetOwnerWindow()->GetMousePos());
	if (check) return true;

	uint32_t itemCount = item->GetItemCount();
	for (int i = 0; i < itemCount; ++i)
	{
		if (CheckPoint(item->GetItem(i)))
		{
			return true;
		}
	}



	return false;
}

void MainMenuItem::Awake()
{
	if (GetOwner()->GetCollider() == nullptr) GetOwner()->RegisterCollider(JGUI_Collider_Box);
	m_Image = GetOwner()->CreateJGUIComponent<JGUIImageRenderer>();
	m_Image->SetColor(MenuItem::HightlightColor);
	m_ItemCollection = GetOwner()->CreateJGUIElement("MenuItemCollection")->CreateJGUIComponent<MenuItemCollection>();
	m_ContentsText   = GetOwner()->CreateJGUIElement("ContentsText")->CreateJGUIComponent<Text>();


	GetOwner()->BindMouseBtDownFunc([&](const JGUIKeyDownEvent& e)
	{
		m_IsMouseDown = true;
	});
	GetOwner()->BindMouseBtUpFunc([&](const JGUIKeyUpEvent& e)
	{
		if (!m_IsMouseDown) return;
		m_IsMouseDown = false;
		if (m_ItemCollection->IsOpenItems()) CloseItems();
		else
		{
			auto menu_parent = GetOwner()->GetParent();
			if (menu_parent)
			{
				auto childCount = menu_parent->GetChildCount();
				for (uint32_t i = 0; i < childCount; ++i)
				{
					auto item = menu_parent->GetChild(i)->FindComponent<MainMenuItem>();
					item->CloseItems();
				}
			}
			OpenItems();
		}
	});
}

void MainMenuItem::Start()
{

	

	CloseItems();
}

void MainMenuItem::Destroy()
{



}

void MainMenuItem::Tick(float tick)
{
	if (m_IsMouseDown && JGUI::GetKeyUp(GetOwnerWindow(), KeyCode::LeftMouseButton))
	{
		m_IsMouseDown = false;
	}

	if (m_ItemCollection->IsOpenItems())
	{
		m_Image->SetColor(MenuItem::NormalColor);
	}
	else
	{
		if (GetOwner()->GetCollider()->CheckInPoint(GetOwnerWindow()->GetMousePos()) || m_IsMouseDown)
		{
			m_Image->SetColor(MenuItem::SepColor);
		}
		else
		{
			m_Image->SetColor(MenuItem::HightlightColor);
		}
	}

}

MenuItem* MainMenuItem::AddItem(const MenuItemDesc& desc)
{
	return m_ItemCollection->AddItem(desc);
}

MenuItem* MainMenuItem::GetItem(uint32_t idx) const
{
	return m_ItemCollection->GetItem(idx);
}

uint32_t MainMenuItem::GetItemCount() const
{
	return m_ItemCollection->GetItemCount();
}

void MainMenuItem::AddSeparater()
{
	m_ItemCollection->AddSeparater();
}

void MainMenuItem::OpenItems()
{
	m_ItemCollection->OpenItems();
}

void MainMenuItem::CloseItems()
{
	m_ItemCollection->CloseItems();
}

void MainMenuItem::Setting()
{
	string content_text;
	content_text = m_Desc.contents;
	if (m_Desc.auxiliary_key != KeyCode::Alt)
	{
		content_text += "(" + m_Desc.auxiliary_key.ToString() + ")";
	}

	float font_size = MenuItem::MenuItemDefaultHeight - 2.0f;
	m_ContentsTextWidth = Text::GetTextWidth(m_ContentsText->GetFontName(), font_size, content_text);


	m_ContentsText->SetText(content_text);
	m_ContentsText->SetHAlign(JGUI_Text_HAlignment_Center);
	m_ContentsText->SetVAlign(JGUI_Text_VAlignment_Center);
	m_ContentsText->SetFontSize(font_size);
	GetTransform()->AttachTransform(m_ContentsText->GetTransform());
	GetTransform()->SetSize(m_ContentsTextWidth + (JGUI::Gap() * 2), MenuItem::MenuItemDefaultHeight);


	m_ItemCollection->GetTransform()->SetLocalPosition(0.0f, MenuItem::MenuItemDefaultHeight);
}

void MainMenuItem::ReceiveAuxKeyDown(const JGUIKeyDownEvent& e)
{
	if (!IsActive()) return;
	m_ItemCollection->ReceiveShortCutKeyDown(e);





	if (m_Desc.auxiliary_key == KeyCode::Alt) return;

	if (e.Code == KeyCode::Alt)
	{
		m_ActiveAuxKey = true;
	}

	if (!m_ActiveAuxKey) return;

	if (m_ItemCollection->IsOpenItems())
	{

		// 콜렉션 키 받기
		m_ItemCollection->ReceiveAuxKeyDown(e);





	}
	else
	{
		if (m_Desc.auxiliary_key == e.Code)
		{
			OpenItems();
		}
	}

}

void MainMenuItem::ReceiveAuxKeyUp(const JGUIKeyUpEvent& e)
{
	if (e.Code == KeyCode::Alt)
	{
		m_ActiveAuxKey = false;
	}
	m_ItemCollection->ReceiveAuxKeyUp(e);
	m_ItemCollection->ReceiveShortCutKeyUp(e);
	// 콜렉 션 키 받기
}
