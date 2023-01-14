#pragma once
#include "Core.h"
#include "GUIDefine.h"
#include "Builder/GUIBuilder.h"
#include "Builder/MenuBuilder.h"

class WWidget : public IMemoryObject 
{
	PName _widgetName;

public:
	WWidget() = default;
	virtual ~WWidget() = default;

	void  SetName(PName inName);
	PName GetName() const;

protected:
	friend class GGUIGlobalSystem;
	virtual void OnConstruct() {}
	virtual void OnOpen()  {}
	virtual void OnClose() {}
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) {}
	virtual void OnMenuBuild(HMenuBuilder& inBuilder) {}
};