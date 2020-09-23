#pragma once



#include "Components/NativeScriptComponent.h"


namespace JGUI
{
	class Button;
	class TextMeshComponent;
	class ImageComponent;

	class TreeView;
	class TreeItemDesc
	{
	public:
		std::wstring name       = TT("TreeItem");
		std::wstring openImage  = GUIIF::GetStyle().GUIImages[GUIStyle_Resource_DownFillArrow];
		std::wstring closeImage = GUIIF::GetStyle().GUIImages[GUIStyle_Resource_RightFillArrow];;
		std::wstring iconImage  = TT("none");
	};


	class GUI_API TreeItem : public NativeScriptComponent
	{
	protected:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Destroy() override;
		virtual void Tick() override;

	public:
		TreeItem* AddItem(const TreeItemDesc& desc);
		TreeItem* InsertItem(uint32_t idx, const std::wstring& name);
		void      SwapItem(uint32_t idx1, uint32_t idx2);
		void      SwapItem(TreeItem* item1, TreeItem* item2);
		void      RemoveItem(const std::wstring& name);
		TreeItem* GetItem(uint32_t idx) const;
		TreeItem* GetItem(const std::wstring& name) const;
		uint32_t  GetItemIndex(TreeItem* item);
		uint32_t  GetItemCount() const;
		uint32_t  GetOpenedItemCount();

		void CloseItems();
		void OpenItems();
		bool IsOpenItems();
		TreeItem* GetTreeParent() const;


		void SetDesc(const TreeItemDesc& desc);
	private:
		void Setting();
		void SendDirty();
		bool IsDirty();
	private:
		Element* m_TreeItems = nullptr;
		Button*  m_OpenBt = nullptr;
		TextMeshComponent* m_TreeText = nullptr;
		ImageComponent* m_Icon = nullptr;



		std::map<std::wstring, TreeItem*> m_ItemsByName;
		TreeItemDesc m_Desc;


		bool  m_IsDirty = false;

		float m_BtSize  =  GUIIF::GetStyle().ButtonSize.y;
		float m_OpenGap = 3.0f;
		float m_Gap     = 1.0f;
	};
}
