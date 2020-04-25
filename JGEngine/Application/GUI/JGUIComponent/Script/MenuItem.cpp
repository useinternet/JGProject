#include "pch.h"
#include "MenuItem.h"


#include "GUI/JGUIComponent/Element/JGUIRenderer.h"
#include "GUI/JGUIComponent/Element/JGUICanvas.h"

#include "GUI/JGUIObject/JGUIElement.h"
#include "GUI/JGUIObject/JGUIWindow.h"

#include "GUI/JGUIComponent/Script/Text.h"
using namespace std;

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

	// Item 여는 법은 두가지  마우스를 가만히 대고 있거나
	// Item을 마우스 다운하거나
	GetOwner()->BindMouseBtDownFunc([&](const JGUIKeyDownEvent& e)
	{
		m_IsMouseDown = true;
	});
	GetOwner()->BindMouseBtUpFunc([&](const JGUIKeyUpEvent& e)
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



			}
			// 부모 MenuItem 에게 다른 아이템은 닫아달라 말한다.
			OpenItems();
		}
	});



	CloseItems();
}

void MenuItem::Destroy()
{

}

void MenuItem::Tick(float tick)
{
	if (m_IsMouseDown && JGUI::GetKeyUp(GetOwnerWindow(), KeyCode::LeftMouseButton))
	{
		m_IsMouseDown = false;
	}



	if (GetOwner()->GetCollider()->CheckInPoint(GetOwnerWindow()->GetMousePos()) || m_IsMouseDown)
	{
		m_Image->SetColor(m_HightlightColor);
	}
	else
	{
		m_Image->SetColor(m_NormalColor);
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
	m_IsDirty = true;
	return item;
}
MenuItem* MenuItem::GetItem(uint32_t idx) const
{
	return m_Items->GetChild(idx)->FindComponent<MenuItem>();
}
MenuItem* MenuItem::GetItem(const std::string& name) const
{
	return nullptr;
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
	renderer->SetColor(m_SepColor);
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

	for (int i = 0; i < itemCount; ++i)
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
		m_Image->SetColor(m_NormalColor);
	}
	if (m_Desc.backGround_path != "none")
	{
		m_BackGround->SetColor(JColor::White());
		m_BackGround->SetImage(m_Desc.backGround_path);
	
	}
	else
	{
		m_BackGround->SetColor(m_NormalColor);
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
	float fontsize = size.y - (m_IconGap * 2);
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
	float offset_x = 0.0f;
	if (m_Icon)
	{
		float icon_size = size.y - (m_IconGap * 2);
		m_Icon->GetTransform()->SetLocalPosition(JGUI::Gap() + m_IconGap, m_IconGap);
		m_Icon->GetTransform()->SetSize(icon_size, icon_size);
	}
	offset_x += JGUI::Gap() + size.y + JGUI::Gap();


	// ContentsText
	float contentTextSize_x = m_ContentsTextWidth + JGUI::Gap() * 2;
	m_ContentsText->GetTransform()->SetLocalPosition(offset_x, 0.0f);
	m_ContentsText->GetTransform()->SetSize(contentTextSize_x, size.y);
	



	// ShortcutKey
	offset_x += contentTextSize_x;
	float shortcutKeySize_x = m_ShortcutKeyTextWidth + JGUI::Gap();
	m_ShortcutKeyText->GetTransform()->SetLocalPosition(offset_x, 0.0f);
	m_ShortcutKeyText->GetTransform()->SetSize(shortcutKeySize_x, size.y);
	offset_x += shortcutKeySize_x;

	// arrow
	float arrowsize = MenuItemDefaultHeight - (m_ArrowGap * 2);
	if (m_Arrow)
	{
		m_Arrow->GetTransform()->SetSize(arrowsize, arrowsize);
		m_Arrow->GetTransform()->SetLocalPosition(offset_x + m_ArrowGap, m_ArrowGap);
	}
	offset_x += MenuItemDefaultHeight;

	// Text 설정
	m_ContentsText->SetFontSize(fontsize);
	m_ContentsText->SetVAlign(JGUI_Text_VAlignment_Center);
	m_ContentsText->SetText(contents_text);


	m_ShortcutKeyText->SetFontSize(size.y - (m_IconGap * 2));
	m_ShortcutKeyText->SetVAlign(JGUI_Text_VAlignment_Center);


	// 메뉴 단축키
	if (!m_Desc.shortcut_key.empty())
	{
		m_ShortcutKeyText->SetText(shortcut_key_text);
	}




	m_BackGround->GetTransform()->AttachTransform(m_BackGroundBorder->GetTransform());
	m_BackGroundBorder->CreateEmptyRectangle(m_BorderThick);
	m_BackGroundBorder->SetColor(m_BorderColor);


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
	//float arrow_area_size = 
	float item_size = icon_area_size + contents_area_size + shortcut_area_size + arrow_area_size;


	auto owner_size = GetTransform()->GetSize();
	m_BackGround->GetTransform()->SetSize(item_size + (item_gap * 2), (sepCount * m_SepHeight) + (itemCount * MenuItemDefaultHeight) + (item_gap * 2));
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
			icon_area_size + contents_area_size + shortcut_area_size + m_ArrowGap,
			m_ArrowGap);



		offset_y += MenuItemDefaultHeight;

		//
		if (m_SepIndexMap.find(i) != m_SepIndexMap.end())
		{
			// Icon ------
			m_SepIndexMap[i]->GetTransform()->SetLocalPosition(owner_size.x + item_gap + offset_x + icon_area_size, item_gap + offset_y + m_SepGap);
			m_SepIndexMap[i]->GetTransform()->SetSize(contents_area_size + shortcut_area_size - m_SepGap, m_SepHeight - (m_SepGap * 2));
			offset_y += m_SepHeight;
		}
	}
}
