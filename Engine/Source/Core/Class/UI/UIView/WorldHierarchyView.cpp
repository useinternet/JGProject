#include "pch.h"
#include "WorldHierarchyView.h"
#include "Imgui/imgui.h"
#include "Class/Game/GameNode.h"
#include "Class/Game/GameWorld.h"
#include "Class/UI/UIView/InspectorView.h"
#include "Class/Game/Components/SpriteRenderer.h"
#include "Class/Game/Components/StaticMeshRenderer.h"
#include "Class/Game/Components/PointLight.h"


namespace JG
{
	WorldHierarchyView::WorldHierarchyView()
	{

		UIManager::GetInstance().RegisterMainMenuItem("Windows/WorldHierarchy", 0, [&]()
		{
			Open();
		}, nullptr);
	}
	void WorldHierarchyView::Load()
	{


		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Create/EmptyObject", 0, [&]()
		{
			CreateEmptyObject();
		}, nullptr);
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Create/2D/Sprite", 0, [&]()
		{
			CreateSprite();
		}, nullptr);
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Create/3D/Cube", 0, [&]()
		{
			CreateCube();
		}, nullptr);
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Create/3D/Sphere", 0, [&]()
		{
			CreateSphere();
		}, nullptr);
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Create/Light/PointLight", 0, [&]()
		{
			CreatePointLight();
		}, nullptr);

		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Copy", 0,
			[&]()
		{
			//mVm->CopyGameNodes->Execute();
		},
			[&]() -> bool
		{
			//if (mVm->GetSelectdNodeInContextMenu() != nullptr)
			//{
			//	return mVm->GetSelectdNodeInContextMenu()->GetParent() != nullptr;
			//}
			//return false;
			return false;
		});


		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Paste", 0,
			[&]()
		{
			//mVm->PasteGameNodes->Execute();
		}, nullptr);

		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Delete", 0, 
			[&]()
		{
			//mVm->DeleteGameNode->Execute();
		}, 
			[&]() -> bool
		{
			//if (mVm->GetSelectdNodeInContextMenu() != nullptr)
			//{
			//	return mVm->GetSelectdNodeInContextMenu()->GetParent() != nullptr;
			//}
			//return false;
			return false;
		});
	}
	void WorldHierarchyView::Initialize()
	{
		mIsCtrl = false;
		mGameWorld = nullptr;


		mTargetGameNode = nullptr;
		mSelectedGameNode.clear();
		mTreeNodeStatePool.clear();
		//mVm = GetViewModel();
	}
	void WorldHierarchyView::OnGUI()
	{
		mIsCtrl = ImGui::IsKeyDown((i32)EKeyCode::Ctrl);
		ImGui::Begin("WorldHierarchy", &mOpenGUI);
		UpdateNode();
		if (ImGui::BeginTable("WorldHierarchy_Table", 3, ImGuiTableFlags_Borders) == true)
		{
			ImGui::TableSetupColumn("##Active", ImGuiTableColumnFlags_WidthFixed, 28.0f);
			ImGui::TableSetupColumn("Object");
			ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 100.0F);
			ImGui::TableHeadersRow();
			GameNode_OnGUI(mGameWorld);
			if (ImGui::IsAnyItemHovered() == false)
			{
				if (ImGui::IsMouseClicked(0))
				{
					mSelectedGameNode.clear();
					mTargetGameNode = nullptr;
				}
			}
			ImGui::EndTable();
		}
		ImGui::End();



		//bool isCtrl = ImGui::IsKeyDown((int)EKeyCode::Ctrl);
	

		//mVm->ForEach(
		//	[&](WorldHierarchyTreeNode& nodeData) -> bool
		//{
		//	bool isOpen = false;
		//	bool isRoot = nodeData.Object->GetParent() == nullptr;
		//	bool isLeaf = nodeData.Object->GetChildCount() == 0;
		//	nodeData.UserFlags |= ImGuiTreeNodeFlags_OpenOnArrow;

		//	(isLeaf) ?
		//		nodeData.UserFlags |= ImGuiTreeNodeFlags_Leaf :
		//		nodeData.UserFlags &= ~(ImGuiTreeNodeFlags_Leaf);

		//	if (isRoot == true)
		//	{
		//		isOpen = ImGui::CollapsingHeader((nodeData.Object->GetName() + "##GameWorld").c_str());
		//		nodeData.IsTreePop = false;
		//	}
		//	else
		//	{
		//		
		//		isOpen = ImGui::TreeNodeEx((void*)nodeData.Object, nodeData.UserFlags, nodeData.Object->GetName().c_str()); 
		//		nodeData.IsTreePop = isOpen;
		//	}
		//	return isOpen;
		//},
		//	[&](WorldHierarchyTreeNode& nodeData)
		//{
		//	static bool isContextOpen         = false;

		//	ImGui::PushID(nodeData.Object);


		//	auto selectedNodeList = mVm->GetSelectedNodeList();
		//	bool isSelectedNode   = selectedNodeList.find(nodeData.Object) != selectedNodeList.end();


		//	if (UIManager::GetInstance().ShowContextMenu(GetType()) == true)
		//	{
		//		if (isContextOpen == false)
		//		{
		//			isContextOpen = true;
		//			mVm->AddSelectedNode(nodeData.Object);
		//			mVm->SetSelectedNodeInContextMenu(nodeData.Object);
		//		}
		//	}
		//	else
		//	{
		//		isContextOpen = false;
		//		if (ImGui::IsItemClicked() == true)
		//		{
		//			mVm->AddSelectedNode(nodeData.Object);
		//			mVm->SetSelectedNodeInInspector(nodeData.Object);
		//		}
		//	}

		//	// ¿ø·¡ isSelectedNode == false 
		//	if (isCtrl == false && nodeData.IsSelected && ImGui::IsMouseReleased(0) && ImGui::IsItemHovered())
		//	{
		//		mVm->ClearSelectedNode();
		//		mVm->AddSelectedNode(nodeData.Object);
		//	}
		//	else if (isCtrl == false && nodeData.IsSelected && isSelectedNode == false)
		//	{
		//		mVm->ClearSelectedNode();
		//		mVm->AddSelectedNode(nodeData.Object);
		//	}




		//	(nodeData.IsSelected == true) ?
		//		nodeData.UserFlags |= ImGuiTreeNodeFlags_Selected :
		//		nodeData.UserFlags &= ~ImGuiTreeNodeFlags_Selected;


		//	ImGui::PopID();
		//	UpdateDragAndDrop(nodeData.Object);
		//},
		//	[&](WorldHierarchyTreeNode& nodeData)
		//{
		//	ImGui::TreePop();
		//});
		//ImGui::End();



		//auto selectedNodeList = mVm->GetSelectedNodeList();
		//for (auto& node : selectedNodeList)
		//{
		//	auto parentNode = node->GetParent();
		//	if (parentNode == nullptr) continue;

		//	i64 nodeIndex = parentNode->GetNodeIndex(node);
		//	if (ImGui::IsKeyPressed((int)EKeyCode::NumPadAdd))
		//	{
		//		parentNode->Swap(nodeIndex, nodeIndex + 1);
		//	}
		//	if (ImGui::IsKeyPressed((int)EKeyCode::NumPadSubtract))
		//	{
		//		parentNode->Swap(nodeIndex, nodeIndex - 1);
		//	}
		//}

		if (mOpenGUI == false)
		{
			mOpenGUI = true;
			Close();
		}
	}

	void WorldHierarchyView::Destroy()
	{

	}
	void WorldHierarchyView::OnEvent(IEvent& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<NotifyDestroyJGObjectEvent>(EVENT_BIND_FN(&WorldHierarchyView::ResponseDestroyGameObject));
		dispatcher.Dispatch<NotifyChangeGameWorldEvent>(EVENT_BIND_FN(&WorldHierarchyView::ResponseChangeGameWorld));
		dispatcher.Dispatch<NotifySelectedGameNodeInEditorEvent>(EVENT_BIND_FN(&WorldHierarchyView::ResponseSelectedGameNodeInEditor));
	}
	void WorldHierarchyView::GameNode_OnGUI(GameNode* gameNode)
	{
		if (gameNode == nullptr)
		{
			return;
		}
		auto treeNodeState = GetTreeNodeState(gameNode);
		if (treeNodeState == nullptr)
		{
			mTreeNodeStatePool.emplace(gameNode, CreateSharedPtr<NodeState>());
			treeNodeState = GetTreeNodeState(gameNode);
		}

		i32 treeFlags = ImGuiTreeNodeFlags_OpenOnArrow;
		if (gameNode->GetParent() == nullptr)
		{
			treeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
		}
		if (gameNode->GetChildCount() == 0)
		{
			treeFlags |= ImGuiTreeNodeFlags_Leaf;
		}
		if (treeNodeState->IsState(NodeState_Selected) == true)
		{
			treeFlags |= ImGuiTreeNodeFlags_Selected;
		}
	


		ImGui::TableNextColumn();
		ImGui::SetCursorPosX(ImGui::GetStyle().ItemSpacing.x * 2);
		bool isActive = gameNode->IsActive();
		if (isActive == false)
		{
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(100, 100, 100, 100), 0);
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(100, 100, 100, 100), 1);
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(100, 100, 100, 100), 2);
		}
		if (ImGui::Checkbox(("##Active_CheckBox" + gameNode->GetName()).c_str(), &isActive) && gameNode->GetParent() != nullptr)
		{
			gameNode->SetActive(isActive);
		}

		ImGui::TableNextColumn();
		bool isOpen = ImGui::TreeNodeEx((const void*)gameNode, treeFlags, gameNode->GetName().c_str());
		bool isItemClick = ImGui::IsItemClicked();
		bool isClearSelectedFiles = mIsCtrl == false;
		if (UIManager::GetInstance().ShowContextMenu(GetType()) == true)
		{
			if (mIsCtrl == false && treeNodeState->IsState(NodeState_Selected) == false)
			{
				isClearSelectedFiles = true;
			}
			else
			{
				isClearSelectedFiles = false;
			}
			isItemClick = true;
		}
		if (isItemClick) {
			if (isClearSelectedFiles) {
				mSelectedGameNode.clear();
			}
			mSelectedGameNode.insert(gameNode);
			mTargetGameNode = gameNode;
			NotifySelectedGameNodeInEditorEvent e;
			e.SelectedGameNode = gameNode;
			SendEvent(e);
		}




		ImGui::TableNextColumn();
		ImGui::Text(ReplaceAll(gameNode->GetType().GetName(), "JG::", "").c_str());




		if (isOpen == true)
		{
			u64 childCnt = gameNode->GetChildCount();
			for (auto i = 0; i < childCnt; ++i)
			{
				GameNode_OnGUI(gameNode->FindNode(i));
			}
			ImGui::TreePop();
		}

		treeNodeState->Off(NodeState_Selected);
	}

	void WorldHierarchyView::UpdateNode()
	{
		for (auto& node : mSelectedGameNode)
		{
			auto nodeState = GetTreeNodeState(node);
			if (nodeState == nullptr) continue;
			nodeState->On(NodeState_Selected);
		}
	}

	void WorldHierarchyView::CreateEmptyObject()
	{
		if (mTargetGameNode == nullptr) return;
		mTargetGameNode->AddNode("EmptyObject");
	}
	void WorldHierarchyView::CreateSprite()
	{
		if (mTargetGameNode == nullptr) return;
		auto node = mTargetGameNode->AddNode("Sprite");
		node->AddComponent<SpriteRenderer>();
	}
	void WorldHierarchyView::CreateCube()
	{
		if (mTargetGameNode == nullptr) return;
		auto node = mTargetGameNode->AddNode("Cube");
		auto renderer = node->AddComponent<StaticMeshRenderer>();
		renderer->SetMesh("Asset/Engine/Mesh/Cube.jgasset");
	}
	void WorldHierarchyView::CreateSphere()
	{
		if (mTargetGameNode == nullptr) return;
		auto node = mTargetGameNode->AddNode("Sphere");
		auto renderer = node->AddComponent<StaticMeshRenderer>();
		renderer->SetMesh("Asset/Engine/Mesh/Sphere.jgasset");
	}
	void WorldHierarchyView::CreatePointLight()
	{
		if (mTargetGameNode == nullptr) return;
		auto node = mTargetGameNode->AddNode("PointLight");
		node->AddComponent<PointLight>();
	}
	WorldHierarchyView::NodeState* WorldHierarchyView::GetTreeNodeState(GameNode* gameNode)
	{
		if (mTreeNodeStatePool.find(gameNode) == mTreeNodeStatePool.end())
		{
			return nullptr;
		}
		else
		{
			return mTreeNodeStatePool[gameNode].get();
		}
	}
	bool WorldHierarchyView::ResponseDestroyGameObject(NotifyDestroyJGObjectEvent& e)
	{
		if (mTargetGameNode == e.DestroyedObject)
		{
			mTargetGameNode = nullptr;
		}
		bool isDestroy = false;
		for (auto& node : mSelectedGameNode)
		{
			if (node == e.DestroyedObject)
			{
				isDestroy = true;
				break;
			}
		}
		if (isDestroy)
		{
			mSelectedGameNode.erase((GameNode*)e.DestroyedObject);
		}
		return false;
	}

	bool WorldHierarchyView::ResponseChangeGameWorld(NotifyChangeGameWorldEvent& e)
	{
		if (e.GameWorld != nullptr)
		{
			mGameWorld = e.GameWorld;
		}
		return false;
	}
	bool WorldHierarchyView::ResponseSelectedGameNodeInEditor(NotifySelectedGameNodeInEditorEvent& e)
	{

		return false;
	}
	//void WorldHierarchyView::UpdateDragAndDrop(GameNode* targetNode)
	//{
	//	auto parentNode = targetNode->GetParent();
	//	auto selectedNodeList = mVm->GetSelectedNodeList();
	//	DragAndDropSource<JG::DDDGameNode>([&](DDDGameNode* ddd)
	//	{
	//		for (auto& node : selectedNodeList)
	//		{
	//			ddd->GameNodeList.push_back(node);
	//			ImGui::TextUnformatted(node->GetName().c_str());
	//		}
	//	});
	//	DragAndDropTarget<DDDGameNode>([&](DDDGameNode* ddd)
	//	{
	//		for (auto& node : ddd->GameNodeList)
	//		{
	//			node->SetParent(targetNode);
	//		}
	//	});

	//}
}

