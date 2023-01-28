#pragma once
#include "Core.h"
#include "GUIDefine.h"
#include "External/Classes/ImGuiWidgetComponentGenerator.h"

class HGUIBuilder;
class WWidgetComponent
	: public IMemoryObject
	, public IGUIEventReceiver
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
	virtual EReply OnMouseMove() override { return EReply::UnHandled; }
	virtual EReply OnMouseHover() override { return EReply::UnHandled; }
	virtual EReply OnMouseLeave() override { return EReply::UnHandled; }
	virtual EReply OnMouseLButtonDown() override { return EReply::UnHandled; }
	virtual EReply OnMouseRButtonDown() override { return EReply::UnHandled; }
	virtual EReply OnMouseMButtonDown() override { return EReply::UnHandled; }
	virtual EReply OnMouseLButtonUp() override { return EReply::UnHandled; }
	virtual EReply OnMouseRButtonUp() override { return EReply::UnHandled; }
	virtual EReply OnMouseMButtonUp() override { return EReply::UnHandled; }
	virtual EReply OnMouseLButtonClick() override { return EReply::UnHandled; }
	virtual EReply OnMouseRButtonClick() override { return EReply::UnHandled; }
	virtual EReply OnMouseMButtonClick() override { return EReply::UnHandled; }
	virtual EReply OnMouseLButtonDoubleClick() override { return EReply::UnHandled; }
	virtual EReply OnMouseRButtonDoubleClick() override { return EReply::UnHandled; }
	virtual EReply OnMouseMButtonDoubleClick() override { return EReply::UnHandled; }

	// Begin IImGuiWidgetComponentGenerator
protected:
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) {}
	virtual void GenerateImGuiWidgetComponent(const HWidgetContext& inWidgetContext) override {}
	// End   IImGuiWidgetComponentGenerator
};