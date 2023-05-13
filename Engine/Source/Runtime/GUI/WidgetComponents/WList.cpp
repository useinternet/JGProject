#include "PCH/PCH.h"
#include "WList.h"
#include "Builder/GUIBuilder.h"


void WList::SetItemList(const HList<PSharedPtr<IListItem>>& inItemList)
{
	_itemList = inItemList;
}

void WList::SetSelectedItemIndex(int32 inIndex)
{
	bool bDirty = _selectedItemIndex != inIndex;

	_selectedItemIndex = inIndex;

	if (bDirty)
	{
		if (_selectedItemIndex != INDEX_NONE || _selectedItemIndex < _itemList.size())
		{
			_itemList[_selectedItemIndex]->OnSelected();
		}
	}
}

void WList::SetSelectedItem(PSharedPtr<IListItem> inItem)
{
	int32 index = 0;
	for (PSharedPtr<IListItem> item : _itemList)
	{
		if (inItem == item)
		{
			SetSelectedItemIndex(index);
			break;
		}

		++index;
	}
}

void WList::SetAllowMultiSelected(bool bInAllowMultiSelected)
{
	_bAllowMultiSelected = bInAllowMultiSelected;
}

void WList::OnGUIBuild(HGUIBuilder& inBuilder)
{
	for (PSharedPtr<IListItem> item : _itemList)
	{
		PSharedPtr<WWidgetComponent> widgetComp = item->CreateWidgetComponent();
		inBuilder.PushWidgetComponent(widgetComp);
	}
}