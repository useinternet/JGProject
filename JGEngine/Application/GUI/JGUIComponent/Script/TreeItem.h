#pragma once



#include "GUI/JGUIComponent/JGUIElementComponent.h"

class Button;
class Text;
class JGUIImageRenderer;

class TreeView;
class TreeItemDesc
{
public:
	std::string name       = "TreeItem";
	std::string openImage  = "Down_FillArrow.png";
	std::string closeImage = "Right_FillArrow.png";
	std::string iconImage  = "none";
};


class JGENGINE_API TreeItem : public JGUIScriptComponent
{
protected:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Destroy() override;
	virtual void Tick(float tick) override;

public:
	TreeItem* AddItem(const TreeItemDesc& desc);
	TreeItem* InsertItem(uint32_t idx, const std::string& name);
	void      SwapItem(uint32_t idx1, uint32_t idx2);
	void      SwapItem(TreeItem* item1, TreeItem* item2);
	void      RemoveItem(const std::string& name);
	TreeItem* GetItem(uint32_t idx) const;
	TreeItem* GetItem(const std::string& name) const;
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
	JGUIElement* m_TreeItems = nullptr;
	Button*      m_OpenBt    = nullptr;
	Text*        m_TreeText  = nullptr;
	JGUIImageRenderer* m_Icon = nullptr;



	std::map<std::string, TreeItem*> m_ItemsByName;
	TreeItemDesc m_Desc;


	bool  m_IsDirty = false;

	float m_BtSize = 18.0f;
	float m_OpenGap = 3.0f;
	float m_Gap = 1.0f;
};