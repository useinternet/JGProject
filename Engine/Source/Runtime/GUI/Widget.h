#pragma once
#include "Core.h"
#include "GUIDefine.h"


class HGUIBuilder;
class WWidget : public IMemoryObject
{
	PName _widgetName;

public:
	WWidget();
	virtual ~WWidget() = default;
	virtual void BuildWidget(HGUIBuilder& inBuilder) {}

public:
	void SetName(const PName& inName);
	const PName& GetName() const;
};