#pragma once
#include "Components/NativeScriptComponent.h"
#include "TreeItem.h"


namespace JGUI
{

	class ImageComponent;
	class GUI_API TreeView : public NativeScriptComponent
	{
	protected:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Destroy() override;
		virtual void Tick() override;

	public:
		TreeItem* CreateRoot(const TreeItemDesc& desc);
		TreeItem* GetSelectedItem() const;
	private:
		TreeItem* TrackingSelectedItem(TreeItem* item, uint32_t& select_index);
	private:
		TreeItem* m_Root = nullptr;
		TreeItem* m_SelectedItem = nullptr;
		ImageComponent* m_SelectImage = nullptr;

		bool  m_IsMouseDown = false;
		float m_TreeHeight  = GUIIF::GetStyle().ButtonSize.y;
	};

}
