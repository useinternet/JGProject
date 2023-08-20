#include "PCH/PCH.h"
#include "WDevelopWidget.h"
#include "WDUTComboBox.h"
#include "WDevelopUnitList.h"
#include "WDevelopUnitContent.h"

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

WDevelopWidget::WDevelopWidget()
{
	_developUnitListData = Allocate<JGDevelopUnitListData>();
}

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
		args.DevelopUnitListData = _developUnitListData;
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

}

void WDevelopWidget::OnUpdate()
{
	if (_selectedDevelopUnit.IsValid())
	{
		JGDevelopUnit* devUnit = _selectedDevelopUnit.Pin()->GetDevelopUnit();
		if (devUnit != nullptr)
		{
			try
			{
				devUnit->Update();
			}
			catch (std::exception e)
			{

			}
			
		}
	}
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
