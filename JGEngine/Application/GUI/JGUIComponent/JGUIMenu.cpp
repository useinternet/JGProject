#include "pch.h"
#include "JGUIMenu.h"
#include "JGUIShape.h"
#include "JGUIRectTransform.h"
#include "JGUIButton.h"
#include "JGUICollider.h"
#include "GUI/JGUIObject/JGUIWindow.h"
#include "JGUIText.h"
using namespace std;

void JGUIMenu::Awake()
{

	m_Rectangle = CreateJGUIComponent<JGUIRectangle>("JGUIRectangle");
	GetTransform()->AttachTransform(m_Rectangle->GetTransform());
	m_Rectangle->SetColor(JColor(0.12549f, 0.13333f, 0.18431f, 1.0f));
}

void JGUIMenu::Start()
{
	for (auto& item : m_MenuItems)
	{
		item->BtOnClick();
	}
	MenuItemAllClose();
}


void JGUIMenu::Tick(const JGUITickEvent& e)
{

	if(JGUI::GetKeyDown(GetOwnerWindow(), KeyCode::LeftMouseButton))
	{
		bool result = false;
		for (auto& menuitem : m_MenuItems)
		{
			if (!menuitem->IsActive()) continue;
			if(menuitem->CollisionCheck(GetOwnerWindow()->GetMousePos()))
			{
				result = true;
				break;
			}
		}
		if (!result)
		{
			MenuItemAllClose();
		}
	}


	for (auto& info_pair : m_KeyInfoMap)
	{
		auto& info = info_pair.second;
		auto& key  = info_pair.first;

		if (info.is_pressed)
		{
			m_KeyPressedMap[key] = true;
		}
		if (info.is_released)
		{
			info.tick += GetTick();
			if (info.tick >= info.allow_time)
			{
				info.is_released = false;
				m_KeyPressedMap[key] = false;
			}
			else
			{
				m_KeyPressedMap[key] = true;
			}
		}
	}
}



JGUIMenuItem* JGUIMenu::AddMenuItem(const JGUIMenuItemDesc& desc)
{
	auto item = CreateJGUIComponent<JGUIMenuItem>(desc.name);
	string txt_name = desc.name;
	if (desc.is_use_keyboardkey)
	{
		txt_name = desc.name + " (" + desc.keyboardKey.ToString() + ")";
	}
	float txt_width = JGUIText::GetTextWidth(item->m_NameText->GetFontName(), txt_name, item->m_NameText->GetTextSize());
	item->GetTransform()->SetSize(txt_width + (4 * JGUI::Gap()), 20);
	item->m_NameText->SetText(txt_name);
	item->m_NameText->GetTransform()->SetSize(txt_width + (2 * JGUI::Gap()), 20);
	item->m_Desc = desc;
	item->SetMenuItemType(JGUIMenuItem::MenuItemType_MainMenuItem);
	item->m_OwnerMainMenu = this;

	uint32_t count = (uint32_t)m_MenuItems.size();
	auto size = GetTransform()->GetSize();

	float accX = 0.0f;
	for (auto& item : m_MenuItems)
	{
		accX += item->GetTransform()->GetSize().x;
	}
	item->GetTransform()->SetLocalPosition(accX, 0.0f);
	m_MenuItems.push_back(item);

	return item;
}
uint32_t      JGUIMenu::GetMenuItemCount() const
{
	return (uint32_t)m_MenuItems.size();
}
JGUIMenuItem* JGUIMenu::GetMenuItem(uint32_t idx)
{
	return m_MenuItems[idx];
}

void JGUIMenu::ReceiveKeyEvent(const JGUIKeyDownEvent& e)
{
	if (m_MenuItems.empty()) return;
	if (e.Code == KeyCode::Alt || e.Code == KeyCode::Ctrl || e.Code == KeyCode::Shift)
	{
		m_KeyInfoMap[e.Code].is_pressed  = true;
		m_KeyInfoMap[e.Code].is_released = false;
		m_KeyInfoMap[e.Code].tick = 0.0f;
		m_KeyInfoMap[e.Code].allow_time = 0.5f;
	}
}

void JGUIMenu::ReceiveKeyEvent(const JGUIKeyUpEvent& e)
{
	if (m_MenuItems.empty()) return;
	if (e.Code == KeyCode::Alt || e.Code == KeyCode::Ctrl || e.Code == KeyCode::Shift)
	{
		m_KeyInfoMap[e.Code].is_pressed = false;
		m_KeyInfoMap[e.Code].is_released = true;
	}
	else
	{
		std::vector<JGUIMenuItem*> selected_menuItem;


		for (auto& item : m_MenuItems)
		{
			selected_menuItem = item->GetLastActivedMenuItems();
			if (!selected_menuItem.empty()) break;
		}

		if (selected_menuItem.empty()) selected_menuItem = m_MenuItems;

		for (auto& item : selected_menuItem)
		{
			auto desc = item->GetDesc();
			if (item->m_OwnerMainMenu)
			{
				if (desc.is_use_keyboardkey && desc.keyboardKey == e.Code && m_KeyPressedMap[KeyCode::Alt])
				{
					item->BtOnClick();
				}
			}
			else
			{
				if (desc.is_use_keyboardkey && desc.keyboardKey == e.Code)
				{
					item->BtOnClick();
				}
			}
		}
		// ÇÖ Å°
		for (auto& item : m_MenuItems)
		{
			item->ProcessShortCutKey(
				m_KeyPressedMap[KeyCode::Ctrl],
				m_KeyPressedMap[KeyCode::Alt],
				m_KeyPressedMap[KeyCode::Shift],
				e.Code);
		}
	}
}

void JGUIMenu::KeyFlush()
{
	for (auto& info_pair : m_KeyInfoMap)
	{
		info_pair.second.is_pressed = false;
		info_pair.second.is_released = false;
		info_pair.second.tick = 0.0f;
		info_pair.second.allow_time = 0.5f;
	}
}

void JGUIMenu::NotifySelectItem(JGUIMenuItem* item)
{
	for (auto& menuitem : m_MenuItems)
	{
		if (menuitem != item) menuitem->SubItemClose();
	}
}

void JGUIMenu::MenuItemAllClose()
{
	for (auto& menuitem : m_MenuItems)
	{
		menuitem->SubItemClose();
	}
}
