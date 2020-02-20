#include "pch.h"
#include "JGUIMenu.h"
#include "JGUIShape.h"
#include "JGUIRectTransform.h"
#include "JGUIButton.h"
#include "JGUICollider.h"
#include "GUI/JGUIObject/JGUIWindow.h"
using namespace std;

void JGUIMenu::Awake()
{
	m_Rectangle = CreateJGUIComponent<JGUIRectangle>("JGUIRectangle");
	GetTransform()->AttachTransform(m_Rectangle->GetTransform());
	m_Rectangle->SetColor(JColor(0.12549f, 0.13333f, 0.18431f, 1.0f));




}


void JGUIMenu::Tick(const JGUITickEvent& e)
{

	if(JGUI::GetKeyDown(GetOwnerWindow(), KeyCode::LeftMouseButton))
	{
		bool result = false;
		for (auto& menuitem : m_MenuItems)
		{
			if (!menuitem->IsActive()) continue;
			bool s = menuitem->IsActive();
			auto ss = menuitem->CollisionCheck(GetOwnerWindow()->GetMousePos());
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



}

JGUIMenuItem* JGUIMenu::AddMenuItem(const JGUIMenuItemDesc& desc)
{
	auto item = CreateJGUIComponent<JGUIMenuItem>(desc.name);
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
