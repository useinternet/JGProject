#pragma once
#include "GUI/JGUIComponent/JGUIElementComponent.h"
#include "GUI/JGUIComponent/Script/TreeItem.h"



class JGUIImageRenderer;
class JGENGINE_API TreeView : public JGUIScriptComponent
{
protected:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Destroy() override;
	virtual void Tick(float tick) override;

public:
	TreeItem* CreateRoot(const TreeItemDesc& desc);
	TreeItem* GetSelectedItem() const;
private:
	TreeItem* TrackingSelectedItem(TreeItem* item, uint32_t& select_index);
private:
	TreeItem* m_Root         = nullptr;
	TreeItem* m_SelectedItem = nullptr;
	JGUIImageRenderer* m_SelectImage = nullptr;





	bool m_IsMouseDown = false;
	float m_TreeHeight = 18.0f;
};