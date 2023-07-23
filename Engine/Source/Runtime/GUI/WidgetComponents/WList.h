#pragma once
#include "WidgetComponents/WidgetComponent.h"



class IListItem
{
public:
	virtual ~IListItem() = default;
};

class WSelectable;
class WList : public WWidgetComponent
{
public:
	JG_DECLARE_DELEGATE(HOnSelectChanged, PSharedPtr<IListItem>, bool)
	JG_DECLARE_DELEGATE_RET(HOnGenerateWidgetComponent, PSharedPtr<WWidgetComponent>, PSharedPtr<IListItem>)

	struct HArguments
	{
		HOnSelectChanged OnSelectChanged;
		HOnGenerateWidgetComponent OnGenerateWidgetComponent;
	};

private:
	HList<PSharedPtr<IListItem>>   _itemList;
	HList<PSharedPtr<WSelectable>> _widgetList;
	HSet<int32> _selectedItemIndexes;

	bool  _bAllowMultiSelected  = false;

	HOnSelectChanged _onSelectChanged;
	HOnGenerateWidgetComponent _onGenerateWidgetComponent;
public:
	WList() : WWidgetComponent() {}
	virtual ~WList() = default;

	void Construct(const HArguments& inArgs);

public:
	template<class ListItemType>
	void SetItemList(const HList<PSharedPtr<ListItemType>>& inItemList)
	{
		HList<PSharedPtr<IListItem>> itemList;
		for (const PSharedPtr<ListItemType>& item : inItemList)
		{
			itemList.push_back(item);
		}

		SetItemListInternal(itemList);
	}

	void SelectItemIndex(int32 inIndex);
	void SelectItem(PSharedPtr<IListItem> inItem);

	void DeselectItemIndex(int32 inIndex);
	void DeselectItem(PSharedPtr<IListItem> inItem);
	void SetAllowMultiSelected(bool bInAllowMultiSelected);

	bool IsSelectedItem(PSharedPtr<IListItem> inItem) const;
	HList<PSharedPtr<IListItem>> GetSelectedItems() const;
	HList<int32> GetSelectedItemIndexes() const;
	int32 GetItemIndex(PSharedPtr<IListItem> inItem) const;

private:
	void SetItemListInternal(const HList<PSharedPtr<IListItem>>& inItemList);
	void OnItemSelected(int32 inSelectIndex);
	PSharedPtr<WWidgetComponent> OnItemContent(int32 inSelectIndex);

protected:
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override;
};