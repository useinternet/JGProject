#include "pch.h"
#include "EditorGUI.h"

#include "GUI/EditorGUI/ImGui/imgui_internal.h"
#include "GUI/EditorGUI/ImGui/imgui_impl_dx12.h"
#include "GUI/EditorGUI/ImGui/imgui_impl_win32.h"
#include "Object/DxObject/RenderDevice.h"

#include "EditorGUI/IGManager.h"
using namespace std;

namespace JE
{
	EditorGUI::EditorGUI()
	{
		
	}
	void EditorGUI::Init(RE::RenderDevice* renderDevice)
	{
		m_pRenderDevice = renderDevice;
		auto render_device_desc = renderDevice->GetDesc();
		m_hWnd = render_device_desc.hWnd;
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		auto filename  = GlobalLinkData::_EngineConfig->GetImGuiConfigFile();
		static std::string file_path = GlobalLinkData::_EngineConfig->InConfig(filename);
		io.IniFilename = file_path.c_str();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}


		ImGui_ImplWin32_Init(m_hWnd);
		ImGui_ImplDX12_Init(renderDevice);
		if (!ImGui_ImplDX12_CreateDeviceObjects())
		{
			assert(false && "failed prepare imgui");
		}
		ENGINE_LOG_INFO("EditorGui Init");
	}

	void EditorGUI::Load()
	{
		if (!m_IGManager)
		{
			m_IGManager = make_shared<IGManager>(this);
			m_IGManager->Load();
		}
	}
	void EditorGUI::Destroy()
	{
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		ENGINE_LOG_INFO("EditorGui Destroy");
	}

	void EditorGUI::Render(ID3D12GraphicsCommandList* cmdList)
	{
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList);
	}
	void EditorGUI::OnEvent(Event& e)
	{
		m_IGManager->OnEvent(e);
	}
	void EditorGUI::Update()
	{
		ENGINE_PERFORMANCE_TIMER("Application", "Editor");
		{
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
			ApplyDockSpace();
		}
		ImGui::ShowDemoWindow();
		m_IGManager->Update();
		if (ImGui::BeginMainMenuBar())
		{
			m_IGManager->OnMainMenu();
			ImGui::EndMainMenuBar();
		}
	}
	void EditorGUI::ApplyDockSpace()
	{
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("JGEngine_DockSpace", nullptr, window_flags);
		ImGui::PopStyleVar();
	//	ImVec2 size = ImGui::GetWindowSize();




		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("JGEngine_DockSpaceID");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		else
		{
			ImGuiIO& io = ImGui::GetIO();
			ImGui::Text("ERROR: Docking is not enabled! See Demo > Configuration.");
			ImGui::Text("Set io.ConfigFlags |= ImGuiConfigFlags_DockingEnable in your code, or ");
			ImGui::SameLine(0.0f, 0.0f);
			if (ImGui::SmallButton("click here"))
				io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		}
		ImGui::End();
	}
}