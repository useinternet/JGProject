#pragma once
#include "Core.h"
#include "Widget.h"

class GGUIGlobalSystem : public GGlobalSystemInstance<GGUIGlobalSystem>
{
	PSharedPtr<IMemoryObject> _memObject;

	HHashMap<HGuid, PSharedPtr<WWidget>> _widgetPool;
	HHashMap<PName,  HGuid> _widgetGuidRedirectByName;
	HHashMap<JGType, HGuid> _widgetGuidRedirectByType;

	HHashMap<HGuid, PSharedPtr<WWidget>> _openWidgets;
protected:
	virtual void Start() override;
	virtual void Destroy() override;

	void BuildGUI(const PTaskArguments& args);
public:
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
};
