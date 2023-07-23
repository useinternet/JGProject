#include "PCH/PCH.h"
#include "WList.h"
#include "WSelectable.h"
#include "Builder/GUIBuilder.h"

void WList::Construct(const HArguments& inArgs)
{
	//_onSelectItem = inArgs.OnSelectItem;
	_onSelectChanged = inArgs.OnSelectChanged;
	_onGenerateWidgetComponent = inArgs.OnGenerateWidgetComponent;
}

void WList::SelectItemIndex(int32 inIndex)
{
	bool bDirty = _selectedItemIndexes.contains(inIndex) == false;
	if (bDirty == false)
	{
		return;
	}

	if (_bAllowMultiSelected == false)
	{
		HSet<int32> TempSelectedItemIndexes = _selectedItemIndexes;
		for (int32 ItemIndex : TempSelectedItemIndexes)
		{
			DeselectItemIndex(ItemIndex);
		}

		_selectedItemIndexes.clear();
	}

	_selectedItemIndexes.insert(inIndex);

	if (inIndex != INDEX_NONE || inIndex < _itemList.size())
	{
		_onSelectChanged.ExecuteIfBound(_itemList[inIndex], true);
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

	_onSelectChanged.ExecuteIfBound(_itemList[inIndex], false);
	_selectedItemIndexes.erase(inIndex);
	//_itemList[inIndex]->OnDeselected();
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

bool WList::IsSelectedItem(PSharedPtr<IListItem> inItem) const
{
	for (int32 Index : _selectedItemIndexes)
	{
		if (inItem == _itemList[Index])
		{
			return true;
		}
	}

	return false;
}

HList<PSharedPtr<IListItem>> WList::GetSelectedItems() const
{
	HList<PSharedPtr<IListItem>> results;
	for (int32 Index : _selectedItemIndexes)
	{
		results.push_back(_itemList[Index]);
	}

	return results;
}

HList<int32> WList::GetSelectedItemIndexes() const
{
	HList<int32> results;
	for (int32 Index : _selectedItemIndexes)
	{
		results.push_back(Index);
	}

	return results;
}

int32 WList::GetItemIndex(PSharedPtr<IListItem> inItem) const
{
	int32 index = INDEX_NONE;
	for (PSharedPtr<IListItem> Item : _itemList)
	{
		++index;

		if (Item == inItem)
		{
			return index;
		}
	}

	return INDEX_NONE;
}

void WList::SetItemListInternal(const HList<PSharedPtr<IListItem>>& inItemList)
{
	_itemList = inItemList;
	_widgetList.clear();
	_widgetList.resize(_itemList.size());

	int32 numItem = (int32)_itemList.size();
	for (int32 i = 0; i < numItem; ++i)
	{
		WSelectable::HArguments args;
		args.OnSelected = WSelectable::HOnSelected::CreateSP(SharedWrap(this), &WList::OnItemSelected, i);
		args.OnContent  = WSelectable::HOnContent::CreateSP(SharedWrap(this), &WList::OnItemContent, i);

		_widgetList[i] = NewWidgetComponent<WSelectable>(args);
	}
}

void WList::OnItemSelected(int32 inSelectIndex)
{
	SelectItemIndex(inSelectIndex);
}

PSharedPtr<WWidgetComponent> WList::OnItemContent(int32 inSelectIndex)
{
	return _onGenerateWidgetComponent.ExecuteIfBound(_itemList[inSelectIndex]);
}

void WList::OnGUIBuild(HGUIBuilder& inBuilder)
{
	int32 numItem = (int32)_itemList.size();
	for (int32 i = 0; i < numItem; ++i)
	{
		if (_widgetList[i] == nullptr)
		{
			continue;
		}

		inBuilder.PushWidgetComponent(_widgetList[i]);
	}
}