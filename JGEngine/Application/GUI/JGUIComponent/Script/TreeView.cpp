#include "pch.h"
#include "TreeView.h"

#include "GUI/JGUIObject/JGUIElement.h"
#include "GUI/JGUIObject/JGUIWindow.h"

#include "GUI/JGUIComponent/Element/JGUIRenderer.h"

#include "GUI/JGUIComponent/Script/Button.h"
#include "GUI/JGUIComponent/Script/Text.h"

void TreeView::Awake()
{
	if (GetOwner()->GetCollider() == nullptr) GetOwner()->RegisterCollider(JGUI_Collider_Box);
	m_SelectImage = GetOwner()->CreateJGUIElement("SelectImage")->CreateJGUIComponent<JGUIImageRenderer>();
	m_SelectImage->GetOwner()->SetActive(false);



	GetOwner()->BindMouseBtDownFunc([&](const JGUIKeyDownEvent& e)
	{
		if (e.Code != KeyCode::LeftMouseButton) return;
		if (m_Root == nullptr) return;
		if (m_IsMouseDown) return;



		m_IsMouseDown = true;
		JVector2Int mouse_pos = GetOwnerWindow()->GetMousePos();
		JGUIRect    owner_rect = GetTransform()->GetRect();


		float delta = (float)mouse_pos.y - owner_rect.top;
		uint32_t select_index = (uint32_t)(delta / m_TreeHeight);
		uint32_t count = select_index;
		m_SelectedItem = TrackingSelectedItem(m_Root, count);

		if (!m_SelectImage->GetOwner()->IsActive())
		{
			m_SelectImage->GetOwner()->SetActive(true);
		}
		if (m_SelectedItem)
		{
			ENGINE_LOG_INFO("Select Tree : {0}", m_SelectedItem->GetOwner()->GetName());
			m_SelectImage->SetColor(JColor::Blue());

			m_SelectImage->GetTransform()->SetLocalPosition(0.0f, select_index * m_TreeHeight);
		}

	});


	GetOwner()->BindResizeFunc([&](const JGUIResizeEvent& e)
	{
		m_SelectImage->GetTransform()->SetSize(e.width, m_TreeHeight);
	});
}

void TreeView::Start()
{



}

void TreeView::Destroy()
{


}

void TreeView::Tick(float tick)
{
	if (m_IsMouseDown && JGUI::GetKeyUp(GetOwnerWindow(), KeyCode::LeftMouseButton))
	{
		m_IsMouseDown = false;
	}
	if (m_SelectedItem && !m_SelectedItem->IsActive())
	{
		auto parent = m_SelectedItem->GetTreeParent();
		uint32_t index = parent->GetItemIndex(m_SelectedItem) + 1;
		m_SelectedItem = parent;
		m_SelectImage->GetTransform()->OffsetLocalPosition(0.0f, -(index * m_TreeHeight));

	}


}

TreeItem* TreeView::CreateRoot(const TreeItemDesc& desc)
{
	if (m_Root) return m_Root;
	m_Root = GetOwner()->CreateJGUIElement(desc.name)->CreateJGUIComponent<TreeItem>();
	m_Root->SetDesc(desc);
	m_Root->GetTransform()->SetSize(100, m_TreeHeight);
	return m_Root;
}

TreeItem* TreeView::GetSelectedItem() const
{
	return nullptr;
}

// 5
TreeItem* TreeView::TrackingSelectedItem(TreeItem* item, uint32_t& select_index)
{
	if (select_index == 0)
	{
		return item;
	}


	// 자기 자신 감면
	--select_index;


	if (item->IsOpenItems())
	{
		for (uint32_t i = 0; i < item->GetItemCount(); ++i)
		{
			auto tracking_item = TrackingSelectedItem(item->GetItem(i), select_index);
			if (tracking_item) return tracking_item;
		}
	}




	return nullptr;
}
