#pragma once
#include "Core.h"
#include "GUIDefine.h"
#include "External/Classes/ImGuiWidgetComponentGenerator.h"

class HGUIBuilder;
enum class EReply
{
	Handled,
	UnHandled,
};

class WWidgetComponent
	: public IMemoryObject
	, public IImGuiWidgetComponentGenerator
{
private:
	HGuid _guid;

public:
	JG_DECLARE_DELEGATE_RETVAL(EWidgetVisibility, PIsVisibility);
	JG_DECLARE_DELEGATE_RETVAL(bool, PIsEnable);

	PIsVisibility IsVisiblity;
	PIsEnable	  IsEnable;

public:
	WWidgetComponent();
	virtual ~WWidgetComponent() = default;

public:
	const HGuid& GetGuid() const;

protected:
	virtual EReply OnMouseMove()  {}
	virtual EReply OnMouseHover() {}
	virtual EReply OnMouseLeave() {}
	virtual EReply OnMouseButtonDown()  {}
	virtual EReply OnMouseButtonUp()    {}
	virtual EReply OnMouseButtonClick() {}
	virtual EReply OnMouseButtonDoubleClick() {}

	// Begin IImGuiWidgetComponentGenerator
protected:
	virtual void GenerateImGuiWidgetComponent(const HWidgetContext& inWidgetContext) override {}
	// End   IImGuiWidgetComponentGenerator
};