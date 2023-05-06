#pragma once
#include "WidgetComponents/WidgetComponent.h"



class IListItem
{
public:
	virtual PSharedPtr<WWidgetComponent> CreateWidgetComponent() = 0;
	virtual void OnSelected() = 0;
};

class WList : public WWidgetComponent
{
private:
	HList<PSharedPtr<IListItem>> _itemList;

	bool  _bAllowMultiSelected  = false;
	int32 _selectedItemIndex    = INDEX_NONE;

public:
	WList() : WWidgetComponent() {}
	virtual ~WList() = default;


public:
	template<class ListItemType>
	void SetItemList(const HList<PSharedPtr<ListItemType>>& inItemList)
	{
		HList<PSharedPtr<IListItem>> itemList;
		for (const ListItemType& item : inItemList)
		{
			itemList.push_back(item);
		}

		SetItemList(inItemList);
	}

	void SetItemList(const HList<PSharedPtr<IListItem>>& inItemList);
	void SetSelectedItemIndex(int32 inIndex);
	void SetSelectedItem(PSharedPtr<IListItem> inItem);
	void SetAllowMultiSelected(bool bInAllowMultiSelected);

protected:
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override;
};