#pragma once
#include "Core.h"
#include "GUIDefine.h"
#include "Builder/GUIBuilder.h"
#include "Builder/MenuBuilder.h"

class WWidget : public IMemoryObject 
{
	PName _widgetName;
	HGuid _widgetGuid;

	EWidgetFlags _widgetFlags;

	bool _bIsOpen;
public:
	WWidget() = default;
	virtual ~WWidget() = default;

	void  SetName(PName inName);
	PName GetName() const;

	const HGuid& GetGuid() const;

	void SetWidgetFlags(EWidgetFlags flags);
	void AddWidgetFlags(EWidgetFlags flags);
	void RemoveWidgetFlags(EWidgetFlags flags);
	EWidgetFlags GetWidgetFlags() const;

protected:
	friend class GGUIGlobalSystem;
	virtual void Construct() override;
	virtual void OnConstruct() {}
	virtual void OnOpen()  {}
	virtual void OnClose() {}
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) {}
	virtual void OnMenuBuild(HMenuBuilder& inBuilder) {}
};