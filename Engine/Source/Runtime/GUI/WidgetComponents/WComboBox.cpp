#include "PCH/PCH.h"
#include "WComboBox.h"
#include "Builder/GUIBuilder.h"
#include "External/imgui/imgui.h"
#include "External/imgui/imgui_internal.h"


void WComboBox::Construct(const HArguments& inArgs)
{
	_onSelectChanged = inArgs.OnSelectChanged;
}

void WComboBox::SetSelectedItemIndex(int32 inIndex)
{
	bool bDirty = _selectedItemIndex != inIndex;

	_selectedItemIndex = inIndex;

	if (bDirty)
	{
		if (_selectedItemIndex != INDEX_NONE || _selectedItemIndex < _itemList.size())
		{
			_itemList[_selectedItemIndex]->OnSelected();
			_onSelectChanged.ExecuteIfBound(_itemList[_selectedItemIndex]);
		}
	}
}

void WComboBox::SetSelectedItem(PSharedPtr<IComboItem> inItem)
{
	int32 index = 0;
	for (PSharedPtr<IComboItem> item : _itemList)
	{
		if (inItem == item)
		{
			SetSelectedItemIndex(index);
			break;
		}

		++index;
	}
}

int32 WComboBox::GetSelectedItemIndex() const
{
	return _selectedItemIndex;
}

int32 WComboBox::GetItemIndex(PSharedPtr<IComboItem> inItem) const
{
	int32 index = 0;
	for (PSharedPtr<IComboItem> item : _itemList)
	{
		if (inItem == item)
		{
			return index;
		}

		++index;
	}

	return INDEX_NONE;
}

PSharedPtr<IComboItem> WComboBox::GetSelectedItem() const
{
	if (_selectedItemIndex != INDEX_NONE || _selectedItemIndex < _itemList.size())
	{
		return _itemList[_selectedItemIndex];
	}

	return nullptr;
}

void WComboBox::OnGUIBuild(HGUIBuilder& inBuilder)
{
	inBuilder.PushGenerateNativeGUI(SharedWrap(this), HOnGenerateNativeGUI::CreateLambda([&](const HWidgetContext& widgetContext)
		{
			PSharedPtr<IComboItem> SelectedItem = GetSelectedItem();

			PString label = PString("##") + GetGuid().ToString();
			PString previewLabel = "None";

			if (SelectedItem != nullptr)
			{
				previewLabel = SelectedItem->GetLabel();
			}
	
			if (ImGui::BeginCombo(label.GetCStr(), previewLabel.GetCStr(), ImGuiComboFlags_None))
			{
				int32 itemIndex = 0;
				for (PSharedPtr<IComboItem> comboItem : _itemList)
				{
					if (ImGui::Selectable(comboItem->GetLabel().GetCStr()))
					{
						comboItem->OnSelected();
						SetSelectedItemIndex(itemIndex);
					}

					++itemIndex;
				}
	
				ImGui::EndCombo();
			}
		}));
}

void WComboBox::setItemListInternal(const HList<PSharedPtr<IComboItem>>& inItemList)
{
	_itemList = inItemList;
}
