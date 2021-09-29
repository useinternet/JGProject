#include "pch.h"
#include "WorldHierarchyView.h"
#include "Imgui/imgui.h"
#include "Class/Game/GameNode.h"
#include "Class/Game/GameWorld.h"
#include "Class/UI/UIView/InspectorView.h"
#include "Class/Game/Components/SpriteRenderer.h"
#include "Class/Game/Components/StaticMeshRenderer.h"
#include "Class/Game/Components/PointLight.h"
#include "Class/Game/Components/Camera.h"


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

		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Copy %_C", 0,
			[&]()
		{
			Copy();
		},
			[&]() -> bool
		{
			bool result = true;
			for (auto& node : mSelectedGameNode)
			{
				if (node->Is<GameWorld>() == true || node->Is<EditorCamera>())
				{
					result = false;
					break;
				}
			}
			return result;
		});
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Move", 0,
			[&]()
		{
			Copy(true);
		},
			[&]() -> bool
		{
			return IsCanAbleToCopy();
		});

		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Paste", 0,
			[&]()
		{
			Paste();
		}, [&]() -> bool
		{
			return IsCanAbleToPaste();
		});

		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Delete", 0, 
			[&]()
		{
			for (auto& node : mSelectedGameNode)
			{
				GameObject::DestoryObject(node);
			}
			mSelectedGameNode.clear();
		}, 
			[&]() -> bool
		{
			return IsCanAbleToDelete();
		});
	}
	void WorldHierarchyView::Initialize()
	{
		mIsCtrl = false;
		mGameWorld = nullptr;


		mTargetGameNode = nullptr;
		mSelectedGameNode.clear();
		mTreeNodeStatePool.clear();
	}
	void WorldHierarchyView::OnGUI()
	{
		mIsCtrl = ImGui::IsKeyDown((i32)EKeyCode::Ctrl);

		ImGui::Begin("WorldHierarchy", &mOpenGUI);
		UpdateNode();
		if (ImGui::BeginTable("WorldHierarchy_Table", 4) == true)
		{
			ImGui::TableSetupColumn("Object");
			ImGui::TableSetupColumn("##Move", ImGuiTableColumnFlags_WidthFixed, 56.0f);
			ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 100.0F);
			ImGui::TableSetupColumn("##Active", ImGuiTableColumnFlags_WidthFixed, 28.0f);
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
		PostProcess();

		if (mIsCtrl && ImGui::IsWindowFocused())
		{
			if (ImGui::IsKeyPressed((i32)EKeyCode::C) && IsCanAbleToCopy())
			{
				Copy();
			}
			if (ImGui::IsKeyPressed((i32)EKeyCode::X) && IsCanAbleToCopy())
			{
				Copy(true);
			}
			if (ImGui::IsKeyPressed((i32)EKeyCode::V) && IsCanAbleToPaste())
			{
				Paste();
			}
		}

		ImGui::End();
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

		i32  treeFlags  = ImGuiTreeNodeFlags_OpenOnArrow  | ImGuiTreeNodeFlags_FramePadding;
		u32  tableColor = 0;
		bool isActive  = gameNode->IsActive();
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

		if (isActive == false)
		{
			for (int i = 0; i < 4; ++i)
			{
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(100, 100, 100, 100), i);
			}
		}
		else if (treeNodeState->IsState(NodeState_Selected) == true)
		{
			
			auto color = ImGui::GetStyle().Colors[ImGuiCol_Button];
			for (int i = 0; i < 4; ++i)
			{
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(color.x * 255, color.y * 255, color.z * 255, color.w * 100), i);
			}
		}
		 
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
		if (ImGui::ArrowButton(("##UpButton" + gameNode->GetName()).c_str(), ImGuiDir_Up) || (mTargetGameNode == gameNode && ImGui::IsKeyPressed((i32)EKeyCode::NumPadSubtract)))
		{
			auto parent = gameNode->GetParent();
			if (parent != nullptr)
			{
				auto ppData = CreateSharedPtr<PP_MoveInParentData>();
				ppData->Command  = PostProcess_NodeMoveUp;
				ppData->GameNode = gameNode;
				mPostProcessQueue.push(ppData);
			}
		}ImGui::SameLine();
		if (ImGui::ArrowButton(("##DownButton" + gameNode->GetName()).c_str(), ImGuiDir_Down) || (mTargetGameNode == gameNode && ImGui::IsKeyPressed((i32)EKeyCode::NumPadAdd)))
		{
			auto parent = gameNode->GetParent();
			if (parent != nullptr)
			{
				auto ppData = CreateSharedPtr<PP_MoveInParentData>();
				ppData->Command = PostProcess_NodeMoveDown;
				ppData->GameNode = gameNode;
				mPostProcessQueue.push(ppData);
			}
		}

		ImGui::TableNextColumn();
		ImGui::Text(ReplaceAll(gameNode->GetType().GetName(), "JG::", "").c_str());
		ImGui::TableNextColumn();

		if (ImGui::Checkbox(("##Active_CheckBox" + gameNode->GetName()).c_str(), &isActive) && gameNode->GetParent() != nullptr)
		{
			gameNode->SetActive(isActive);
		}

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

	void WorldHierarchyView::PostProcess()
	{
		while (mPostProcessQueue.empty() == false)
		{
			auto data = mPostProcessQueue.front(); mPostProcessQueue.pop();

			switch (data->Command)
			{
			case PostProcess_NodeMoveUp:
			{
				auto move_in_parent_data = data->As<PP_MoveInParentData>();
				if (move_in_parent_data != nullptr && move_in_parent_data->GameNode->GetParent())
				{
					auto p = move_in_parent_data->GameNode->GetParent();
					u64 curIndex = p->GetNodeIndex(move_in_parent_data->GameNode);
					if (curIndex > 0)
					{
						p->Swap(curIndex, curIndex - 1);
					}
				
				}
			}
			break;
			case PostProcess_NodeMoveDown:
			{
				auto move_in_parent_data = data->As<PP_MoveInParentData>();
				if (move_in_parent_data != nullptr && move_in_parent_data->GameNode->GetParent())
				{
					auto p = move_in_parent_data->GameNode->GetParent();
					u64 curIndex = p->GetNodeIndex(move_in_parent_data->GameNode);
					p->Swap(curIndex, curIndex + 1);

				}
			}
				break;
			}


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
	void WorldHierarchyView::Copy(bool is_remove_gamenode_after_copy)
	{
		mGameNodeListToDeleteAfterCopy.clear();
		List<GameNode*> removeList;
		auto copyGameNodes = mSelectedGameNode;
		for (auto& node : copyGameNodes)
		{
			for (auto& parentNode : copyGameNodes)
			{
				if (parentNode == node) continue;
				if (node->GetParent() == parentNode)
				{
					removeList.push_back(node);
					break;
				}
			}
		}
		for (auto& node : removeList)
		{
			copyGameNodes.erase(node);
		}

		mGameNodeJsonToCopy = CreateSharedPtr<Json>();
		for (auto& node : copyGameNodes)
		{
			auto jsonData = mGameNodeJsonToCopy->CreateJsonData();
			node->MakeJson(jsonData);
			mGameNodeJsonToCopy->AddMember(jsonData);
		}
		if (is_remove_gamenode_after_copy)
		{
			mGameNodeListToDeleteAfterCopy = copyGameNodes;
		}
	}
	void WorldHierarchyView::Paste()
	{
		if (mTargetGameNode == nullptr) return;
		if (mGameNodeJsonToCopy == nullptr) return;

		u64 copyCnt = mGameNodeJsonToCopy->GetSize();
		for (u64 i = 0; i < copyCnt; ++i)
		{
			auto jsonData = mGameNodeJsonToCopy->GetJsonDataFromIndex(i);
			auto node = mTargetGameNode->AddNode("PastedNode");
			node->LoadJson(jsonData);
		}

		for (auto& node : mGameNodeListToDeleteAfterCopy)
		{
			GameObject::DestoryObject(node);
		}
	}

	bool WorldHierarchyView::IsCanAbleToCopy()
	{
		bool result = true;
		for (auto& node : mSelectedGameNode)
		{
			if (node->Is<GameWorld>() == true || node->Is<EditorCamera>())
			{
				result = false;
				break;
			}
		}
		return result;
	}

	bool WorldHierarchyView::IsCanAbleToPaste()
	{
		return mGameNodeJsonToCopy != nullptr;
	}

	bool WorldHierarchyView::IsCanAbleToDelete()
	{
		return IsCanAbleToCopy();
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

