#pragma once
#include "WidgetComponents/WidgetComponent.h"



class IListItem
{
public:
	virtual PSharedPtr<WWidgetComponent> CreateWidgetComponent() = 0;
	virtual void OnSelected() = 0;
	virtual void OnDeselected() = 0;
};

class WList : public WWidgetComponent
{
private:
	HList<PSharedPtr<IListItem>> _itemList;
	HSet<int32> _selectedItemIndexes;

	bool  _bAllowMultiSelected  = false;
	

public:
	WList() : WWidgetComponent() {}
	virtual ~WList() = default;

public:
	template<class ListItemType>
	void SetItemList(const HList<PSharedPtr<ListItemType>>& inItemList)
	{
		HList<PSharedPtr<IListItem>> itemList;
		for (const PSharedPtr<ListItemType>& item : inItemList)
		{
			itemList.push_back(item);
		}

		setItemListInternal(itemList);
	}

	void SelectItemIndex(int32 inIndex);
	void SelectItem(PSharedPtr<IListItem> inItem);

	void DeselectItemIndex(int32 inIndex);
	void DeselectItem(PSharedPtr<IListItem> inItem);
	void SetAllowMultiSelected(bool bInAllowMultiSelected);

private:
	void setItemListInternal(const HList<PSharedPtr<IListItem>>& inItemList);

protected:
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override;
};