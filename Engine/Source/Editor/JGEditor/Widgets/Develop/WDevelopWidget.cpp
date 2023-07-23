#include "PCH/PCH.h"
#include "WDevelopWidget.h"
#include "WDUTComboBox.h"
#include "WDevelopUnitList.h"

#include "Misc/DevelopUnit.h"

#include "WidgetComponents/WText.h"
#include "WidgetComponents/WInputText.h"
#include "WidgetComponents/WInputScalar.h"
#include "WidgetComponents/WButton.h"
#include "WidgetComponents/WList.h"

class WDevelopUnitExecuter : public WWidget
{
public:
	struct HArguments
	{

	};

private:
	PSharedPtr<WInputText> _inputText;

public:
	void Construct(const HArguments& InArgs) 
	{
		SetWidgetFlags(EWidgetFlags::ChildWidget_Border);
	}
protected:

	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override
	{

	}

	virtual void OnContextMenuBuild(HContextMenuBuilder& inBuilder) override
	{

	}
};

class WDevelopUnitContent : public WWidget
{
public:
	struct HArguments
	{
		PWeakPtr<WDevelopWidget> OwnerWidget;
	};

private:
	PWeakPtr<WDevelopWidget> _ownerWidget;

public:
	void Construct(const HArguments& InArgs)
	{
		SetWidgetFlags(EWidgetFlags::ChildWidget_Border);

		_ownerWidget = InArgs.OwnerWidget;
	}

protected:
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override
	{
		if (_ownerWidget.IsValid())
		{
			PSharedPtr<PDevelopUnitItem> selectedItem = _ownerWidget.Pin()->GetSelectedDevelopUnit();
			if (selectedItem != nullptr)
			{
				JGDevelopUnit* developUnit = selectedItem->GetDevelopUnit();
				if (developUnit != nullptr)
				{
					inBuilder.PushWidgetComponent(developUnit->CreateContentWidgetComponent());
				}
			}
		}
	}

	virtual void OnContextMenuBuild(HContextMenuBuilder& inBuilder) override
	{

	}
};

void WDevelopWidget::OnOpen()
{
	_selectedDevelopUnit = nullptr;

	if (_developUnitExecuter == nullptr)
	{
		_developUnitExecuter = NewWidgetComponent<WDevelopUnitExecuter>();
	}

	if (_developUnitList == nullptr)
	{
		WDevelopUnitList::HArguments args;
		args.OnSelectChanged = WList::HOnSelectChanged::CreateSP(SharedWrap(this), &WDevelopWidget::OnSelectedDevelopUnit);
		_developUnitList = NewWidgetComponent<WDevelopUnitList>(args);
	}

	if (_developUnitContent == nullptr)
	{
		WDevelopUnitContent::HArguments args;
		args.OwnerWidget = SharedWrap(this);

		_developUnitContent = NewWidgetComponent<WDevelopUnitContent>(args);
	}
}

void WDevelopWidget::OnClose()
{
	//Reset();
}

void WDevelopWidget::OnUpdate()
{
	//// dll

	//try
	//{
	//	if (_developUnit != nullptr)
	//	{
	//		_developUnit->Update();
	//	}
	//}
	//catch (...)
	//{
	//	Reset();
	//}
}

void WDevelopWidget::OnGUIBuild(HGUIBuilder& inBuilder)
{
	inBuilder.BeginVertical();
	{
		inBuilder.PushChildWidget(_developUnitExecuter, HPushChildWidgetArguments(0.0f, 30));

		inBuilder.BeginHorizontal();
		{
			inBuilder.PushChildWidget(_developUnitList,   HPushChildWidgetArguments(350, 0.0f));
			inBuilder.PushChildWidget(_developUnitContent, HPushChildWidgetArguments(0.0f, 0.0f));
		}
		inBuilder.EndHorizontal();
	}
	inBuilder.EndVertical();
}

void WDevelopWidget::OnMenuBuild(HMenuBuilder& inBuilder)
{

}

void WDevelopWidget::OnContextMenuBuild(HContextMenuBuilder& inBuilder)
{

}

PSharedPtr<PDevelopUnitItem> WDevelopWidget::GetSelectedDevelopUnit() const
{
	if (_selectedDevelopUnit.IsValid() == false)
	{
		return nullptr;
	}

	return _selectedDevelopUnit.Pin();
}

void WDevelopWidget::OnSelectedDevelopUnit(PSharedPtr<IListItem> inDevelopUnit, bool inSelected)
{
	if (inSelected == false)
	{
		return;
	}

	if (_selectedDevelopUnit.IsValid() == false)
	{
		_selectedDevelopUnit = nullptr;
	}

	if (_selectedDevelopUnit.Pin() == inDevelopUnit)
	{
		return;
	}

	_selectedDevelopUnit = Cast<PDevelopUnitItem>(inDevelopUnit);
}
