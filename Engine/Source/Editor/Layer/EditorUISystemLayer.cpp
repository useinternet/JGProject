#include "pch.h"
#include "EditorUISystemLayer.h"
#include "Imgui/imgui.h"
#include "ExternalImpl/JGImGui.h"
// UI
#include "UI/UIView/SceneView.h"
#include "UI/UIView/WorldHierarchyView.h"
#include "UI/UIView/InspectorView.h"
#include "UI/UIView/ContentsView.h"
#include "UI/UIView/ProjectSettingView.h"
#include "UI/UIView/DevelopView.h"
#include "UI/UIView/DevConsoleView.h"
#include "UI/UIView/ModelView.h"
#include "UI/UIView/MaterialView.h"
#include "UI/UIView/PrefabView.h"
#include "UI/UIView/AnimationClipView.h"
#include "UI/UIView/AnimationView.h"
#include "UI/UIView/AnimationBlendSpace1DView.h"

// PopupUI
#include "UI/ContextUI/ComponentFinderContextView.h"
#include "UI/ContextUI/AssetFinderContextView.h"
//
#include "UI/ModalUI/ProgressBarModalView.h"
#include "UI/ModalUI/MessageBoxModalView.h"



#include "UI/InspectorUI/GameInspectorUI.h"
#include "UI/InspectorUI/AssetInspectorUI.h"

namespace JG
{
	void EditorUISystemLayer::OnAttach()
	{
		GlobalSingletonManager::GetInstance().RegisterSingleton<UIManager>();
	}
	void EditorUISystemLayer::OnDetach()
	{
		GlobalSingletonManager::GetInstance().UnRegisterSingleton<UIManager>();
	}


	void EditorUISystemLayer::Begin()
	{
		GlobalSingletonManager::GetInstance().RegisterSingleton<JGImGui>();
		Init();
		SetMainMenu();
		RegisterUIView();
		RegisterPopupUIView();
		RegisterInspectorUI();
		LoadUISettings("JGUI.jgconfig");
	}
	void EditorUISystemLayer::Destroy()
	{
		SaveUISettings("JGUI.jgconfig");
		GlobalSingletonManager::GetInstance().UnRegisterSingleton<JGImGui>();
	}
	void EditorUISystemLayer::OnEvent(IEvent& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<AppResizeEvent>(EVENT_BIND_FN(&EditorUISystemLayer::Resize));
		dispatcher.Dispatch<RequestOpenProgressBarEvent>(EVENT_BIND_FN(&EditorUISystemLayer::ResponseOpenProgressBar));
		dispatcher.Dispatch<RequestDisplayProgressBarEvent>(EVENT_BIND_FN(&EditorUISystemLayer::ResponseDisplayProgressBar));
		dispatcher.Dispatch<RequestCloseProgressBarEvent>(EVENT_BIND_FN(&EditorUISystemLayer::ResponseCloseProgressBar));
		dispatcher.Dispatch<RequestOpenMessageBoxEvent>(EVENT_BIND_FN(&EditorUISystemLayer::ResponseOpenMessageBox));



		if (e.Handled == false)
		{
			UIManager::GetInstance().OnEvent(e);
		}
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
		UIManager::GetInstance().RegisterUIView<DevelopView>();
		UIManager::GetInstance().RegisterUIView<DevConsoleView>();
		UIManager::GetInstance().RegisterUIView<ModelView>();
		UIManager::GetInstance().RegisterUIView<MaterialView>();
		UIManager::GetInstance().RegisterUIView<PrefabView>();
		UIManager::GetInstance().RegisterUIView<AnimationClipView>();
		UIManager::GetInstance().RegisterUIView<AnimationView>();
		UIManager::GetInstance().RegisterUIView<AnimationBlendSpace1DView>();
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
	void EditorUISystemLayer::RegisterInspectorUI()
	{
		UIManager::GetInstance().RegisterInspectorUI<GameNodeInspectorUI>();
		UIManager::GetInstance().RegisterInspectorUI<GameWorldInspectorUI>();
		UIManager::GetInstance().RegisterInspectorUI<TransformInspectorUI>();
		UIManager::GetInstance().RegisterInspectorUI<CameraInspectorUI>();
		UIManager::GetInstance().RegisterInspectorUI<EditorCameraInspectorUI>();
		UIManager::GetInstance().RegisterInspectorUI<PointLightInspectorUI>();
		UIManager::GetInstance().RegisterInspectorUI<SkyDomeInspectorUI>();
		UIManager::GetInstance().RegisterInspectorUI<SpriteRendererInspectorUI>();
		UIManager::GetInstance().RegisterInspectorUI<StaticMeshRendererInspectorUI>();
		UIManager::GetInstance().RegisterInspectorUI<SkeletalMeshRendererInspectorUI>();
		UIManager::GetInstance().RegisterInspectorUI<DirectionalLightInspectorUI>();
		UIManager::GetInstance().RegisterInspectorUI<SpotLightInspectorUI>();
		//

		UIManager::GetInstance().RegisterInspectorUI<MaterialAssetInspectorUI>();
	}
	bool EditorUISystemLayer::ResponseOpenProgressBar(RequestOpenProgressBarEvent& e)
	{
		auto progressBar = UIManager::GetInstance().OpenPopupUIView<ProgressBarModalView>(ProgressBarInitData(e.OpenData.Title));
		if (progressBar != nullptr)
		{
			progressBar->Display(e.OpenData.Contents, e.OpenData.Ratio);
		}

		return true;
	}
	bool EditorUISystemLayer::ResponseDisplayProgressBar(RequestDisplayProgressBarEvent& e)
	{
		auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
		if (progressBar != nullptr)
		{
			progressBar->Display(e.DisplayData.Contents, e.DisplayData.Ratio);
		}
		return true;
	}
	bool EditorUISystemLayer::ResponseCloseProgressBar(RequestCloseProgressBarEvent& e)
	{
		auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
		if (progressBar != nullptr)
		{
			progressBar->Close();
		}
		return true;
	}
	bool EditorUISystemLayer::ResponseOpenMessageBox(RequestOpenMessageBoxEvent& e)
	{
		MessageBoxInitData initData;
		initData.Title = e.OpenData.Title;
		initData.Contents = e.OpenData.Contents;
		initData.MsgType  = (EMessageBoxType)e.OpenData.MsgType;
		initData.ParamName[0] = e.OpenData.ParamName[0];
		initData.ParamName[1] = e.OpenData.ParamName[1];
		initData.ParamAction[0] = e.OpenData.ParamAction[0];
		initData.ParamAction[1] = e.OpenData.ParamAction[1];

		UIManager::GetInstance().OpenPopupUIView<MessageBoxModalView>(initData);


		return true;
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
			shortCut = StringHelper::ReplaceAll(shortCut, "Ctrl", "");
			shortCut = StringHelper::ReplaceAll(shortCut, "Shift", "");
			shortCut = StringHelper::ReplaceAll(shortCut, "Alt", "");
			shortCut = StringHelper::ReplaceAll(shortCut, " ", "");
			shortCut = StringHelper::ReplaceAll(shortCut, "+", "");

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