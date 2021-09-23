#include "pch.h"
#include "WorldHierarchyViewModel.h"
#include "Application.h"
#include "Class/Game/GameNode.h"
#include "Class/Game/GameWorld.h"
#include "Class/UI/UIView/InspectorView.h"
#include "Class/Game/Components/SpriteRenderer.h"
#include "Class/Game/Components/StaticMeshRenderer.h"
#include "Class/Game/Components/PointLight.h"

namespace JG
{




	void WorldHierarchyViewModel::Initialize()
	{
		//RequestGetGameWorldEvent e;
		//SendEventImmediate(e);
		//mGameWorld = e.GameWorld;

		CreateEmptyObject = CreateUniquePtr<Command<>>();
		CreateEmptyObject->Subscribe(this, [&]()
		{
			auto parent = GetSelectdNodeInContextMenu();
			if (parent == nullptr)
			{
				return;
			}
			parent->AddNode("Empty");
		});


		CreateSprite = CreateUniquePtr<Command<>>();
		CreateSprite->Subscribe(this, [&]()
		{
			auto parent = GetSelectdNodeInContextMenu();
			if (parent == nullptr)
			{
				return;
			}
			auto node = parent->AddNode("Sprite");
			node->AddComponent<SpriteRenderer>();
		});

		CreateCube = CreateUniquePtr<Command<>>();
		CreateCube->Subscribe(this, [&]()
		{
			auto parent = GetSelectdNodeInContextMenu();
			if (parent == nullptr)
			{
				return;
			}
			auto node = parent->AddNode("Cube");
			auto renderer = node->AddComponent<StaticMeshRenderer>();
			renderer->SetMesh("Asset/Engine/Mesh/Cube.jgasset");
		});


		CreateSphere = CreateUniquePtr<Command<>>();
		CreateSphere->Subscribe(this, [&]()
		{
			auto parent = GetSelectdNodeInContextMenu();
			if (parent == nullptr)
			{
				return;
			}
			auto node = parent->AddNode("Sphere");
			auto renderer = node->AddComponent<StaticMeshRenderer>();
			renderer->SetMesh("Asset/Engine/Mesh/Sphere.jgasset");
		});


		CreatePointLight = CreateUniquePtr<Command<>>();
		CreatePointLight->Subscribe(this, [&]()
		{
			auto parent = GetSelectdNodeInContextMenu();
			if (parent == nullptr)
			{
				return;
			}
			auto node = parent->AddNode("PointLight");
			node->AddComponent<PointLight>();
		});



		CopyGameNodes = CreateUniquePtr<Command<>>();
		CopyGameNodes->Subscribe(this, [&]() {
			if (mCurrentSelectedNodeList.empty())
			{
				return;
			}

			mCopyJson.reset(); mCopyJson = nullptr;
			mCopyJson = CreateSharedPtr<Json>();
			auto nodeListJson = mCopyJson->CreateJsonData();

			for (auto node : mCurrentSelectedNodeList)
			{
				auto jsonData = nodeListJson->CreateJsonData();
				node->MakeJson(jsonData);
				nodeListJson->AddMember(jsonData);
			}
			mCopyJson->AddMember("NodeList", nodeListJson);
			UIManager::GetInstance().SetClipBoard((u64)this, mCopyJson.get(), sizeof(ptraddr));
		});


		PasteGameNodes = CreateUniquePtr<Command<>>();
		PasteGameNodes->Subscribe(this, [&]() {
			auto parent = GetSelectdNodeInContextMenu();
			if (parent == nullptr)
			{
				return;
			}
			if (mCopyJson == nullptr) {
				return;
			}
			auto nodeListJson = mCopyJson->GetMember("NodeList");
			for (i32 i = 0; i < nodeListJson->GetSize(); ++i)
			{
				auto jsonData = nodeListJson->GetJsonDataFromIndex(i);

				auto node = parent->AddNode(std::to_string(i));
				node->LoadJson(jsonData);
			}
			mCopyJson = nullptr;
		
		});

		DeleteGameNode = CreateUniquePtr<Command<>>();
		DeleteGameNode->Subscribe(this, [&]() {
			if (mCurrentSelectedNodeList.empty())
			{
				return;
			}
			Scheduler::GetInstance().ScheduleOnceByFrame(0, SchedulePriority::BeginSystem, [&]() -> EScheduleResult
			{
				for (auto& node : mCurrentSelectedNodeList)
				{
					node->Destroy(node);
					mTreeNodePool.erase(node);
				}
				ClearSelectedNode();
				return EScheduleResult::Continue;
			});
		

		
		});
	}

	void WorldHierarchyViewModel::Destroy()
	{
		UIViewModel::Destroy();

		CreateEmptyObject->UnSubscribe(this);
		CreateSprite->UnSubscribe(this);
		CreateCube->UnSubscribe(this);
		CreateSphere->UnSubscribe(this);
		CreatePointLight->UnSubscribe(this);
		CopyGameNodes->UnSubscribe(this);
		PasteGameNodes->UnSubscribe(this);
		DeleteGameNode->UnSubscribe(this);

		mTreeNodePool.clear();
		CreateEmptyObject = nullptr;
		DeleteGameNode = nullptr;
		mGameWorld     = nullptr;
		mCurrentSelectedNodeInInspector   = nullptr;
		mCurrentSelectedNodeInContextMenu = nullptr;
	}

	void WorldHierarchyViewModel::OnEvent(IEvent& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<NotifyDestroyJGObjectEvent>(EVENT_BIND_FN(&WorldHierarchyViewModel::ResponseDestroyGameObject));
		dispatcher.Dispatch<NotifyChangeGameWorldEvent>(EVENT_BIND_FN(&WorldHierarchyViewModel::ResponseChangeGameWorld));
		dispatcher.Dispatch<NotifySelectedGameNodeInEditorEvent>(EVENT_BIND_FN(&WorldHierarchyViewModel::ResponseSelectedGameNodeInEditor));
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

		if (mCurrentSelectedNodeInInspector != nullptr)
		{
			auto& treeNode = mTreeNodePool[mCurrentSelectedNodeInInspector];
		}
		if (mCurrentSelectedNodeInContextMenu != nullptr)
		{
			auto& treeNode = mTreeNodePool[mCurrentSelectedNodeInContextMenu];
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

		if (mCurrentSelectedNodeInContextMenu != nullptr)
		{
			auto& treeNode = mTreeNodePool[mCurrentSelectedNodeInContextMenu];
		}

		if (mCurrentSelectedNodeInInspector != nullptr)
		{
			auto& treeNode = mTreeNodePool[mCurrentSelectedNodeInInspector];
			mCurrentSelectedNodeInInspector = nullptr;
		}
		mCurrentSelectedNodeInContextMenu = node;
	}

	GameNode* WorldHierarchyViewModel::GetSelectdNodeInContextMenu() const
	{
		return mCurrentSelectedNodeInContextMenu;
	}

	void WorldHierarchyViewModel::AddSelectedNode(GameNode* node)
	{
		if (mTreeNodePool.find(node) == mTreeNodePool.end())
		{
			return;
		}
		auto& treeNode = mTreeNodePool[node];
		treeNode.IsSelected = true;



		mCurrentSelectedNodeList.emplace(node);
	}

	void WorldHierarchyViewModel::ClearSelectedNode()
	{
		for (auto& selectedNode : mCurrentSelectedNodeList)
		{
			if (mTreeNodePool.find(selectedNode) == mTreeNodePool.end()) continue;
			auto& treeNode = mTreeNodePool[selectedNode];
			treeNode.IsSelected = false;
		}


		mCurrentSelectedNodeList.clear();
	}

	const HashSet<GameNode*>& WorldHierarchyViewModel::GetSelectedNodeList() const
	{
		return mCurrentSelectedNodeList;
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



	bool WorldHierarchyViewModel::ResponseDestroyGameObject(NotifyDestroyJGObjectEvent& e)
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

	bool WorldHierarchyViewModel::ResponseChangeGameWorld(NotifyChangeGameWorldEvent& e)
	{
		mGameWorld = e.GameWorld;
		return false;
	}
	bool WorldHierarchyViewModel::ResponseSelectedGameNodeInEditor(NotifySelectedGameNodeInEditorEvent& e)
	{
		if (mCurrentSelectedNodeInInspector != e.SelectedGameNode)
		{
			SetSelectedNodeInInspector(e.SelectedGameNode);
		}

		return false;
	}
}