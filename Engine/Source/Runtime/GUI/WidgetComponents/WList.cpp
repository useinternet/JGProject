#include "PCH/PCH.h"
#include "WList.h"
#include "Builder/GUIBuilder.h"

void WList::SelectItemIndex(int32 inIndex)
{
	bool bDirty = _selectedItemIndexes.contains(inIndex) == false;

	if (_bAllowMultiSelected == false)
	{
		_selectedItemIndexes.clear();
	}

	_selectedItemIndexes.insert(inIndex);

	if (bDirty)
	{
		if (inIndex != INDEX_NONE || inIndex < _itemList.size())
		{
			_itemList[inIndex]->OnSelected();
		}

		if (_bAllowMultiSelected == false)
		{
			HSet<int32> TempSelectedItemIndexes = _selectedItemIndexes;
			for (int32 ItemIndex : TempSelectedItemIndexes)
			{
				if (ItemIndex == inIndex)
				{
					continue;
				}

				DeselectItemIndex(ItemIndex);
			}
		}
	}
}

void WList::SelectItem(PSharedPtr<IListItem> inItem)
{
	int32 index = 0;
	for (PSharedPtr<IListItem> item : _itemList)
	{
		if (inItem == item)
		{
			SelectItemIndex(index);
			break;
		}

		++index;
	}
}

void WList::DeselectItemIndex(int32 inIndex)
{
	if (inIndex == INDEX_NONE || inIndex >= _itemList.size())
	{
		return;
	}

	if (_selectedItemIndexes.contains(inIndex) == false)
	{
		return;
	}


	_selectedItemIndexes.erase(inIndex);
	_itemList[inIndex]->OnDeselected();
}

void WList::DeselectItem(PSharedPtr<IListItem> inItem)
{
	int32 index = 0;
	for (PSharedPtr<IListItem> item : _itemList)
	{
		if (inItem == item)
		{
			DeselectItemIndex(index);
			break;
		}

		++index;
	}
}

void WList::SetAllowMultiSelected(bool bInAllowMultiSelected)
{
	_bAllowMultiSelected = bInAllowMultiSelected;
}

void WList::setItemListInternal(const HList<PSharedPtr<IListItem>>& inItemList)
{
	_itemList = inItemList;
}

void WList::OnGUIBuild(HGUIBuilder& inBuilder)
{
	for (PSharedPtr<IListItem> item : _itemList)
	{
		PSharedPtr<WWidgetComponent> widgetComp = item->CreateWidgetComponent();
		inBuilder.PushWidgetComponent(widgetComp);
	}
}

