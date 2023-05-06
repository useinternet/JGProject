#pragma once
#include "Core.h"
#include "GUIDefine.h"
#include "Builder/GUIBuilder.h"
#include "Builder/MenuBuilder.h"
#include "Builder/ContextMenuBuilder.h"
#include "Runtime/CodeGen/Module.GUI.Widget.generation.h"
JGCLASS()
class WWidget : public JGObject 
{
	JG_GENERATED_CLASS_BODY

	JGPROPERTY()
	HGuid _widgetGuid;

	JGPROPERTY()
	bool _bIsOpen;

	EWidgetFlags _widgetFlags;

protected:
	bool _bAllowUpdate;

public:
	WWidget();
	virtual ~WWidget() = default;

	const HGuid& GetGuid() const;

	void SetWidgetFlags(EWidgetFlags flags);
	void AddWidgetFlags(EWidgetFlags flags);
	void RemoveWidgetFlags(EWidgetFlags flags);
	EWidgetFlags GetWidgetFlags() const;

protected:
	friend class GGUIGlobalSystem;
	virtual void Construct() override;
	virtual void OnConstruct() {}
	virtual void OnUpdate() {}
	virtual void OnOpen()  {}
	virtual void OnClose() {}
public:
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) {}
	virtual void OnMenuBuild(HMenuBuilder& inBuilder) {}
	virtual void OnContextMenuBuild(HContextMenuBuilder& inBuilder) {}
};