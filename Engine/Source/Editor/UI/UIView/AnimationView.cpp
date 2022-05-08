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
		mNodeEditor = CreateUniquePtr<NodeGUI::NodeEditor>();
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
		NodeGUI::NodeBuilder nodeBuilder;
		nodeBuilder.SetInitLocation(JVector2(250, 100));
		nodeBuilder.SetNodeFlags(NodeGUI::ENodeFlags::LinkOutPin);
		nodeBuilder.SetHeader("Root");
		mNodeEditor->CreateNode(nodeBuilder);

	}

	void AnimationView::CreateAnimationClipNode()
	{
		ImVec2 mousePos = ImGui::GetMousePos();
		JVector2 winPos   = mNodeEditor->GetWindowPos();
		NodeGUI::NodeBuilder nodeBuilder;
		
		nodeBuilder.SetInitLocation(JVector2(mousePos.x - winPos.x, mousePos.y - winPos.y));
		nodeBuilder.SetNodeFlags(NodeGUI::ENodeFlags::LinkPin);
		nodeBuilder.SetHeader("AnimationClip");
		mNodeEditor->CreateNode(nodeBuilder);
	}

	void AnimationView::UpdateScene()
	{

	}

}