#include "pch.h"
#include "TreeItem.h"
#include "GUI/JGUIObject/JGUIElement.h"
#include "GUI/JGUIObject/JGUIWindow.h"

#include "GUI/JGUIComponent/Element/JGUIRenderer.h"
#include "GUI/JGUIComponent/Script/Button.h"
#include "GUI/JGUIComponent/Script/Text.h"


void TreeItem::Awake()
{
	GetTransform()->SetSize(100, m_BtSize);
	m_TreeItems = GetOwner()->CreateJGUIElement("ChildTreeItems");




	auto openBt = GetOwner()->CreateJGUIElement("OpenBt");
	openBt->CreateJGUIComponent<JGUIImageRenderer>();
	m_OpenBt = openBt->CreateJGUIComponent<Button>();
	m_OpenBt->SetImage(m_Desc.closeImage);
	m_OpenBt->BindOnClick([&]()
	{
		if (m_TreeItems->IsActive()) CloseItems();
		else OpenItems();

	});
	m_TreeText = GetOwner()->CreateJGUIElement("Text")->CreateJGUIComponent<Text>();
	m_TreeText->SetVAlign(JGUI_Text_VAlignment_Center);



	m_Icon = GetOwner()->CreateJGUIElement("Icon")->CreateJGUIComponent<JGUIImageRenderer>();
	m_Icon->SetImage(m_Desc.iconImage);
	GetOwner()->BindResizeFunc([&](const JGUIResizeEvent& e)
	{
		Setting();
	});


}

void TreeItem::Start()
{
	Setting();
	CloseItems();
}

void TreeItem::Destroy()
{
}

void TreeItem::Tick(float tick)
{

	if (m_IsDirty)
	{

		m_IsDirty = false;
		uint32_t itemCount = GetItemCount();
		uint32_t openedItemCount = 1;
		for (uint32_t i = 0; i < itemCount; ++i)
		{
			auto item = GetItem(i);
			item->GetTransform()->SetLocalPosition(m_BtSize, m_BtSize * openedItemCount);

			openedItemCount += item->GetOpenedItemCount();
			++openedItemCount;
		}
	}
}

TreeItem* TreeItem::AddItem(const TreeItemDesc& desc)
{
	uint32_t  itemCount = GetItemCount();
	TreeItem* item = m_TreeItems->CreateJGUIElement(desc.name)->CreateJGUIComponent<TreeItem>();
	m_Desc = desc;
	item->Setting();
	m_ItemsByName[desc.name] = item;
	
	m_IsDirty = true;
	return item;
}

TreeItem* TreeItem::InsertItem(uint32_t idx, const std::string& name)
{
	m_IsDirty = true;
	return nullptr;
}
void      TreeItem::SwapItem(uint32_t idx1, uint32_t idx2)
{
	m_IsDirty = true;
	return;
}
void      TreeItem::SwapItem(TreeItem* item1, TreeItem* item2)
{
	m_IsDirty = true;
	return;
}
void TreeItem::RemoveItem(const std::string& name)
{
	m_IsDirty = true;
	return;
}

TreeItem* TreeItem::GetItem(uint32_t idx) const
{
	auto item = m_TreeItems->GetChild(idx);
	if (item) return item->FindComponent<TreeItem>();
	else return nullptr;
}

TreeItem* TreeItem::GetItem(const std::string& name) const
{
	if (m_ItemsByName.find(name) != m_ItemsByName.end()) return m_ItemsByName.at(name);
	else return nullptr;
}

uint32_t TreeItem::GetItemIndex(TreeItem* item)
{

	for (uint32_t i = 0; i < GetItemCount(); ++i)
	{
		if (item->GetOwner() == m_TreeItems->GetChild(i))
			return i;
	}
	return -1;
}

uint32_t TreeItem::GetItemCount() const
{
	return m_TreeItems->GetChildCount();
}


uint32_t TreeItem::GetOpenedItemCount()
{
	if (!IsOpenItems()) return 0;
	uint32_t itemCount = GetItemCount();
	uint32_t result = itemCount;
	for (uint32_t i = 0; i < itemCount; ++i)
	{
		auto item = GetItem(i);
		if(item->IsOpenItems()) result += item->GetItemCount();
	}
	return result;
}

void TreeItem::CloseItems()
{
	m_TreeItems->SetActive(false);
	m_OpenBt->SetImage(m_Desc.closeImage);


	if (GetTreeParent())
	{
		GetTreeParent()->SendDirty();
	}

}
void TreeItem::OpenItems()
{
	m_TreeItems->SetActive(true);
	m_OpenBt->SetImage(m_Desc.openImage);
	if (GetTreeParent())
	{
		GetTreeParent()->SendDirty();
	}
}

bool TreeItem::IsOpenItems()
{
	return m_TreeItems->IsActive();
}
TreeItem* TreeItem::GetTreeParent() const
{
	if (GetOwner()->GetParent() && GetOwner()->GetParent()->GetParent())
	{
		return GetOwner()->GetParent()->GetParent()->FindComponent<TreeItem>();
	}


	return nullptr;
}


void TreeItem::SetDesc(const TreeItemDesc& desc)
{
	m_Desc = desc;
	SetName(desc.name);
	if (IsOpenItems()) m_OpenBt->SetImage(desc.openImage);
	else m_OpenBt->SetImage(desc.closeImage);
	if (m_Icon)
	{
		m_Icon->SetImage(desc.iconImage);
		m_Icon->GetOwner()->SetActive(!(desc.iconImage == "none"));
	}
}

void TreeItem::Setting()
{
	auto owner_size = GetTransform()->GetSize();
	m_BtSize = owner_size.y;

	
	float offset_x = 0.0f;

	uint32_t ItemCount = GetItemCount();

	if (ItemCount == 0)
	{
		m_OpenBt->GetOwner()->SetActive(false);
	}
	else
	{
		m_OpenBt->GetOwner()->SetActive(true);
		m_OpenBt->GetTransform()->SetLocalPosition(offset_x + m_OpenGap, m_OpenGap);
		m_OpenBt->GetTransform()->SetSize(m_BtSize - (m_OpenGap * 2), m_BtSize - (m_OpenGap * 2));
	}
	offset_x += m_BtSize + m_Gap;



	m_Icon->GetTransform()->SetLocalPosition(offset_x, 0.0f);
	m_Icon->GetTransform()->SetSize(m_BtSize, m_BtSize);
	offset_x += m_BtSize + m_Gap;





	// 이미지 추가
	m_TreeText->GetTransform()->SetLocalPosition(offset_x, 0.0f);
	m_TreeText->GetTransform()->SetSize(1000.0f, m_BtSize);

	m_TreeText->SetFontSize(m_BtSize);
	m_TreeText->SetColor(JColor::White());
	m_TreeText->SetText(GetOwner()->GetName());

	if (m_Desc.iconImage == "none")
	{
		m_Icon->GetOwner()->SetActive(false);
	}

}

void TreeItem::SendDirty()
{
	m_IsDirty =  true;
}

bool TreeItem::IsDirty()
{
	return m_IsDirty;
}

