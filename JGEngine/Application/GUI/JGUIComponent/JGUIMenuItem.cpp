#include "pch.h"
#include "JGUIMenuItem.h"
#include "JGUIRectTransform.h"
#include "JGUIButton.h"
#include "JGUIText.h"
#include "JGUIShape.h"
#include "JGUIMenu.h"
#include "JGUICollider.h"
using namespace std;

void JGUIMenuItem::Awake()
{
	RegisterCollider(GetOwnerWindow(), JGUI_Collider_Box);
	m_Bt   = CreateJGUIComponent<JGUIButton>("MenuItemBt");
	m_Text = CreateJGUIComponent<JGUIText>("MenuItemTxt");
	
	m_Bt->SetBtColor(EJGUIBtState::JGUI_BtState_Normal, JColor(0.12549f, 0.13333f, 0.18431f, 1.0f));
	m_Bt->SetBtColor(EJGUIBtState::JGUI_BtState_Hightlight, JColor(0.32549f, 0.33333f, 0.38431f, 1.0f));
	m_Bt->SetBtColor(EJGUIBtState::JGUI_BtState_Pressed, JColor(0.32549f, 0.33333f, 0.38431f, 1.0f));
	m_Bt->SetBtColor(EJGUIBtState::JGUI_BtState_Selected, JColor(0.32549f, 0.33333f, 0.38431f, 1.0f));

	GetTransform()->AttachTransform(m_Bt->GetTransform());
	m_Text->SetParent(m_Bt);
	Setting();
	m_Bt->BIndOnClick([&]()
	{
		BtOnClick();
	});
}

void JGUIMenuItem::Resize(const JGUIResizeEvent& e)
{
	m_Text->GetTransform()->SetSize(GetTransform()->GetSize());
	auto size = GetTransform()->GetSize();
	int idx = 0;
	for (auto& subitem : m_SubItems)
	{
		switch (m_MIType)
		{
		case MenuItemType_MainMenuItem:
			subitem->GetTransform()->SetLocalPosition(0.0f, (++idx) * size.y);
			break;
		case MenuItemType_SubMenuItem:
			subitem->GetTransform()->SetLocalPosition(size.x, (idx++) * size.y);
			break;
		case MenuItemType_ContextMenuItem:
			break;
		}
	}
	if (m_Image)
	{
		m_Image->GetTransform()->SetLocalPosition(size.x - 20.0f, 0.0f);
	}
	
}

void JGUIMenuItem::Start()
{

}




JGUIMenuItem* JGUIMenuItem::AddMenuItem(const JGUIMenuItemDesc& desc)
{
	uint32_t count = (uint32_t)m_SubItems.size();
	auto sub_item = CreateJGUIComponent<JGUIMenuItem>(desc.name);
	auto size = GetTransform()->GetSize();
	sub_item->m_OwnerItem = this;

	switch (m_MIType)
	{
	case MenuItemType_MainMenuItem:
		sub_item->GetTransform()->SetLocalPosition(0.0f, (count + 1) * size.y);
		break;
	case MenuItemType_SubMenuItem:
		sub_item->GetTransform()->SetLocalPosition(size.x, count * size.y);
		break;
	case MenuItemType_ContextMenuItem:
		break;
	}

	sub_item->SetActive(false);
	m_SubItems.push_back(sub_item);


	
	return sub_item;
}

void JGUIMenuItem::SetDesc(const JGUIMenuItemDesc& desc)
{
	m_Desc = desc;
	Setting();
}
uint32_t      JGUIMenuItem::GetMenuItemCount() const
{
	return (uint32_t)m_SubItems.size();
}
JGUIMenuItem* JGUIMenuItem::GetMenuItem(uint32_t idx)
{
	return m_SubItems[idx];
}
void JGUIMenuItem::Setting()
{
	auto size = GetTransform()->GetSize();
	SetName(m_Desc.name);
	m_Text->SetText(m_Desc.name);
	m_Text->GetTransform()->SetSize(GetTransform()->GetSize());
	switch (m_MIType)
	{
	case MenuItemType_MainMenuItem:
		m_Text->SetVAlign(JGUI_Text_VAlignment_Center);
		m_Text->SetHAlign(JGUI_Text_HAlignment_Center);
		GetTransform()->SetSize(150.0f, 20);
		break;
	case MenuItemType_SubMenuItem:
		m_Text->SetVAlign(JGUI_Text_VAlignment_Center);
		m_Text->SetHAlign(JGUI_Text_HAlignment_Left);
		GetTransform()->SetSize(150.0f, 20);
		break;
	case MenuItemType_ContextMenuItem:
		break;
	}
}
void JGUIMenuItem::SubItemOpen()
{
	if (m_IsOpen) return;
	m_IsOpen = true;
	for (auto& subitem : m_SubItems)
	{
		subitem->SetActive(true);
		subitem->CreateRightArrow();
		subitem->m_IsOpen = true;
		subitem->SubItemClose();
	}
}
void JGUIMenuItem::SubItemClose()
{
	if (!m_IsOpen) return;
	m_IsOpen = false;
	for (auto& subitem : m_SubItems)
	{
		subitem->SetActive(false);
	}
}

void JGUIMenuItem::BtOnClick()
{
	if (!m_IsOpen)
	{
		SubItemOpen();

		switch (m_MIType)
		{
		case MenuItemType_MainMenuItem:
			m_OwnerMainMenu->NotifySelectItem(this);
			break;
		case MenuItemType_SubMenuItem:
			m_OwnerItem->NotifySelectItem(this);
			break;
		case MenuItemType_ContextMenuItem:
			break;
		}

		
	}
	else
	{
		SubItemClose();
	}
}

void JGUIMenuItem::CreateRightArrow()
{
	if (GetMenuItemCount() == 0) return;
	if (m_Image == nullptr)
	{
		auto size = GetTransform()->GetSize();
		m_Image = CreateJGUIComponent<JGUIImage>("MenuArrow");
		m_Image->SetImage("Right_Arrow.png");
		m_Image->GetTransform()->SetSize(20, 20);
		m_Image->GetTransform()->SetLocalPosition(size.x - 20.0f, 0.0f);
	}
	else
	{
		m_Image->SetActive(true);
	}
}

bool JGUIMenuItem::CollisionCheck(const JVector2Int& mouse_pos)
{
	if (!IsActive()) return false;
	for (auto& item : m_SubItems)
	{
		if (item->CollisionCheck(mouse_pos)) return true;
	}

	return m_Bt->GetCollider()->CheckInPoint(mouse_pos);
}

void JGUIMenuItem::SetMenuItemType(EMenuItemType type)
{
	m_MIType = type;
	Setting();
}

void JGUIMenuItem::NotifySelectItem(JGUIMenuItem* item)
{
	for (auto& menuitem : m_SubItems)
	{
		if (menuitem != item) menuitem->SubItemClose();
	}
}

