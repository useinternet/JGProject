#include "pch.h"
#include "JGUIMenuItem.h"
#include "JGUIButton.h"
#include "JGUIText.h"
#include "JGUIShape.h"
#include "JGUIMenu.h"
using namespace std;

void JGUIMenuItem::Awake()
{
	JGUIElement::Awake();
	m_Bt   = CreateJGUIElement<JGUIButton>("MenuItemBt");
	m_NameText = CreateJGUIElement<JGUIText>("MenuItemNameTxt");
	m_ShortCutText = CreateJGUIElement<JGUIText>("MenuItemShortCutTxt");
	m_BackGround = CreateJGUIElement<JGUIRectangle>("SubItemBacground");
	
	m_Bt->SetBtColor(EJGUIBtState::JGUI_BtState_Normal, JColor(0.10549f, 0.11333f, 0.16431f, 1.0f));
	m_Bt->SetBtColor(EJGUIBtState::JGUI_BtState_Hightlight, JColor(0.32549f, 0.33333f, 0.38431f, 1.0f));
	m_Bt->SetBtColor(EJGUIBtState::JGUI_BtState_Pressed, JColor(0.32549f, 0.33333f, 0.38431f, 1.0f));
	m_Bt->SetBtColor(EJGUIBtState::JGUI_BtState_Selected, JColor(0.32549f, 0.33333f, 0.38431f, 1.0f));


	m_BackGround->SetActive(false);
	m_BackGround->SetColor(JColor(0.12549f, 0.13333f, 0.18431f, 1.0f));
	
	m_BackGround->RegisterCollider(JGUI_Collider_Box);
	GetTransform()->AttachTransform(m_Bt->GetTransform());
	m_NameText->SetParent(m_Bt);

	m_ShortCutText->SetParent(m_Bt);
	m_ShortCutText->SetColor(JColor(0.7f, 0.7f, 0.7f, 1.0f));
	m_ShortCutText->SetActive(false);
	Setting();
	m_Bt->BindOnClick([&]()
	{
		BtOnClick();
	});
}

void JGUIMenuItem::Resize(const JGUIResizeEvent& e)
{
	Setting();
	auto size = GetTransform()->GetSize();
	int idx = 0;
	int se_idx = 0;

	for (auto& subitem : m_SubItems)
	{
		bool result = false;
		if (m_Separators.find(idx) != m_Separators.end()) result = true;
		subitem->GetTransform()->SetLocalPosition(0.0f, ((idx++) * size.y) + (se_idx * m_SeparatorHeightGap));
		if (result)
		{
			se_idx++;
		}
	}
	if (m_Image)
	{
		m_Image->GetTransform()->SetLocalPosition(size.x - 20.0f, 0.0f);
	}

	
}

void JGUIMenuItem::Start()
{
	JGUIElement::Start();
	float size_y = (20 * m_SubItems.size()) + (m_Separators.size() * m_SeparatorHeightGap);
	float size_x = m_SubItemWidth;
	m_BackGround->GetTransform()->SetSize(size_x, size_y);
}






JGUIMenuItem* JGUIMenuItem::AddMenuItem(const JGUIMenuItemDesc& desc)
{
	uint32_t count = (uint32_t)m_SubItems.size();

	// 서브아이템 생성 및 초기화
	auto sub_item = CreateJGUIElement<JGUIMenuItem>(desc.name);
	auto size = GetTransform()->GetSize();
	sub_item->m_OwnerItem = this;
	sub_item->SetParent(m_BackGround);

	// Separator 의 갯수에따른 서브아이템 위치 조정
	float item_y = 0.f;
	uint32_t sep_count = (uint32_t)m_Separators.size();
	item_y += (sep_count * m_SeparatorHeightGap);
	item_y += (count * size.y);
	sub_item->GetTransform()->SetLocalPosition(size.x, item_y);

	m_SubItems.push_back(sub_item);

	// 단축키 및 숏키 에따른 텍스트 변경과 버튼 크기 조정
	string txt_name = desc.name;
	if (desc.is_use_keyboardkey)
	{
		txt_name = desc.name + " (" + desc.keyboardKey.ToString() + ")";
	}
	// -- 메뉴 이름 
	float txt_width = JGUIText::GetTextWidth(sub_item->m_NameText->GetFontName(), txt_name, sub_item->m_NameText->GetTextSize());
	m_SubItemNameWidth = std::max<float>(m_SubItemNameWidth, txt_width + (JGUI::Gap() * 4));

	// -- 메뉴 숏키
	string shortcut;
	if (desc.is_use_shortcut_key)
	{
		sub_item->m_ShortCutText->SetActive(true);
		if (desc.is_ctrl)  shortcut += "Ctrl + ";
		if (desc.is_alt)   shortcut += "Alt + ";
		if (desc.is_shift) shortcut += "Shift + ";
		shortcut += desc.shortcut_key.ToString();

		float shortcut_width = JGUIText::GetTextWidth(sub_item->m_NameText->GetFontName(), shortcut, sub_item->m_NameText->GetTextSize());
		m_SubItemShortCutWidth = std::max<float>(m_SubItemShortCutWidth, shortcut_width + (JGUI::Gap() * 4));
	}
	
	m_SubItemWidth = m_SubItemNameWidth + m_SubItemShortCutWidth;


	// 서브아이템 조정
	for (auto& item : m_SubItems)
	{
		item->m_ShortCutText->GetTransform()->SetSize(m_SubItemShortCutWidth - (2 * JGUI::Gap()), size.y);
		item->m_NameText->GetTransform()->SetSize(m_SubItemNameWidth - (2 * JGUI::Gap()), size.y);
		item->GetTransform()->SetSize(m_SubItemWidth, size.y);
	}
	sub_item->m_NameText->SetText(txt_name);
	sub_item->m_NameText->GetTransform()->OffsetLocalPosition(JGUI::Gap(), 0.0f);
	sub_item->m_ShortCutText->SetText(shortcut);
	sub_item->m_ShortCutText->GetTransform()->OffsetLocalPosition(
		sub_item->m_NameText->GetTransform()->GetSize().x + (JGUI::Gap() * 2), 0.0f);
	sub_item->SetDesc(desc);
	//sub_item->SetActive(false);

	for (auto& separtor_pair : m_Separators)
	{
		auto size = separtor_pair.second->GetTransform()->GetSize();
		separtor_pair.second->GetTransform()->SetSize(m_SubItemWidth - m_SeparatorWidthGap, size.y);
	}
	return sub_item;
}

void JGUIMenuItem::AddSeparator()
{
	int idx = m_SubItems.size() - 1;


	auto separtor = CreateJGUIElement<JGUIRectangle>("Separtor_" + to_string(idx));
	separtor->SetParent(m_BackGround);
	m_Separators[idx] = separtor;


	separtor->SetColor(JColor(0.8f, 0.8f, 0.8f, 1.0f));

	float se_size_x = m_SubItemWidth - m_SeparatorWidthGap;
	float se_size_y = m_SeparatorHeightGap;

	separtor->GetTransform()->SetSize(se_size_x, m_SeparatorThickness);

	float se_x = 0.0f;
	se_x += (m_SeparatorWidthGap * 0.5f);
	float se_y = (idx + 1) * GetTransform()->GetSize().y;
	se_y += (m_SeparatorHeightGap * 0.5f - (m_SeparatorThickness * 0.5f));


	separtor->GetTransform()->SetLocalPosition(se_x, se_y);
	//separtor->SetActive(false);
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

	SetName(m_Desc.name);
	auto size = GetTransform()->GetSize();
	switch (m_MIType)
	{
	case MenuItemType_MainMenuItem:
		m_NameText->SetVAlign(JGUI_Text_VAlignment_Top);
		m_NameText->SetHAlign(JGUI_Text_HAlignment_Center);
		m_BackGround->GetTransform()->SetLocalPosition(0.0f, size.y);
		break;
	case MenuItemType_SubMenuItem:
		m_NameText->SetVAlign(JGUI_Text_VAlignment_Top);
		m_NameText->SetHAlign(JGUI_Text_HAlignment_Left);
		m_BackGround->GetTransform()->SetLocalPosition(size.x, 0.0f);
		break;
	case MenuItemType_ContextMenuItem:
		break;
	}

}
void JGUIMenuItem::SubItemOpen()
{
	if (m_IsOpen) return;
	m_IsOpen = true;
	if(!m_SubItems.empty()) m_BackGround->SetActive(true);

	for (auto& subitem : m_SubItems)
	{
		//subitem->SetActive(true);
		subitem->CreateRightArrow();
		subitem->m_IsOpen = true;
		subitem->SubItemClose();
	}
}
void JGUIMenuItem::SubItemClose()
{
	if (!m_IsOpen) return;
	m_IsOpen = false;
	m_BackGround->SetActive(false);

	m_Bt->Reset();
}

void JGUIMenuItem::BtOnClick()
{
	if (m_SubItems.empty())
	{
		if (m_Desc.is_binding_func)
		{
			m_Desc.func();
		}
		JGUIMenuItem* nextIt = m_OwnerItem;
		while (nextIt != nullptr)
		{
			if (nextIt->m_OwnerItem == nullptr)
			{
				nextIt->m_OwnerMainMenu->MenuItemAllClose();
			}
			nextIt = nextIt->m_OwnerItem;
		
		}
	}
	else
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


}

void JGUIMenuItem::CreateRightArrow()
{
	if (GetMenuItemCount() == 0) return;
	if (m_Image == nullptr)
	{
		auto size = GetTransform()->GetSize();
		m_Image = CreateJGUIElement<JGUIImage>("MenuArrow");
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
	if (!IsActive() || !m_BackGround->IsActive()) return false;


	for (auto& item : m_SubItems)
	{
		if (item->CollisionCheck(mouse_pos)) return true;
	}
	return m_BackGround->GetCollider()->CheckInPoint(mouse_pos) || m_Bt->GetCollider()->CheckInPoint(mouse_pos);
}
void JGUIMenuItem::SetDesc(const JGUIMenuItemDesc& desc)
{
	m_Desc = desc;
	Setting();
}
std::vector<JGUIMenuItem*> JGUIMenuItem::GetLastActivedMenuItems() const
{
	if(!m_BackGround->IsActive()) return std::vector<JGUIMenuItem*>();


	auto result = m_SubItems;


	for (auto& item : m_SubItems)
	{
		auto items = item->GetLastActivedMenuItems();
		if (!items.empty())
		{
			result = items;
			break;
		}
	}
	return result;
}

void JGUIMenuItem::ProcessShortCutKey(bool is_ctrl, bool is_alt, bool is_shift, KeyCode code)
{
	for (auto& item : m_SubItems)
	{
		item->ProcessShortCutKey(is_ctrl, is_alt, is_shift, code);
	}
	bool result = true;

	if (m_Desc.is_use_shortcut_key)
	{
		if (!is_ctrl && m_Desc.is_ctrl)   result = false;
		if (!is_shift && m_Desc.is_shift)  result = false;
		if (!is_alt && m_Desc.is_alt)    result = false;

		if (result && m_Desc.shortcut_key == code && m_Desc.is_binding_func)
		{
			BtOnClick();
		}
	}


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

