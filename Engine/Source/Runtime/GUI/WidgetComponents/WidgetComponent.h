#pragma once
#include "Core.h"
#include "GUIDefine.h"
#include "WidgetContext.h"
#include "Builder/MenuBuilder.h"
#include "Builder/ContextMenuBuilder.h"

class HGUIBuilder;
class HContextMenuBuilder;
class WWidgetComponent
	: public IMemoryObject
	, public IGUIEventReceiver
{
private:
	static HGuid _classGuid;
	HGuid _guid;

public:
	JG_DECLARE_DELEGATE_RETVAL(EWidgetVisibility, HIsVisibility);
	JG_DECLARE_DELEGATE_RETVAL(bool, HIsEnable);

	HIsVisibility IsVisiblity;
	HIsEnable	  IsEnable;

public:
	WWidgetComponent();
	virtual ~WWidgetComponent() = default;

public:
	const HGuid& GetGuid() const;

protected:
	virtual void OnMouseMove() override 
	{
		//JG_LOG(GUI, ELogLevel::Info, "[ %s ]: OnMouseMove", _guid.ToString());
	}

	virtual void OnMouseHover() override
	{
		//JG_LOG(GUI, ELogLevel::Info, "[ %s ]: OnMouseHover", _guid.ToString());
	}

	virtual void OnMouseEnter() override
	{
		//JG_LOG(GUI, ELogLevel::Info, "[ %s ]: OnMouseEnter", _guid.ToString());
	}

	virtual void OnMouseLeave() override
	{
		//JG_LOG(GUI, ELogLevel::Info, "[ %s ]: OnMouseLeave", _guid.ToString());
	}
	virtual void OnMouseLButtonDown() override
	{
		//JG_LOG(GUI, ELogLevel::Info, "[ %s ]: OnMouseLButtonDown", _guid.ToString());
	}

	virtual void OnMouseRButtonDown() override
	{
		//JG_LOG(GUI, ELogLevel::Info, "[ %s ]: OnMouseRButtonDown", _guid.ToString());
	}

	virtual void OnMouseMButtonDown() override
	{
		//JG_LOG(GUI, ELogLevel::Info, "[ %s ]: OnMouseMButtonDown", _guid.ToString());
	}

	virtual void OnMouseLButtonUp() override
	{
		//JG_LOG(GUI, ELogLevel::Info, "[ %s ]: OnMouseLButtonUp", _guid.ToString());
	}

	virtual void OnMouseRButtonUp() override
	{
		//JG_LOG(GUI, ELogLevel::Info, "[ %s ]: OnMouseRButtonUp", _guid.ToString());
	}

	virtual void OnMouseMButtonUp() override
	{
		//JG_LOG(GUI, ELogLevel::Info, "[ %s ]: OnMouseMButtonUp", _guid.ToString());
	}

	virtual void OnMouseLButtonClick() override
	{
		//JG_LOG(GUI, ELogLevel::Info, "[ %s ]: OnMouseLButtonClick", _guid.ToString());
	}

	virtual void OnMouseRButtonClick() override
	{
		//JG_LOG(GUI, ELogLevel::Info, "[ %s ]: OnMouseRButtonClick", _guid.ToString());
	}

	virtual void OnMouseMButtonClick() override
	{
		//JG_LOG(GUI, ELogLevel::Info, "[ %s ]: OnMouseMButtonClick", _guid.ToString());
	}

	virtual void OnMouseLButtonDoubleClick() override
	{
		//JG_LOG(GUI, ELogLevel::Info, "[ %s ]: OnMouseLButtonDoubleClick", _guid.ToString());
	}

	virtual void OnMouseRButtonDoubleClick() override
	{
		//JG_LOG(GUI, ELogLevel::Info, "[ %s ]: OnMouseRButtonDoubleClick", _guid.ToString());
	}

	virtual void OnMouseMButtonDoubleClick() override
	{
		//JG_LOG(GUI, ELogLevel::Info, "[ %s ]: OnMouseMButtonDoubleClick", _guid.ToString());
	}

public:
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) {}
	virtual void OnContextMenuBuild(HContextMenuBuilder& inBuilder) {}
};