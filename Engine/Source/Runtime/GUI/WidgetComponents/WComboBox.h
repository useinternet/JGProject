#pragma once
#include "WidgetComponents/WidgetComponent.h"



class IComboItem
{
public:
	virtual const PString& GetLabel() const = 0;
	virtual void OnSelected() = 0;
};

class WComboBox : public WWidgetComponent
{
private:
	HList<PSharedPtr<IComboItem>> _itemList;
	int32 _selectedItemIndex = INDEX_NONE;
	bool _bOpenCombo        = false;
	bool _bRequestOpenCombo = false;

public:
	JG_DECLARE_DELEGATE_ONEPARAM(HOnSelectChanged, PSharedPtr<IComboItem>)

	HOnSelectChanged OnSelectChanged;
public:
	WComboBox() : WWidgetComponent() {}
	virtual ~WComboBox() = default;

public:
	template<class ComboItemType>
	void SetItemList(const HList<PSharedPtr<ComboItemType>>& inItemList)
	{
		HList<PSharedPtr<IComboItem>> itemList;
		for (const PSharedPtr<ComboItemType>& item : inItemList)
		{
			itemList.push_back(item);
		}

		setItemListInternal(itemList);
	}

	void SetSelectedItemIndex(int32 inIndex);
	void SetSelectedItem(PSharedPtr<IComboItem> inItem);

	int32 GetSelectedItemIndex() const;
	int32 GetItemIndex(PSharedPtr<IComboItem> inItem) const;
	PSharedPtr<IComboItem> GetSelectedItem() const;

protected:
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override;

private:
	void setItemListInternal(const HList<PSharedPtr<IComboItem>>& inItemList);
};