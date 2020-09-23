#include "pch.h"
#include "TreeView.h"

#include "Elements/Element.h"
#include "Windows/Window.h"
#include "Components/LayerComponent.h"
#include "Components/DrawComponent.h"
#include "Components/Script/Border.h"
#include "Button.h"


namespace JGUI
{
	void TreeView::Awake()
	{
		GetOwner()->CreateComponent<ImageComponent>()->SetColor(JColor(0,0,0,0));
		
		m_SelectImage = GetOwner()->CreateElement<Element>(TT("SelectImage"))->CreateComponent<ImageComponent>();
		m_SelectImage->GetOwner()->SetActive(false);


		OnMouseButtonDown = [&](KeyCode code)
		{
			if (code != KeyCode::LeftMouseButton) return;
			if (m_Root == nullptr) return;
			JVector2 mouse_pos = GetOwnerWindow()->GetMousePos();
			JRect    owner_rect = GetTransform()->GetRect();


			float delta = (float)mouse_pos.y - owner_rect.top;
			uint32_t select_index = (uint32_t)(delta / m_TreeHeight);
			uint32_t count = select_index;
			m_SelectedItem = TrackingSelectedItem(m_Root, count);

			if (m_SelectedItem)
			{
				m_SelectImage->GetOwner()->SetActive(true);
				m_SelectImage->SetColor(JColor::Blue());
				m_SelectImage->GetTransform()->SetLocalLocation(0.0f, select_index * m_TreeHeight);
			}
			else
			{
				m_SelectImage->GetOwner()->SetActive(false);
			}

		};
		OnResize = [&](float width, float height)
		{
			m_SelectImage->GetTransform()->SetSize(width, m_TreeHeight);
		};


	}

	void TreeView::Start()
	{



	}

	void TreeView::Destroy()
	{


	}

	void TreeView::Tick()
	{
		if (m_SelectedItem && !m_SelectedItem->IsActive())
		{
			auto parent = m_SelectedItem->GetTreeParent();
			uint32_t index = parent->GetItemIndex(m_SelectedItem) + 1;
			m_SelectedItem = parent;
			m_SelectImage->GetTransform()->OffsetLocalLocation(0.0f, -(index * m_TreeHeight));

		}


	}

	TreeItem* TreeView::CreateRoot(const TreeItemDesc& desc)
	{
		if (m_Root) return m_Root;
		m_Root = GetOwner()->CreateElement<Element>(desc.name)->CreateComponent<TreeItem>();
		m_Root->SetDesc(desc);
		m_Root->GetTransform()->SetSize(100, m_TreeHeight);
		return m_Root;
	}

	TreeItem* TreeView::GetSelectedItem() const
	{
		return m_SelectedItem;
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

}

