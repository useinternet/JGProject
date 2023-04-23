#pragma once
#include "Core.h"
#include "GUIDefine.h"
#include "MenuCommand.h"

class WWidget;
class HMenuTrees
{
	friend class HMenuBuilder;

public:
	struct HMainMenuData;
	struct HMenuData;

	JG_DECLARE_DELEGATE_ONEPARAM_RETVAL(bool, HOnBeginMainMenu, const PName&);
	JG_DECLARE_DELEGATE(HOnEndMainMenu);
	JG_DECLARE_DELEGATE_ONEPARAM(HOnMainMenuItem, const HMainMenuData&);
	JG_DECLARE_DELEGATE_ONEPARAM(HOnMainMenuAction, const HMainMenuData&);
	JG_DECLARE_DELEGATE_ONEPARAM(HOnMenuAction, const HMenuData&);

	struct HEvents
	{
		HOnBeginMainMenu  OnBeginMainMenu;
		HOnEndMainMenu    OnEndMainMenu;
		HOnMainMenuItem	  OnMainMenuItem;
		HOnMainMenuAction OnMainMenuAction;
		HOnMenuAction	  OnMenuAction;
	};

	struct HSectionData
	{
		PName Name;
		int32 Priority;
	};

	struct HMainMenuData
	{
		PName		 SectionName;
		HMenuCommand MenuCommand;
		HMenuAction  Action;
		HIsVisibleMenu IsVisible;
		HIsEnableMenu  IsEnable;
		int32 Priority;
	};

	struct HMenuData
	{
		PName			  SectionName;
		PWeakPtr<WWidget> Widget;
		HMenuCommand	  MenuCommand;
		HMenuAction		  Action;
	};

private:
	struct HMenuNode
	{
		PName NodeName;

		int32 NodeID = INDEX_NONE;
		int32 Parent = INDEX_NONE;

		HHashMap<PName, HList<int32>> Childs;

		PName SectionName;
		int32 MenuIndex = INDEX_NONE;
		int32 Priority = 0;
	};


	// RootNode
	HHashMap<PName, int32> _rootNodesByName;
	HList<int32> _rootNodes;


	// RootNodew
	HList<HMenuNode> _menuNodes;


	HList<HSectionData>  _sectionDatas;
	HList<HMainMenuData> _mainMenuDatas;
	HList<HMenuData>	 _menuDatas;
	HEvents _events;

	bool bUpdateDirty = true;
public:
	void UpdateTrees();
	void BindEvents(const HEvents& inEvents);
private:
	bool isExistMenuPath(const PString& inMenuPath) const;
	void createMenuPathIfExist(const PName& inSectionName, const PString& inMenuPath, int32 inPriority, int32 menuDataIndex);
	void updateTrees(HMenuNode& inMenuNode);
};


class HContextMenuTrees
{
	friend class HContextMenuBuilder;

public:
	struct HMenuData;

	JG_DECLARE_DELEGATE_ONEPARAM_RETVAL(bool, HOnBeginContextMenu, const PName&);
	JG_DECLARE_DELEGATE(HOnEndContextMenu);
	JG_DECLARE_DELEGATE_ONEPARAM(HOnContextMenuItem, const HMenuData&);
	JG_DECLARE_DELEGATE_ONEPARAM(HOnContextMenuAction, const HMenuData&);

	struct HEvents
	{
		HOnBeginContextMenu   OnBeginContextMenu;
		HOnEndContextMenu     OnEndContextMenu;
		HOnContextMenuItem	  OnContextMenuItem;
		HOnContextMenuAction  OnContextMenuAction;
	};

	struct HSectionData
	{
		PName Name;
		int32 Priority;
	};

	struct HMenuData
	{
		PName		 SectionName;
		HMenuCommand MenuCommand;
		HGuid        OwnerGuid;
		HMenuAction    Action;
		HIsVisibleMenu IsVisible;
		HIsEnableMenu  IsEnable;
		int32 Priority;
	};

private:
	struct HMenuNode
	{
		PName NodeName;

		int32 NodeID = INDEX_NONE;
		int32 Parent = INDEX_NONE;

		HHashMap<PName, HList<int32>> Childs;

		PName SectionName;
		int32 MenuIndex = INDEX_NONE;
		int32 Priority = 0;
	};


	// RootNode
	HHashMap<PName, int32> _rootNodesByName;
	HList<int32> _rootNodes;


	// RootNodew
	HList<HMenuNode> _menuNodes;


	HList<HSectionData>  _sectionDatas;
	HList<HMenuData>	 _menuDatas;
	HEvents _events;

	bool bUpdateDirty = true;
public:
	void UpdateTrees();
	void BindEvents(const HEvents& inEvents);

	bool IsValid() const;
private:
	bool isExistMenuPath(const PString& inMenuPath) const;
	void createMenuPathIfExist(const PName& inSectionName, const PString& inMenuPath, int32 inPriority, int32 menuDataIndex);
	void updateTrees(HMenuNode& inMenuNode);
};