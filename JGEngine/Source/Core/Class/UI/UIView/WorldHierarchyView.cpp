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
			//mVm->CreateEmptyObject->Execute();
		}, nullptr);
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Create/2D/Sprite", 0, [&]()
		{
			//mVm->CreateSprite->Execute();
		}, nullptr);
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Create/3D/Cube", 0, [&]()
		{
			//mVm->CreateCube->Execute();
		}, nullptr);
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Create/3D/Sphere", 0, [&]()
		{
			//mVm->CreateSphere->Execute();
		}, nullptr);
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Create/Light/PointLight", 0, [&]()
		{
			//mVm->CreatePointLight->Execute();
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
		//mVm = GetViewModel();
	}
	void WorldHierarchyView::OnGUI()
	{
		ImGui::Begin("WorldHierarchy", &mOpenGUI);

		if (ImGui::BeginTable("TestTable", 2) == true)
		{
			ImGui::TableSetupColumn("Object");
			ImGui::TableSetupColumn("Type");
			ImGui::TableHeadersRow();
			GameNode_OnGUI(mGameWorld);
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

		//	// ���� isSelectedNode == false 
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

		ImGui::TableNextColumn();
		bool isOpen = ImGui::TreeNodeEx((const void*)gameNode, ImGuiTreeNodeFlags_None | ImGuiTreeNodeFlags_OpenOnArrow, gameNode->GetName().c_str());
		ImGui::TableNextColumn();
		ImGui::Text(gameNode->GetType().GetName().c_str());
		if (isOpen == true)
		{
			u64 childCnt = gameNode->GetChildCount();
			for (auto i = 0; i < childCnt; ++i)
			{
				GameNode_OnGUI(gameNode->FindNode(i));
			}
			ImGui::TreePop();
		}


	}
	bool WorldHierarchyView::ResponseDestroyGameObject(NotifyDestroyJGObjectEvent& e)
	{
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

