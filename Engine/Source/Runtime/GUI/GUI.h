#pragma once
#include "Core.h"
#include "Widget.h"

class IGUIBuild;
class IMenuBuild;
class IContextMenuBuild;
class HMenuBuilder;
class HContextMenuBuilder;
class JGGUIStyle;

JG_DECLARE_MULTICAST_DELEGATE(POnMenuBuild, HMenuBuilder&)
JG_DECLARE_MULTICAST_DELEGATE(POnContextBuild, HContextMenuBuilder&)

struct HGUIConstants
{
	static const PString MainWidgetID;
};

struct HGUIEvents
{
	POnMenuBuild    OnMenuBuild;
	POnContextBuild OnContextMenuBuild;
};

class GGUIGlobalSystem : public GGlobalSystemInstance<GGUIGlobalSystem>
{
	PSharedPtr<IMemoryObject> _memObject;

	HHashMap<JGType, PSharedPtr<JGGUIStyle>> _guiStylePool;

	HHashMap<HGuid, PSharedPtr<WWidget>> _widgetPool;
	HHashMap<PName,  HGuid> _widgetGuidRedirectByName;
	HHashMap<JGType, HGuid> _widgetGuidRedirectByType;

	HHashMap<HGuid, PName> _widgetNameRedirectByGuid;
	HHashMap<HGuid, PSharedPtr<WWidget>> _openWidgets;

	PSharedPtr<IGUIBuild>  _guiBuild;
	PSharedPtr<IMenuBuild> _menuBuild;
	PSharedPtr<IContextMenuBuild> _contextMenuBuild;

	bool _bMenuDirty;
public:
	HGUIEvents GUIEvents;

protected:
	virtual void Start() override;
	virtual void Destroy() override;
	void Update();
	void Build();
	void BuildGUI();
	void BuildMenu();
	void BuildContextMenu();

public:
	void SetGUIBuild(PSharedPtr<IGUIBuild> guiBuild);
	PSharedPtr<IGUIBuild> GetGUIBuild() const;

	void SetMenuBuild(PSharedPtr<IMenuBuild> inMenuBuild);
	PSharedPtr<IMenuBuild> GetMenuBuild() const;

	void SetContextMenuBuild(PSharedPtr<IContextMenuBuild> inContextMenuBuild);
	PSharedPtr<IContextMenuBuild> GetContextMenuBuild() const;

	template<class T>
	PSharedPtr<T> OpenWidget(PName widgetKeyName = NAME_NONE)
	{
		PSharedPtr<T> widget = nullptr;
		PSharedPtr<WWidget> baseWidget = nullptr;
		if (widgetKeyName != NAME_NONE)
		{
			if (_widgetGuidRedirectByName.find(widgetKeyName) == _widgetGuidRedirectByName.end())
			{
				widget = Allocate<T>();
				_widgetGuidRedirectByName.emplace(widgetKeyName, widget->GetGuid());
				_widgetNameRedirectByGuid.emplace(widget->GetGuid(), widgetKeyName);
			}
		}
		else
		{
			JGType widgetType = JGTYPE(T);
			if (_widgetGuidRedirectByType.find(widgetType) == _widgetGuidRedirectByType.end())
			{
				widget	   = Allocate<T>();
				_widgetGuidRedirectByType.emplace(widgetType, widget->GetGuid());
			}
		}

		HGuid widgetGuid = GetWidgetGuid<T>(widgetKeyName);
		if (widgetGuid.IsValid() == false)
		{
			return nullptr;
		}

		if (_widgetPool.find(widgetGuid) == _widgetPool.end())
		{
			_widgetPool.emplace(widgetGuid, widget);
		}

		baseWidget = _widgetPool[widgetGuid];

		if (baseWidget != nullptr)
		{
			if (widget->_bIsOpen == false)
			{
				_openWidgets[widgetGuid] = baseWidget;

				baseWidget->_bIsOpen = true;
				baseWidget->OnOpen();
			}
		}

		return widget;
	}

	template<class T>
	void CloseWidget(PName widgetKeyName = NAME_NONE)
	{
		PSharedPtr<WWidget> widget = FindWidget<T>(widgetKeyName);
		if (widget == nullptr)
		{
			return;
		}

		CloseWidget(widget->GetGuid());
	}

	void CloseWidget(const HGuid& widgetGuid)
	{
		PSharedPtr<WWidget> widget = FindWidget<WWidget>(widgetGuid);
		if (widget == nullptr)
		{
			return;
		}

		if (widget->_bIsOpen == false)
		{
			return;
		}

		_openWidgets.erase(widgetGuid);

		widget->_bIsOpen = false;
		widget->OnClose();
	}

	template<class T>
	PSharedPtr<T> FindWidget(PName widgetKeyName = NAME_NONE)
	{
		HGuid widgetGuid = GetWidgetGuid<T>(widgetKeyName);

		return FindWidget<T>(widgetGuid);
	}

	template<class T>
	PSharedPtr<T> FindWidget(const HGuid& inGuid)
	{
		if (inGuid.IsValid() == false)
		{
			return nullptr;
		}

		if (_widgetPool.find(inGuid) == _widgetPool.end())
		{
			return nullptr;
		}

		return RawFastCast<T>(_widgetPool[inGuid]);
	}

	template<class T>
	HGuid GetWidgetGuid(PName widgetKeyName = NAME_NONE)
	{
		HGuid widgetGuid;

		if (widgetKeyName != NAME_NONE)
		{
			if (_widgetGuidRedirectByName.find(widgetKeyName) != _widgetGuidRedirectByName.end())
			{
				widgetGuid = _widgetGuidRedirectByName[widgetKeyName];
			}
		}
		else
		{
			JGType widgetType = JGTYPE(T);
			if (_widgetGuidRedirectByType.find(widgetType) != _widgetGuidRedirectByType.end())
			{
				widgetGuid = _widgetGuidRedirectByType[widgetType];
			}
		}

		return widgetGuid;
	}

	template<class T>
	T& GetGUIStyle() const
	{
		static T NullGUIStyle;

		JGType GuiStyleType = JGTYPE(T);
		if (_guiStylePool.contains(GuiStyleType) == false)
		{
			return NullGUIStyle;
		}

		PSharedPtr<JGGUIStyle> GuiStyle = _guiStylePool.at(GuiStyleType);
		if (GuiStyle == nullptr)
		{
			return NullGUIStyle;
		}
		
		PSharedPtr<T> Result = Cast<T>(GuiStyle);
		if (Result == nullptr)
		{
			return NullGUIStyle;
		}

		return (*Result);
	}

private:
	void SaveGUIDatas();
	void LoadGUIDatas();
	void CollectGUIStyles();
};