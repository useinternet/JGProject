#include "pch.h"
#include "AnimationView.h"
#include "Application.h"
#include "ExternalImpl/JGImGui.h"
#include "ExternalImpl/TextEditor.h"
#include "Class/Asset/Asset.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/GraphicsHelper.h"
#include "UI/UIManager.h"
#include "UI/EditorUIScene.h"
#include "UI/NodeGUI.h"
#include "UI/UIManager.h";

namespace JG
{
	AnimationView::AnimationView()
	{
		DisableUniqueView();

	}

	void AnimationView::Load()
	{
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Create/AnimationClip", 0, [&]()
		{
			CreateAnimationClipNode();
			JG_LOG_INFO("Create Node");
		}, nullptr);
	}

	void AnimationView::Initialize()
	{
		mNodeEditor = CreateUniquePtr<StateNodeGUI::StateNodeEditor>();
		mNodeEditor->BindContextMenuFunc([&]()
		{
			if (UIManager::GetInstance().ShowContextMenu(GetType(), false) == true)
			{

			}
		});
		CreateRootNode();
	}

	void AnimationView::OnGUI()
	{
		mNodeEditor->OnGUI();

	}


	void AnimationView::Destroy()
	{
	}

	void AnimationView::OnEvent(IEvent& e)
	{
	}

	void AnimationView::CreateRootNode()
	{
		StateNodeGUI::StateNodeBuilder nodeBuilder;
		nodeBuilder.SetInitLocation(JVector2(250, 100));
		nodeBuilder.SetName("Root");
		nodeBuilder.SetNodeFlags(
			StateNodeGUI::EStateNodeFlags::No_Remove | 
			StateNodeGUI::EStateNodeFlags::No_ReName | 
			StateNodeGUI::EStateNodeFlags::RootNode);
		mNodeEditor->CreateNode(nodeBuilder);
	}

	void AnimationView::CreateAnimationClipNode()
	{
		JVector2 offset = mNodeEditor->GetOffset();
		ImVec2 mousePos = ImGui::GetMousePos();
		JVector2 winPos = mNodeEditor->GetWindowPos();
		StateNodeGUI::StateNodeBuilder nodeBuilder;
		nodeBuilder.SetInitLocation(JVector2(mousePos.x - winPos.x, mousePos.y - winPos.y) - offset);
		nodeBuilder.SetName("AnimationClip");
		mNodeEditor->CreateNode(nodeBuilder);
	}

	void AnimationView::UpdateScene()
	{

	}

}