#include "pch.h"
#include "WorldHierarchyView.h"
#include "Class/Game/GameNode.h"
#include "Imgui/imgui.h"

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


		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Add/EmptyObject", 0, [&]()
		{
			mVm->AddEmptyObject->Execute(mVm->GetSelectdNodeInContextMenu());
		}, nullptr);

		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Delete", 0, 
			[&]()
		{
			mVm->DeleteGameNode->Execute(mVm->GetSelectdNodeInContextMenu());
		}, 
			[&]() -> bool
		{
			if (mVm->GetSelectdNodeInContextMenu() != nullptr)
			{
				return mVm->GetSelectdNodeInContextMenu()->GetParent() != nullptr;
			}
			return false;
		});
	}
	void WorldHierarchyView::Initialize()
	{
		mVm = GetViewModel();
	}
	void WorldHierarchyView::OnGUI()
	{
		ImGui::Begin("WorldHierarchy", &mOpenGUI);

		mVm->ForEach(
			[&](WorldHierarchyTreeNode& nodeData) -> bool
		{
			bool isOpen = false;
			bool isRoot = nodeData.Object->GetParent() == nullptr;
			bool isLeaf = nodeData.Object->GetChildCount() == 0;
			nodeData.UserFlags |= ImGuiTreeNodeFlags_OpenOnArrow;

			(isLeaf) ?
				nodeData.UserFlags |= ImGuiTreeNodeFlags_Leaf :
				nodeData.UserFlags &= ~(ImGuiTreeNodeFlags_Leaf);

			if (isRoot == true)
			{
				isOpen = ImGui::CollapsingHeader((nodeData.Object->GetName() + "##GameWorld").c_str());
				nodeData.IsTreePop = false;
			}
			else
			{
				isOpen = ImGui::TreeNodeEx((void*)nodeData.Object, nodeData.UserFlags, nodeData.Object->GetName().c_str());
				nodeData.IsTreePop = isOpen;
			}

			return isOpen;
		},
			[&](WorldHierarchyTreeNode& nodeData)
		{
			static bool isContextOpen = false;
			ImGui::PushID(nodeData.Object);

			if (UIManager::GetInstance().ShowContextMenu(GetType()) == true)
			{
				if (isContextOpen == false)
				{
					isContextOpen = true;
					mVm->SetSelectedNodeInContextMenu(nodeData.Object);
				}
			}
			else
			{
				isContextOpen = false;
				if (ImGui::IsItemClicked() == true)
				{
					mVm->SetSelectedNodeInInspector(nodeData.Object);
				}
			}
			(nodeData.IsSelected == true) ?
				nodeData.UserFlags |= ImGuiTreeNodeFlags_Selected :
				nodeData.UserFlags &= ~ImGuiTreeNodeFlags_Selected;

			ImGui::PopID();
		},
			[&](WorldHierarchyTreeNode& nodeData)
		{
			ImGui::TreePop();
		});
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
}

