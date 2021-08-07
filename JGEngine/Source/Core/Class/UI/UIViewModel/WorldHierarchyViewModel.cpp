#include "pch.h"
#include "WorldHierarchyViewModel.h"
#include "Application.h"
#include "Class/Game/GameNode.h"
#include "Class/Game/GameWorld.h"
#include "Class/UI/UIView/InspectorView.h"


namespace JG
{




	void WorldHierarchyViewModel::Initialize()
	{
		RequestGetGameWorldEvent e;
		SendEventImmediate(e);
		mGameWorld = e.GameWorld;

		AddEmptyObject = CreateUniquePtr<Command<GameNode*>>();
		AddEmptyObject->Subscribe(this, [&](GameNode* parent)
		{
			if (parent == nullptr)
			{
				return;
			}
			parent->AddNode("Empty");
		});

		DeleteGameNode = CreateUniquePtr<Command<GameNode*>>();
		DeleteGameNode->Subscribe(this, [&](GameNode* node)
		{
			if (node == nullptr)
			{
				return;
			}
			node->Destroy(node);
			Scheduler::GetInstance().ScheduleOnce(0, 0, [&]()
			{
				mTreeNodePool.erase(node);
				return EScheduleResult::Break;
			});
		});
	}

	void WorldHierarchyViewModel::Destroy()
	{
		UIViewModel::Destroy();

		AddEmptyObject->UnSubscribe(this);
		DeleteGameNode->UnSubscribe(this);
		mTreeNodePool.clear();
		AddEmptyObject = nullptr;
		DeleteGameNode = nullptr;
		mGameWorld     = nullptr;
		mCurrentSelectedNodeInInspector   = nullptr;
		mCurrentSelectedNodeInContextMenu = nullptr;
	}

	void WorldHierarchyViewModel::OnEvent(IEvent& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<NotifyDestroyJGObjectEvent>(EVENT_BIND_FN(&WorldHierarchyViewModel::NotifyDestroyGameObject));
		dispatcher.Dispatch<NotifyChangeGameWorldEvent>(EVENT_BIND_FN(&WorldHierarchyViewModel::NotifyChangeGameWorld));
	}

	void WorldHierarchyViewModel::ForEach(
		const std::function<bool(WorldHierarchyTreeNode&)>& pushAction,
		const std::function<void(WorldHierarchyTreeNode&)>& action,
		const std::function<void(WorldHierarchyTreeNode&)>& popAction)
	{
		auto gameWorld = mGameWorld;
		if (gameWorld != nullptr)
		{
			ForEach(gameWorld, pushAction, action, popAction);
		}

	}
	void WorldHierarchyViewModel::SetSelectedNodeInInspector(GameNode* node)
	{
		if (mTreeNodePool.find(node) == mTreeNodePool.end() || node == mCurrentSelectedNodeInInspector)
		{
			return;
		}
		auto& treeNode = mTreeNodePool[node];
		treeNode.IsSelected = true;

		if (mCurrentSelectedNodeInInspector != nullptr)
		{
			auto& treeNode = mTreeNodePool[mCurrentSelectedNodeInInspector];
			treeNode.IsSelected = false;
		}
		if (mCurrentSelectedNodeInContextMenu != nullptr)
		{
			auto& treeNode = mTreeNodePool[mCurrentSelectedNodeInContextMenu];
			treeNode.IsSelected = false;
			mCurrentSelectedNodeInContextMenu = nullptr;
		}

		mCurrentSelectedNodeInInspector = node;

		NotifySelectedGameNodeInEditorEvent e;
		e.SelectedGameNode = node;
		SendEvent(e);

	}

	GameNode* WorldHierarchyViewModel::GetSelectedNodeInInspector() const
	{
		return mCurrentSelectedNodeInInspector;
	}

	void WorldHierarchyViewModel::SetSelectedNodeInContextMenu(GameNode* node)
	{
		if (mTreeNodePool.find(node) == mTreeNodePool.end() || node == mCurrentSelectedNodeInContextMenu)
		{
			return;
		}
		auto& treeNode = mTreeNodePool[node];
		treeNode.IsSelected = true;

		if (mCurrentSelectedNodeInContextMenu != nullptr)
		{
			auto& treeNode = mTreeNodePool[mCurrentSelectedNodeInContextMenu];
			treeNode.IsSelected = false;
		}

		if (mCurrentSelectedNodeInInspector != nullptr)
		{
			auto& treeNode = mTreeNodePool[mCurrentSelectedNodeInInspector];
			treeNode.IsSelected = false;
			mCurrentSelectedNodeInInspector = nullptr;
		}
		mCurrentSelectedNodeInContextMenu = node;
	}

	GameNode* WorldHierarchyViewModel::GetSelectdNodeInContextMenu() const
	{
		return mCurrentSelectedNodeInContextMenu;
	}


	void WorldHierarchyViewModel::ForEach(GameNode* gameNode,
		const std::function<bool(WorldHierarchyTreeNode&)>& pushAction,
		const std::function<void(WorldHierarchyTreeNode&)>& action,
		const std::function<void(WorldHierarchyTreeNode&)>& popAction)
	{
		if (gameNode == nullptr)
		{
			return;
		}
		auto iter = mTreeNodePool.find(gameNode);
		if (iter == mTreeNodePool.end())
		{
			mTreeNodePool[gameNode].Object = gameNode;
			mTreeNodePool[gameNode].UserFlags = 0;
		}
		auto& treeNode = mTreeNodePool[gameNode];

		bool isOpen = pushAction(treeNode);
		action(treeNode);

		if (treeNode.IsSelected)
		{
			mSelectedNodes.insert(treeNode.Object);
		}
		else
		{
			mSelectedNodes.erase(treeNode.Object);
		}


		if (isOpen == true)
		{
			gameNode->ForEach([&](GameNode* node)
			{
				ForEach(node, pushAction, action, popAction);
			});
		
		}
	
		if (treeNode.IsTreePop)
		{
			popAction(treeNode);
		}
	}

	bool WorldHierarchyViewModel::NotifyDestroyGameObject(NotifyDestroyJGObjectEvent& e)
	{
		auto currentSelectedNodeInContextMenu = GetSelectdNodeInContextMenu();
		if (currentSelectedNodeInContextMenu != nullptr)
		{
			if (currentSelectedNodeInContextMenu == e.DestroyedObject)
			{
				SetSelectedNodeInContextMenu(nullptr);
			}
		}

		auto currentSelectedNodeInInspector = GetSelectedNodeInInspector();
		if (currentSelectedNodeInInspector != nullptr)
		{
			if (currentSelectedNodeInInspector == e.DestroyedObject)
			{
				SetSelectedNodeInContextMenu(nullptr);
			}
		}
		return false;
	}

	bool WorldHierarchyViewModel::NotifyChangeGameWorld(NotifyChangeGameWorldEvent& e)
	{
		mGameWorld = e.GameWorld;
		return false;
	}

}