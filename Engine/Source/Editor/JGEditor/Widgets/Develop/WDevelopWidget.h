#pragma once
#include "Widget.h"
#include "JGEditorDefine.h"
#include "Datas/DevelopUnitListData.h"
#include "Runtime/CodeGen/Module.JGEditor.WDevelopWidget.generation.h"

class WText;
class WButton;
class JGDevelopUnit;
class WDevelopUnitExecuter;
class WDevelopUnitList;
class WDevelopUnitContent;
class PDevelopUnitItem;
class JGDevelopUnitListData;
class IListItem;

JGCLASS()
class JGEDITOR_API WDevelopWidget : public WWidget
{
	JG_GENERATED_CLASS_BODY

	JGPROPERTY()
	PSharedPtr<JGDevelopUnitListData> _developUnitListData;

	PSharedPtr<WDevelopUnitExecuter> _developUnitExecuter;
	PSharedPtr<WDevelopUnitList>     _developUnitList;
	PSharedPtr<WDevelopUnitContent>  _developUnitContent;

	PWeakPtr<PDevelopUnitItem> _selectedDevelopUnit;
public:
	WDevelopWidget();
	virtual ~WDevelopWidget() = default;

	virtual void OnOpen() override;
	virtual void OnClose() override;
	virtual void OnUpdate() override;
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override;
	virtual void OnMenuBuild(HMenuBuilder& inBuilder) override;
	virtual void OnContextMenuBuild(HContextMenuBuilder& inBuilder) override;

	PSharedPtr<PDevelopUnitItem> GetSelectedDevelopUnit() const;
private:
	void OnSelectedDevelopUnit(PSharedPtr<IListItem> inDevelopUnit, bool inSelected);
};