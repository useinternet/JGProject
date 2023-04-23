#include "PCH/PCH.h"
#include "MenuTrees.h"

void HMenuTrees::UpdateTrees()
{
	if (bUpdateDirty)
	{
		sort(_sectionDatas.begin(), _sectionDatas.end(),
			[](const HSectionData& s1, const HSectionData& s2) -> bool
			{
				return s1.Priority < s2.Priority;
			});
	}

	for (int32 index : _rootNodes)
	{
		updateTrees(_menuNodes[index]);
	}

	// Action
	for (const HMainMenuData& _menuData : _mainMenuDatas)
	{
		_events.OnMainMenuAction.ExecuteIfBound(_menuData);
	}

	for (const HMenuData& _menuData : _menuDatas)
	{
		_events.OnMenuAction.ExecuteIfBound(_menuData);
	}

	bUpdateDirty = false;
}

void HMenuTrees::BindEvents(const HMenuTrees::HEvents& inEvents)
{
	_events = inEvents;
}

bool HMenuTrees::isExistMenuPath(const PString& inMenuPath) const
{
	HList<PString> pathTokens = inMenuPath.Split('/');
	if (pathTokens.empty())
	{
		// Error Log
		return false;
	}

	PName targetPathName;
	const HMenuNode* targetNode = nullptr;

	int32 numTokens = (int32)pathTokens.size();
	for (int32 i = 0; i < numTokens; ++i)
	{
		targetPathName = PName(pathTokens[i]);

		if (targetNode == nullptr)
		{
			if (_rootNodesByName.contains(targetPathName) == false)
			{
				return false;
			}

			int32 index = _rootNodesByName.at(targetPathName);
			targetNode = &_menuNodes[index];
		}
		else
		{
			int32 findIndex = INDEX_NONE;
			for (const HPair<PName, HList<int32>>& pair : targetNode->Childs)
			{
				for (int32 index : pair.second)
				{
					if (_menuNodes[index].NodeName == targetPathName)
					{
						findIndex = index;
						break;
					}
				}

				if (findIndex != INDEX_NONE)
					break;
			}

			if (findIndex != INDEX_NONE)
			{
				targetNode = &_menuNodes[findIndex];
			}
		}
	}
	return targetNode != nullptr;
}

void HMenuTrees::createMenuPathIfExist(const PName& inSectionName, const PString& inMenuPath, int32 inPriority, int32 inMenuDataIndex)
{
	HList<PString> pathTokens = inMenuPath.Split('/');
	if (pathTokens.empty())
	{
		// Error Log
		return;
	}

	PName targetPathName;
	HMenuNode* targetNode = nullptr;
	HHashMap<PName, HList<int32>> targetChildMap;

	int32 numTokens = (int32)pathTokens.size();
	for (int32 i = 0; i < numTokens; ++i)
	{
		targetPathName = PName(pathTokens[i]);

		if (i == 0)
		{
			if (_rootNodesByName.contains(targetPathName) == false)
			{
				HMenuNode newNode;
				newNode.NodeID = (int32)_menuNodes.size();
				newNode.NodeName = targetPathName;
				newNode.SectionName = inSectionName;

				_menuNodes.push_back(newNode);
				_rootNodes.push_back(newNode.NodeID);
				_rootNodesByName[targetPathName] = newNode.NodeID;
			}

			int32 nextIndex = _rootNodesByName[targetPathName];
			targetNode = &(_menuNodes[nextIndex]);
		}
		else if(targetNode != nullptr)
		{
			if (targetNode->Childs.contains(inSectionName) == false)
			{
				targetNode->Childs.emplace(inSectionName, HList<int32>());
			}

			int32 findIndex = INDEX_NONE;
			for (const HPair<PName, HList<int32>>& pair : targetNode->Childs)
			{
				for (int32 menuIndex : pair.second)
				{
					if (_menuNodes[menuIndex].NodeName == targetPathName)
					{
						findIndex = menuIndex;
						break;
					}
				}

				if (findIndex != INDEX_NONE)
					break;
			}

			if (findIndex == INDEX_NONE)
			{
				HMenuNode newNode;
				newNode.NodeID = (int32)_menuNodes.size();
				newNode.Parent = targetNode->NodeID;
				newNode.NodeName = targetPathName;
				newNode.SectionName = inSectionName;
				newNode.MenuIndex = inMenuDataIndex;
				newNode.Priority = inPriority;

				targetNode->Childs[inSectionName].push_back(newNode.NodeID);

				_menuNodes.push_back(newNode);
				findIndex = (int32)(_menuNodes.size() - 1);
			}

			targetNode = &(_menuNodes[findIndex]);
		}
	}
}

void HMenuTrees::updateTrees(HMenuNode& inMenuNode)
{
	if (inMenuNode.MenuIndex == INDEX_NONE)
	{
		if (bUpdateDirty)
		{
			for (HPair<const PName, HList<int32>>& pair : inMenuNode.Childs)
			{
				sort(pair.second.begin(), pair.second.end(),
					[&](int32 index1, int32 index2) -> bool
					{
						return _menuNodes[index1].Priority < _menuNodes[index2].Priority;
					});
			}
		}

		for (const HSectionData& sectionData : _sectionDatas)
		{
			bool bResult = _events.OnBeginMainMenu.ExecuteIfBound(inMenuNode.NodeName);
			if (bResult)
			{
				const HList<int32> childNodes = inMenuNode.Childs[sectionData.Name];
				for (int32 index : childNodes)
				{
					updateTrees(_menuNodes[index]);
				}

				_events.OnEndMainMenu.ExecuteIfBound();
			}
		}
	}
	else
	{
		_events.OnMainMenuItem.ExecuteIfBound(_mainMenuDatas[inMenuNode.MenuIndex]);
	}
}

void HContextMenuTrees::UpdateTrees()
{
	if (bUpdateDirty)
	{
		sort(_sectionDatas.begin(), _sectionDatas.end(),
			[](const HSectionData& s1, const HSectionData& s2) -> bool
			{
				return s1.Priority < s2.Priority;
			});
	}

	for (int32 index : _rootNodes)
	{
		updateTrees(_menuNodes[index]);
	}

	for (const HMenuData& _menuData : _menuDatas)
	{
		_events.OnContextMenuAction.ExecuteIfBound(_menuData);
	}

	bUpdateDirty = false;
}

void HContextMenuTrees::BindEvents(const HContextMenuTrees::HEvents& inEvents)
{
	_events = inEvents;
}

bool HContextMenuTrees::IsValid() const
{
	return _menuNodes.empty() == false;
}

bool HContextMenuTrees::isExistMenuPath(const PString& inMenuPath) const
{
	HList<PString> pathTokens = inMenuPath.Split('/');
	if (pathTokens.empty())
	{
		// Error Log
		return false;
	}

	PName targetPathName;
	const HMenuNode* targetNode = nullptr;

	int32 numTokens = (int32)pathTokens.size();
	for (int32 i = 0; i < numTokens; ++i)
	{
		targetPathName = PName(pathTokens[i]);

		if (targetNode == nullptr)
		{
			if (_rootNodesByName.contains(targetPathName) == false)
			{
				return false;
			}

			int32 index = _rootNodesByName.at(targetPathName);
			targetNode = &_menuNodes[index];
		}
		else
		{
			int32 findIndex = INDEX_NONE;
			for (const HPair<PName, HList<int32>>& pair : targetNode->Childs)
			{
				for (int32 index : pair.second)
				{
					if (_menuNodes[index].NodeName == targetPathName)
					{
						findIndex = index;
						break;
					}
				}

				if (findIndex != INDEX_NONE)
					break;
			}

			if (findIndex != INDEX_NONE)
			{
				targetNode = &_menuNodes[findIndex];
			}
		}
	}
	return targetNode != nullptr;
}

void HContextMenuTrees::createMenuPathIfExist(const PName& inSectionName, const PString& inMenuPath, int32 inPriority, int32 inMenuDataIndex)
{
	HList<PString> pathTokens = inMenuPath.Split('/');
	if (pathTokens.empty())
	{
		// Error Log
		return;
	}

	PName targetPathName;
	HMenuNode* targetNode = nullptr;
	HHashMap<PName, HList<int32>> targetChildMap;

	int32 numTokens = (int32)pathTokens.size();
	for (int32 i = 0; i < numTokens; ++i)
	{
		targetPathName = PName(pathTokens[i]);

		if (i == 0)
		{
			if (_rootNodesByName.contains(targetPathName) == false)
			{
				HMenuNode newNode;
				newNode.NodeID = (int32)_menuNodes.size();
				newNode.NodeName = targetPathName;
				newNode.SectionName = inSectionName;

				_menuNodes.push_back(newNode);
				_rootNodes.push_back(newNode.NodeID);
				_rootNodesByName[targetPathName] = newNode.NodeID;
			}

			int32 nextIndex = _rootNodesByName[targetPathName];
			targetNode = &(_menuNodes[nextIndex]);
		}
		else if(targetNode != nullptr)
		{
			if (targetNode->Childs.contains(inSectionName) == false)
			{
				targetNode->Childs.emplace(inSectionName, HList<int32>());
			}

			int32 findIndex = INDEX_NONE;
			for (const HPair<PName, HList<int32>>& pair : targetNode->Childs)
			{
				for (int32 menuIndex : pair.second)
				{
					if (_menuNodes[menuIndex].NodeName == targetPathName)
					{
						findIndex = menuIndex;
						break;
					}
				}

				if (findIndex != INDEX_NONE)
					break;
			}

			if (findIndex == INDEX_NONE)
			{
				HMenuNode newNode;
				newNode.NodeID = (int32)_menuNodes.size();
				newNode.Parent = targetNode->NodeID;
				newNode.NodeName = targetPathName;
				newNode.SectionName = inSectionName;
				newNode.MenuIndex = inMenuDataIndex;
				newNode.Priority = inPriority;

				targetNode->Childs[inSectionName].push_back(newNode.NodeID);

				_menuNodes.push_back(newNode);
				findIndex = (int32)(_menuNodes.size() - 1);
			}

			targetNode = &(_menuNodes[findIndex]);
		}
	}
}

void HContextMenuTrees::updateTrees(HMenuNode& inMenuNode)
{
	if (inMenuNode.MenuIndex == INDEX_NONE)
	{
		if (bUpdateDirty)
		{
			for (HPair<const PName, HList<int32>>& pair : inMenuNode.Childs)
			{
				sort(pair.second.begin(), pair.second.end(),
					[&](int32 index1, int32 index2) -> bool
					{
						return _menuNodes[index1].Priority < _menuNodes[index2].Priority;
					});
			}
		}

		for (const HSectionData& sectionData : _sectionDatas)
		{
			bool bResult = _events.OnBeginContextMenu.ExecuteIfBound(inMenuNode.NodeName);
			if (bResult)
			{
				const HList<int32> childNodes = inMenuNode.Childs[sectionData.Name];
				for (int32 index : childNodes)
				{
					updateTrees(_menuNodes[index]);
				}

				_events.OnEndContextMenu.ExecuteIfBound();
			}
		}
	}
	else
	{
		_events.OnContextMenuItem.ExecuteIfBound(_menuDatas[inMenuNode.MenuIndex]);
	}
}