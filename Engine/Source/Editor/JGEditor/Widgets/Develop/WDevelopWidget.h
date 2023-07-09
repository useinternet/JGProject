#pragma once
#include "Widget.h"
#include "JGEditorDefine.h"
#include "Runtime/CodeGen/Module.JGEditor.WDevelopWidget.generation.h"

class WText;
class WButton;
class JGDevelopUnit;
class WDevelopUnitExecuter;
class WDevelopUnitList;
class WDevelopUnitContent;

JGCLASS()
class JGEDITOR_API WDevelopWidget : public WWidget
{
	JG_GENERATED_CLASS_BODY

	PSharedPtr<WDevelopUnitExecuter> _developUnitExecuter;
	PSharedPtr<WDevelopUnitList>     _developUnitList;
	PSharedPtr<WDevelopUnitContent>  _developUnitContent;
public:
	WDevelopWidget() = default;
	virtual ~WDevelopWidget() = default;

	virtual void OnOpen() override;
	virtual void OnClose() override;
	virtual void OnUpdate() override;
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override;
	virtual void OnMenuBuild(HMenuBuilder& inBuilder) override;
	virtual void OnContextMenuBuild(HContextMenuBuilder& inBuilder) override;
};