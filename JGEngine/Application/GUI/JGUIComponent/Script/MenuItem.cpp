#include "pch.h"
#include "MenuItem.h"


#include "GUI/JGUIComponent/Element/JGUIRenderer.h"
#include "GUI/JGUIComponent/Element/JGUICanvas.h"

#include "GUI/JGUIObject/JGUIElement.h"
#include "GUI/JGUIObject/JGUIWindow.h"

#include "GUI/JGUIComponent/Script/Text.h"
JGUI_REGISTER_COMPONENT(MenuItem)
JGUI_REGISTER_COMPONENT(MenuItemCollection)
using namespace std;

const float MenuItem::IconGap     = 1.0f;
const float MenuItem::ArrowGap    = 4.0f;
const float MenuItem::SepHeight   = 5.0f;
const float MenuItem::SepGap      = 2.0f;
const float MenuItem::BorderThick = 0.5f;


const JColor MenuItem::NormalColor     = JColor(0.106f, 0.106f, 0.116f, 1.0f);
const JColor MenuItem::HightlightColor = JColor(0.206f, 0.206f, 0.216f, 1.0f);
const JColor MenuItem::BorderColor     = JColor(0.206f, 0.206f, 0.216f, 1.0f);
const JColor MenuItem::SepColor        = JColor(0.376f, 0.376f, 0.386f, 1.0f);

void MenuItem::Awake()
{
	if (GetOwner()->GetCollider() == nullptr) GetOwner()->RegisterCollider(JGUI_Collider_Box);



	m_Image            = GetOwner()->CreateJGUIElement("Item")->CreateJGUIComponent<JGUIImageRenderer>();
	m_ContentsText     = m_Image->GetOwner()->CreateJGUIElement("Contents")->CreateJGUIComponent<Text>();
	m_ShortcutKeyText  = m_Image->GetOwner()->CreateJGUIElement("ShortcutKey")->CreateJGUIComponent<Text>();
	m_Arrow            = m_Image->GetOwner()->CreateJGUIElement("NextArrow")->CreateJGUIComponent<JGUIImageRenderer>();
	m_Items      = GetOwner()->CreateJGUIElement("ChildItems");
	m_BackGround = m_Items->CreateJGUIComponent<JGUIImageRenderer>();
	m_Separaters = GetOwner()->CreateJGUIElement("Separaters");
	m_BackGroundBorder = GetOwner()->CreateJGUIElement("BackGroundBorder")->CreateJGUIComponent<JGUIShapeRenderer>();

	GetTransform()->AttachTransform(m_Image->GetTransform());
	GetTransform()->SetSize(100, MenuItemDefaultHeight);
}

void MenuItem::Start()
{
	Setting();
	ItemsSetting();

	GetOwner()->BindMouseBtDownFunc([&](const JGUIKeyDownEvent& e)
	{
		m_IsMouseDown = true;
	});
	GetOwner()->BindMouseBtUpFunc([&](const JGUIKeyUpEvent& e)
	{
		if (!m_IsMouseDown) return;
		m_IsMouseDown = false;
		ItemOperation();
		
	});
	CloseItems();
}

void MenuItem::Destroy()
{

}

void MenuItem::Tick(float tick)
{
	if (m_IsDirty)
	{
		ItemsSetting();
	}

	if (m_IsMouseDown && JGUI::GetKeyUp(GetOwnerWindow(), KeyCode::LeftMouseButton))
	{
		m_IsMouseDown = false;
	}



	if (GetOwner()->GetCollider()->CheckInPoint(GetOwnerWindow()->GetMousePos()) || m_IsMouseDown)
	{
		m_Image->SetColor(HightlightColor);
	}
	else
	{
		m_Image->SetColor(NormalColor);
	}
	if (m_Items->GetChildCount() == 0)
	{
		m_Arrow->GetOwner()->SetActive(false);
	}
	else
	{
		m_Arrow->GetOwner()->SetActive(true);
	}
}

MenuItem* MenuItem::AddItem(const MenuItemDesc& desc)
{
	MenuItem* item = m_Items->CreateJGUIElement(desc.name)->CreateJGUIComponent<MenuItem>();
	item->m_Desc = desc;
	item->m_OwnerCollection = m_OwnerCollection;
	m_IsDirty = true;
	return item;
}
MenuItem* MenuItem::GetItem(uint32_t idx) const
{
	return m_Items->GetChild(idx)->FindComponent<MenuItem>();
}
uint32_t  MenuItem::GetItemCount() const
{
	return m_Items->GetChildCount();
}
void      MenuItem::AddSeparater()
{
	uint32_t childCount = m_Items->GetChildCount();
	if (childCount == 0) return;
	uint32_t sep_idx = childCount - 1;

	if (m_SepIndexMap.find(sep_idx) != m_SepIndexMap.end()) return;


	auto renderer = m_Separaters->CreateJGUIElement("Separater")->CreateJGUIComponent<JGUIShapeRenderer>();
	renderer->CreateRectangle();
	renderer->SetColor(SepColor);
	m_SepIndexMap[sep_idx] = renderer->GetOwner();
	m_IsDirty = true;
}

void      MenuItem::OpenItems()
{
	uint32_t itemCount = m_Items->GetChildCount();
	if (itemCount == 0) return;
	m_Items->SetActive(true);
	m_Separaters->SetActive(true);
	m_BackGroundBorder->GetOwner()->SetActive(true);

	for (uint32_t i = 0; i < itemCount; ++i)
	{
		m_Items->GetChild(i)->FindComponent<MenuItem>()->CloseItems();

	}

}
void      MenuItem::CloseItems()
{
	m_Separaters->SetActive(false);
	m_Items->SetActive(false);
	m_BackGroundBorder->GetOwner()->SetActive(false);
}
bool MenuItem::IsOpenItems() const
{
	return m_Items->IsActive();
}
void MenuItem::Setting()
{
	if (m_IsSetting) return;
	m_IsSetting = true;
	auto size = GetTransform()->GetSize();



	// 이미지 소스 생성
	if (m_Desc.image_source_path != "none")
	{
		m_Image->SetColor(JColor::White());
		m_Image->SetImage(m_Desc.image_source_path);
	}
	else
	{
		m_Image->SetColor(NormalColor);
	}
	if (m_Desc.backGround_path != "none")
	{
		m_BackGround->SetColor(JColor::White());
		m_BackGround->SetImage(m_Desc.backGround_path);
	
	}
	else
	{
		m_BackGround->SetColor(NormalColor);
	}
	if (m_Desc.icon_source_path != "none")
	{
		if(m_Icon == nullptr)
			m_Icon = m_Image->GetOwner()->CreateJGUIElement("Icon")->CreateJGUIComponent<JGUIImageRenderer>();
		m_Icon->SetColor(JColor::White());
		m_Icon->SetImage(m_Desc.icon_source_path);
	}
	if (m_Arrow)
	{
		m_Arrow->SetImage(m_ArrowSource_Path);
	}




	// 메뉴 내용 및 단축키 가로 길이 및 텍스트 분석
	float fontsize = size.y - (IconGap * 2);
	string contents_text = m_Desc.contents;
	if (m_Desc.auxiliary_key != KeyCode::Alt)
	{
		contents_text += "(" + m_Desc.auxiliary_key.ToString() + ")";
	}
	m_ContentsTextWidth = Text::GetTextWidth(m_ContentsText->GetFontName(), fontsize, contents_text);
	string shortcut_key_text;
	if (!m_Desc.shortcut_key.empty())
	{
		for (size_t i = 0; i < m_Desc.shortcut_key.size(); ++i)
		{
			shortcut_key_text += m_Desc.shortcut_key[i].ToString();
			if (i != m_Desc.shortcut_key.size() - 1)
			{
				shortcut_key_text += " + ";
			}
		}
		m_ShortcutKeyTextWidth = Text::GetTextWidth(m_ShortcutKeyText->GetFontName(), fontsize, shortcut_key_text);
	}
	else
	{
		m_ShortcutKeyTextWidth = 0.0f;
	}


	/* Item 구성물 위치 및 사이즈 조정 */

	if (m_Icon)
	{
		float icon_size = size.y - (IconGap * 2);
		m_Icon->GetTransform()->SetLocalPosition(JGUI::Gap() + IconGap, IconGap);
		m_Icon->GetTransform()->SetSize(icon_size, icon_size);
	}
	float offset_x = JGUI::Gap() + size.y + JGUI::Gap();;

	// ContentsText
	float contentTextSize_x = m_ContentsTextWidth + JGUI::Gap() * 2;
	m_ContentsText->GetTransform()->SetLocalPosition(offset_x, 0.0f);
	m_ContentsText->GetTransform()->SetSize(contentTextSize_x, size.y);
	

	offset_x += contentTextSize_x;
	float shortcutKeySize_x = m_ShortcutKeyTextWidth + JGUI::Gap();
	m_ShortcutKeyText->SetColor(JColor(0.65f, 0.65f, 0.75f, 1.0f));
	m_ShortcutKeyText->GetTransform()->SetLocalPosition(offset_x, 0.0f);
	m_ShortcutKeyText->GetTransform()->SetSize(shortcutKeySize_x, size.y);
	offset_x += shortcutKeySize_x;

	// arrow
	float arrowsize = MenuItemDefaultHeight - (ArrowGap * 2);
	if (m_Arrow)
	{
		m_Arrow->GetTransform()->SetSize(arrowsize, arrowsize);
		m_Arrow->GetTransform()->SetLocalPosition(offset_x + ArrowGap, ArrowGap);
	}
	offset_x += MenuItemDefaultHeight;
	

	// Text 설정
	m_ContentsText->SetFontSize(fontsize);
	m_ContentsText->SetVAlign(JGUI_Text_VAlignment_Center);
	m_ContentsText->SetText(contents_text);


	m_ShortcutKeyText->SetFontSize(size.y - (IconGap * 2));
	m_ShortcutKeyText->SetVAlign(JGUI_Text_VAlignment_Center);


	// 메뉴 단축키
	if (!m_Desc.shortcut_key.empty())
	{
		m_ShortcutKeyText->SetText(shortcut_key_text);
	}




	m_BackGround->GetTransform()->AttachTransform(m_BackGroundBorder->GetTransform());
	m_BackGroundBorder->CreateEmptyRectangle(BorderThick);
	m_BackGroundBorder->SetColor(BorderColor);


	float item_width = offset_x;
	GetTransform()->SetSize(item_width, size.y);
}

void MenuItem::ItemsSetting()
{
	//
	//  Gap  Icon  Gap Contents GapGap Shortcut GapGap
	if (!m_IsDirty) return;
	m_IsDirty = false;

	
	float max_contents_width = 0.0f;
	float max_shortcut_width = 0.0f;
	float icon_size = MenuItemDefaultHeight;

	uint32_t itemCount = m_Items->GetChildCount();
	uint32_t sepCount  = m_Separaters->GetChildCount();
	for (uint32_t i = 0; i < itemCount; ++i)
	{
		MenuItem* item = m_Items->GetChild(i)->FindComponent<MenuItem>();
		item->Setting();
		if (max_contents_width < item->m_ContentsTextWidth) {
			max_contents_width = item->m_ContentsTextWidth;
		}
		if (max_shortcut_width < item->m_ShortcutKeyTextWidth) {
			max_shortcut_width = item->m_ShortcutKeyTextWidth;
		}
	}
	float item_gap = 1.0f;
	float offset_x = 0.0f;
	float offset_y = 0.0f;
	
	float icon_area_size = MenuItemDefaultHeight  + JGUI::Gap() * 2;
	float contents_area_size = max_contents_width + JGUI::Gap() * 2;
	float shortcut_area_size = max_shortcut_width + JGUI::Gap() * 2;
	float arrow_area_size = MenuItemDefaultHeight;

	float item_size = icon_area_size + contents_area_size + shortcut_area_size + arrow_area_size;


	auto owner_size = GetTransform()->GetSize();
	// 요기
	m_BackGround->GetTransform()->SetSize(item_size + (item_gap * 2), (sepCount * SepHeight) + (itemCount * MenuItemDefaultHeight) + (item_gap * 2));
	m_BackGround->GetTransform()->SetLocalPosition(owner_size.x, 0.0f);

	// 아이템 미세 조정
	
	for (uint32_t i = 0; i < itemCount; ++i)
	{
		MenuItem* item = m_Items->GetChild(i)->FindComponent<MenuItem>();
		item->GetTransform()->SetLocalPosition(item_gap + offset_x, item_gap + offset_y);
		item->GetTransform()->SetSize(item_size, MenuItemDefaultHeight);
		item->m_ContentsText->GetTransform()->SetSize(contents_area_size, MenuItemDefaultHeight);
		item->m_ShortcutKeyText->GetTransform()->SetSize(shortcut_area_size, MenuItemDefaultHeight);
		item->m_ShortcutKeyText->GetTransform()->SetLocalPosition(icon_area_size + contents_area_size, 0.0f);


		item->m_Arrow->GetTransform()->SetLocalPosition(
			icon_area_size + contents_area_size + shortcut_area_size + ArrowGap,
			ArrowGap);



		offset_y += MenuItemDefaultHeight;

		//
		if (m_SepIndexMap.find(i) != m_SepIndexMap.end())
		{
			m_SepIndexMap[i]->GetTransform()->SetLocalPosition(owner_size.x + item_gap + offset_x + icon_area_size, item_gap + offset_y + SepGap);
			// Icon ------
			
			m_SepIndexMap[i]->GetTransform()->SetSize(contents_area_size + shortcut_area_size - SepGap, SepHeight - (SepGap * 2));
			offset_y += SepHeight;
		}
	}
}

void MenuItem::ItemOperation()
{
	if (IsOpenItems()) CloseItems();
	else
	{
		auto menu_parent = GetOwner()->GetParent()->GetParent();
		if (menu_parent)
		{
			auto menuItem = menu_parent->FindComponent<MenuItem>();
			if (menuItem)
			{
				uint32_t itemCount = menuItem->GetItemCount();
				for (int i = 0; i < itemCount; ++i)
				{
					menuItem->GetItem(i)->CloseItems();
				}
			}
			else
			{
				auto menuCollection = menu_parent->FindComponent<MenuItemCollection>();

				if (menuCollection)
				{
					uint32_t itemCount = menuCollection->GetItemCount();
					for (int i = 0; i < itemCount; ++i)
					{
						menuCollection->GetItem(i)->CloseItems();
					}
				}

			}
		}
		if (GetItemCount() == 0)
		{
			if (m_Desc.func)
			{
				m_Desc.func(m_Desc.data);
			}

			if (m_OwnerCollection)
			{
				m_OwnerCollection->CloseItems();
			}
		}
		else
		{
			OpenItems();
		}
	}
}

void MenuItem::ReceiveAuxKeyDown(const JGUIKeyDownEvent& e)
{
	//ENGINE_LOG_INFO("{0} Receive KeyDown ", m_Desc.contents);
	if (!IsActive()) return;
	if (m_Desc.auxiliary_key == KeyCode::Alt) return;
	if (m_IsAuxOperation) return;



	if (e.Code == KeyCode::Alt)
	{
		m_ActiveAuxKey = true;
	}

	if (!m_ActiveAuxKey) return;


	if (m_Desc.auxiliary_key == e.Code)
	{
		ENGINE_LOG_INFO("Execute(Aux) : {0} ", m_Desc.contents);
		ItemOperation();
		m_IsAuxOperation = true;
	}
}

void MenuItem::ReceiveAuxKeyUp(const JGUIKeyUpEvent& e)
{
	//ENGINE_LOG_INFO("{0} Receive KeyUp ", m_Desc.contents);
	if (e.Code == KeyCode::Alt)
	{
		m_ActiveAuxKey   = false;
		m_IsAuxOperation = false;
	}
}

void MenuItem::ReceiveShortCutKeyDown(const JGUIKeyDownEvent& e)
{
	if (m_Desc.shortcut_key.empty()) return;
	if (m_IsShortCutOperation) return;
	if (m_ActiveShortcutKey.size() != m_Desc.shortcut_key.size())
	{
		m_ActiveShortcutKey.clear();
		for (auto& key : m_Desc.shortcut_key)
		{
			if (m_ActiveShortcutKey.find(key) == m_ActiveShortcutKey.end())
			{
				m_ActiveShortcutKey[key] = false;
			}
		}
	}

	KeyCode currentCheckKey = m_Desc.shortcut_key[m_ShortCutIndex];
	if (currentCheckKey == e.Code)
	{
		m_ActiveShortcutKey[currentCheckKey] = true;
		++m_ShortCutIndex;
	}



	for (auto check : m_ActiveShortcutKey)
	{
		if (!check.second) return;
	}

	ENGINE_LOG_INFO("Execute(Shortcutkey) : {0} ", m_Desc.contents);

	ItemOperation();
	m_IsShortCutOperation = true;
}

void MenuItem::ReceiveShortCutKeyUp(const JGUIKeyUpEvent& e)
{
	for (auto& key_pair : m_ActiveShortcutKey)
	{
		if (key_pair.second && key_pair.first == e.Code)
		{
			key_pair.second = false;
			m_ShortCutIndex = 0;
			m_IsShortCutOperation = false;
		}
	}
}



void MenuItemCollection::Awake()
{
	m_BackGround = GetOwner()->CreateJGUIComponent<JGUIImageRenderer>();
	m_MenuItems  = GetOwner()->CreateJGUIElement("MenuItems");
	m_Separaters = GetOwner()->CreateJGUIElement("Separaters");
	m_BackGroundBorder = GetOwner()->CreateJGUIElement("Border")->CreateJGUIComponent<JGUIShapeRenderer>();
	m_BackGroundBorder->CreateEmptyRectangle(MenuItem::BorderThick);
	GetTransform()->AttachTransform(m_BackGroundBorder->GetTransform());
	GetTransform()->AttachTransform(m_MenuItems->GetTransform());
}

void MenuItemCollection::Start()
{
	ItemsSetting();
}

void MenuItemCollection::Destroy()
{

}

void MenuItemCollection::Tick(float tick)
{
	if (m_IsDirty)
	{
		ItemsSetting();
	}
}


MenuItem* MenuItemCollection::AddItem(const MenuItemDesc& desc)
{
	MenuItem* item = m_MenuItems->CreateJGUIElement(desc.name)->CreateJGUIComponent<MenuItem>();
	item->m_OwnerCollection = this;
	item->m_Desc = desc;
	m_IsDirty = true;
	return item;
}
MenuItem* MenuItemCollection::GetItem(uint32_t idx) const
{
	return m_MenuItems->GetChild(idx)->FindComponent<MenuItem>();
}
uint32_t  MenuItemCollection::GetItemCount() const
{
	return m_MenuItems->GetChildCount();
}
void      MenuItemCollection::AddSeparater()
{
	uint32_t childCount = m_MenuItems->GetChildCount();
	if (childCount == 0) return;
	uint32_t sep_idx = childCount - 1;

	if (m_SepIndexMap.find(sep_idx) != m_SepIndexMap.end()) return;
	auto renderer = m_Separaters->CreateJGUIElement("Separater")->CreateJGUIComponent<JGUIShapeRenderer>();
	renderer->CreateRectangle();
	renderer->SetColor(MenuItem::SepColor);
	m_SepIndexMap[sep_idx] = renderer->GetOwner();
	m_IsDirty = true;
}


void      MenuItemCollection::OpenItems()
{
	GetOwner()->SetActive(true);
	//m_MenuItems->SetActive(true);
	//m_Separaters->SetActive(true);
	uint32_t itemCount = m_MenuItems->GetChildCount();
	for (uint32_t i = 0; i < itemCount; ++i)
	{
		m_MenuItems->GetChild(i)->FindComponent<MenuItem>()->CloseItems();
	}
}
void      MenuItemCollection::CloseItems()
{
	GetOwner()->SetActive(false);




}

bool MenuItemCollection::IsOpenItems() const
{
	return GetOwner()->IsActive();
}

void MenuItemCollection::ReceiveAuxKeyDown(const JGUIKeyDownEvent& e)
{
	MenuItem* openItem = nullptr;
	uint32_t itemCount = GetItemCount();


	for (uint32_t i = 0; i < itemCount; ++i)
	{
		auto item = GetItem(i);
		if (item->IsOpenItems())
		{
			openItem = item;
			break;
		}
	}
	if (openItem == nullptr)
	{
		for (uint32_t i = 0; i < itemCount; ++i)
		{
			auto item = GetItem(i);
			item->ReceiveAuxKeyDown(e);
		}
	}
	else
	{

		while (true)
		{
			bool is_open = false;
			itemCount = openItem->GetItemCount();
		

			for (uint32_t i = 0; i < itemCount; ++i)
			{
				auto item = openItem->GetItem(i);
				if (item->IsOpenItems())
				{
					openItem = item;
					is_open = true;
					break;
				}
			}
			if (!is_open)
			{
				for (uint32_t i = 0; i < itemCount; ++i)
				{
					auto item = openItem->GetItem(i);
					item->ReceiveAuxKeyDown(e);
					break;
				}
				break;
			}
			if (itemCount == 0) break;
		}
	}
}

void MenuItemCollection::ReceiveAuxKeyUp(const JGUIKeyUpEvent& e)
{
	uint32_t itemCount = GetItemCount();
	for (uint32_t i = 0; i < itemCount; ++i)
	{
		auto item = GetItem(i);
		ItemsAuxKeyUp(item, e);
		item->ReceiveAuxKeyUp(e);
	}
}

void MenuItemCollection::ReceiveShortCutKeyDown(const JGUIKeyDownEvent& e)
{
	uint32_t itemCount = GetItemCount();
	for (uint32_t i = 0; i < itemCount; ++i)
	{
		auto item = GetItem(i);
		ItemsShortcutKeyDown(item, e);
		item->ReceiveShortCutKeyDown(e);
	}
}

void MenuItemCollection::ReceiveShortCutKeyUp(const JGUIKeyUpEvent& e)
{
	uint32_t itemCount = GetItemCount();
	for (uint32_t i = 0; i < itemCount; ++i)
	{
		auto item = GetItem(i);
		ItemsShortcutKeyUp(item, e);
		item->ReceiveShortCutKeyUp(e);
	}
}

void MenuItemCollection::ItemsSetting()
{

	if (!m_IsDirty) return;
	m_IsDirty = false;


	float max_contents_width = 0.0f;
	float max_shortcut_width = 0.0f;
	float icon_size = MenuItem::MenuItemDefaultHeight;

	uint32_t itemCount = m_MenuItems->GetChildCount();
	uint32_t sepCount = m_Separaters->GetChildCount();
	for (uint32_t i = 0; i < itemCount; ++i)
	{
		MenuItem* item = m_MenuItems->GetChild(i)->FindComponent<MenuItem>();
		item->Setting();
		if (max_contents_width < item->m_ContentsTextWidth) {
			max_contents_width = item->m_ContentsTextWidth;
		}
		if (max_shortcut_width < item->m_ShortcutKeyTextWidth) {
			max_shortcut_width = item->m_ShortcutKeyTextWidth;
		}
	}
	float item_gap = 1.0f;
	float offset_x = 0.0f;
	float offset_y = 0.0f;

	float icon_area_size = MenuItem::MenuItemDefaultHeight + JGUI::Gap() * 2;
	float contents_area_size = max_contents_width + JGUI::Gap() * 2;
	float shortcut_area_size = max_shortcut_width + JGUI::Gap() * 2;
	float arrow_area_size = MenuItem::MenuItemDefaultHeight;

	float item_size = icon_area_size + contents_area_size + shortcut_area_size + arrow_area_size;


	// 요기
	m_BackGround->GetTransform()->SetSize(item_size + (item_gap * 2), (sepCount * MenuItem::SepHeight) + (itemCount * MenuItem::MenuItemDefaultHeight) + (item_gap * 2));
	m_BackGround->SetColor(MenuItem::NormalColor);
	m_BackGroundBorder->SetColor(MenuItem::BorderColor);
	// 아이템 미세 조정

	for (uint32_t i = 0; i < itemCount; ++i)
	{
		MenuItem* item = m_MenuItems->GetChild(i)->FindComponent<MenuItem>();
		item->GetTransform()->SetLocalPosition(item_gap + offset_x, item_gap + offset_y);
		item->GetTransform()->SetSize(item_size, MenuItem::MenuItemDefaultHeight);
		item->m_ContentsText->GetTransform()->SetSize(contents_area_size, MenuItem::MenuItemDefaultHeight);
		item->m_ShortcutKeyText->GetTransform()->SetSize(shortcut_area_size, MenuItem::MenuItemDefaultHeight);
		item->m_ShortcutKeyText->GetTransform()->SetLocalPosition(icon_area_size + contents_area_size, 0.0f);


		item->m_Arrow->GetTransform()->SetLocalPosition(
			icon_area_size + contents_area_size + shortcut_area_size + MenuItem::ArrowGap,
			MenuItem::ArrowGap);



		offset_y += MenuItem::MenuItemDefaultHeight;

		//
		if (m_SepIndexMap.find(i) != m_SepIndexMap.end())
		{
			m_SepIndexMap[i]->GetTransform()->SetLocalPosition(item_gap + offset_x + icon_area_size, item_gap + offset_y + MenuItem::SepGap);
			m_SepIndexMap[i]->GetTransform()->SetSize(contents_area_size + shortcut_area_size - MenuItem::SepGap, MenuItem::SepHeight - (MenuItem::SepGap * 2));
			offset_y += MenuItem::SepHeight;
		}
	}

}

void MenuItemCollection::ItemsAuxKeyUp(MenuItem* item, const JGUIKeyUpEvent& e)
{
	uint32_t itemCount = item->GetItemCount();
	for (uint32_t i = 0; i < itemCount; ++i)
	{
		auto I = item->GetItem(i);
		ItemsAuxKeyUp(I, e);
		I->ReceiveAuxKeyUp(e);
	}
}

void MenuItemCollection::ItemsShortcutKeyDown(MenuItem* item, const JGUIKeyDownEvent& e)
{

	uint32_t itemCount = item->GetItemCount();
	for (uint32_t i = 0; i < itemCount; ++i)
	{
		auto I = item->GetItem(i);
		ItemsShortcutKeyDown(I, e);
		I->ReceiveShortCutKeyDown(e);
	}
}

void MenuItemCollection::ItemsShortcutKeyUp(MenuItem* item, const JGUIKeyUpEvent& e)
{
	uint32_t itemCount = item->GetItemCount();
	for (uint32_t i = 0; i < itemCount; ++i)
	{
		auto I = item->GetItem(i);
		ItemsShortcutKeyUp(I, e);
		I->ReceiveShortCutKeyUp(e);
	}
}
