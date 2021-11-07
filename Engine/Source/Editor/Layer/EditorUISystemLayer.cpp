#include "pch.h"
#include "EditorUISystemLayer.h"
#include "Imgui/imgui.h"
#include "ExternalImpl/JGImGui.h"
// UI
#include "Class/UI/UIView/SceneView.h"
#include "Class/UI/UIView/WorldHierarchyView.h"
#include "Class/UI/UIView/InspectorView.h"
#include "Class/UI/UIView/ContentsView.h"
#include "Class/UI/UIView/ProjectSettingView.h"

// PopupUI
#include "Class/UI/ContextUI/ComponentFinderContextView.h"
#include "Class/UI/ContextUI/AssetFinderContextView.h"
//
#include "Class/UI/ModalUI/ProgressBarModalView.h"
#include "Class/UI/ModalUI/MessageBoxModalView.h"

#include "Class/Game/GameWorld.h"
namespace JG
{
	void EditorUISystemLayer::OnAttach()
	{
	
	}
	void EditorUISystemLayer::OnDetach()
	{

	}


	void EditorUISystemLayer::Begin()
	{
		Init();
		SetMainMenu();
		RegisterUIView();
		RegisterPopupUIView();
		LoadUISettings("JGUI.jgconfig");
	}
	void EditorUISystemLayer::Destroy()
	{
		SaveUISettings("JGUI.jgconfig");
		JGImGui::Destroy();
	}
	void EditorUISystemLayer::OnEvent(IEvent& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<AppResizeEvent>(EVENT_BIND_FN(&EditorUISystemLayer::Resize));
	}
	String EditorUISystemLayer::GetLayerName()
	{
		return "UISystemLayer";
	}

	void EditorUISystemLayer::SetMainMenu()
	{
		UIManager::GetInstance().RegisterMainMenuItem("File/", 0, nullptr, nullptr);
	}

	void EditorUISystemLayer::LoadUISettings(const String& fileName)
	{
		auto json = CreateSharedPtr<Json>();
		if (Json::Read(fileName, json) == false)
		{
			return;
		}
		UIManager::GetInstance().ForEach([&](IUIView* view)
		{
			auto viewJson = json->GetMember(view->GetType().GetName());
			if (viewJson)
			{
				view->LoadJson(viewJson);
			}
		});
	}
	void EditorUISystemLayer::SaveUISettings(const String& fileName)
	{

		Dictionary<String, bool> IsOpen;

		auto json = CreateSharedPtr<Json>();
		
		
		UIManager::GetInstance().ForEach([&](IUIView* view)
		{
			auto viewJson = json->CreateJsonData();
			view->MakeJson(viewJson);
			json->AddMember(view->GetType().GetName(), viewJson);
		});

		Json::Write(fileName, json);
	}

	void EditorUISystemLayer::Init()
	{
		JGImGui::Create();
		Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, SchedulePriority::ImGuiSystemLayer, SCHEDULE_BIND_FN(&EditorUISystemLayer::Update));
		Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, SchedulePriority::UISystemLayer, SCHEDULE_BIND_FN(&EditorUISystemLayer::MenuUpdate));
		UIManager::GetInstance().BindShowContextMenuFunc([&](Type type, bool isWhenItemHovered) -> bool {

			bool isOpenPopup = false;
			if (isWhenItemHovered)
			{
				isOpenPopup = ImGui::BeginPopupContextItem();
			}
			else
			{
				isOpenPopup = ImGui::BeginPopupContextWindow();
			}

			if (isOpenPopup)
			{
				UIManager::GetInstance().ForEach(type, EditorUISystemLayer::BeginMenu, EditorUISystemLayer::EndMenu);
				ImGui::EndPopup();
				return true;
			}
			return false;
		});
	}

	EScheduleResult EditorUISystemLayer::Update()
	{
		JGImGui::GetInstance().NewFrame();

		return EScheduleResult::Continue;
	}

	EScheduleResult EditorUISystemLayer::MenuUpdate()
	{
		ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
		ImGuiWindowFlags   window_flags    = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		window_flags |= ImGuiWindowFlags_NoBackground;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("UISystemLayer", nullptr, window_flags);
		ImGui::PopStyleVar();
		ImGui::PopStyleVar(2);
		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("UISystemLayer DockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			UIManager::GetInstance().ForEach(EditorUISystemLayer::BeginMenu, EditorUISystemLayer::EndMenu);
			ImGui::EndMenuBar();
		}

		ImGui::End();
		return EScheduleResult::Continue;
	}

	bool EditorUISystemLayer::Resize(AppResizeEvent& e)
	{
		if (JGImGui::IsValid() == true)
		{
			JGImGui::GetInstance().Resize(e.Width, e.Height);
		}
		return true;
	}

	void EditorUISystemLayer::RegisterUIView()
	{
		// UI
		UIManager::GetInstance().RegisterUIView<SceneView>();
		UIManager::GetInstance().RegisterUIView<WorldHierarchyView>();
		UIManager::GetInstance().RegisterUIView<InspectorView>();
		UIManager::GetInstance().RegisterUIView<ContentsView>();
		UIManager::GetInstance().RegisterUIView<ProjectSettingView>();
	}

	void EditorUISystemLayer::RegisterPopupUIView()
	{
		// PopupUI
		UIManager::GetInstance().RegisterPopupUIView<ComponentFinderContextView>();
		UIManager::GetInstance().RegisterPopupUIView<AssetFinderContextView>();
		// Modal
		UIManager::GetInstance().RegisterPopupUIView<ProgressBarModalView>();
		UIManager::GetInstance().RegisterPopupUIView<MessageBoxModalView>();
	}

	void EditorUISystemLayer::BeginMenu(const MenuItemNode* Node)
	{

		if (Node->MenuItem == nullptr)
		{
			if (Node->Parent->IsOpen)
			{
				if (Node->IsSperator == true)
				{
					ImGui::Separator();
				}
				Node->IsOpen = ImGui::BeginMenu(Node->Name.c_str());
			}
			else
			{
				Node->IsOpen = false;
			}

		}
		// MenuItem
		else
		{
			if (Node->Parent->IsOpen)
			{
				if (Node->IsSperator == true)
				{
					ImGui::Separator();
				}
				bool isEnable = Node->MenuItem->EnableAction ? Node->MenuItem->EnableAction() : true;
				if (ImGui::MenuItem(Node->Name.c_str(), Node->MenuItem->ShortCut.c_str(), nullptr, isEnable) == true)
				{
					if (Node->MenuItem->Action)
					{
						Node->MenuItem->Action();
					}
				}
			}
		}
		// ShortCut
		if (Node->MenuItem != nullptr && Node->MenuItem->ShortCut.empty() == false)
		{
			auto shortCut = Node->MenuItem->ShortCut;
			bool isCtrl  = shortCut.find("Ctrl") != String::npos;
			bool isShift = shortCut.find("Shift") != String::npos;
			bool isAlt   = shortCut.find("Alt") != String::npos;
			shortCut = ReplaceAll(shortCut, "Ctrl", "");
			shortCut = ReplaceAll(shortCut, "Shift", "");
			shortCut = ReplaceAll(shortCut, "Alt", "");
			shortCut = ReplaceAll(shortCut, " ", "");
			shortCut = ReplaceAll(shortCut, "+", "");

			bool isPressed = false;
			bool isSubPressed = false;
			
			if (isCtrl)  isSubPressed |= ImGui::IsKeyDown((int)EKeyCode::Ctrl);
			if (isShift) isSubPressed |= ImGui::IsKeyDown((int)EKeyCode::Shift);
			if (isAlt)   isSubPressed |= ImGui::IsKeyDown((int)EKeyCode::Alt);
			
			isPressed = ImGui::IsKeyPressed((int)StringToKeyCode(shortCut));
			if ((isPressed && isSubPressed )== true)
			{
				Node->MenuItem->Action();
			}
		}
	}

	void EditorUISystemLayer::EndMenu(const MenuItemNode* Node)
	{
		if (Node->MenuItem == nullptr && Node->IsOpen)
		{
			ImGui::EndMenu();
		}
	}

}