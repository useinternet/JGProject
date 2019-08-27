#include "pch.h"
#include "IGManager.h"
using namespace std;
// profile
#include "Profile/RenderingProfile.h"
#include "Profile/ConsoleLog.h"

// editor
#include "Editor/SceneEditor.h"
#include "Editor/FileViewer.h"
namespace JE
{
	IGManager::IGManager(EditorGUI* editor) : m_EditorGui(editor)
	{

	}
	IGManager::~IGManager()
	{
		ofstream fout;
		fout.open(GlobalLinkData::_EngineConfig->InConfig(GlobalLinkData::_EngineConfig->GetEditorConfigFile()));
		
		for (auto& window_pair : m_IGWindowPool)
		{
			fout << window_pair.first << endl;
		}
		fout.close();
	}
	void IGManager::Load()
	{
		{
			ifstream fin;

			auto config = GlobalLinkData::_EngineConfig;
			auto file = config->GetEditorConfigFile();

			fin.open(config->InConfig(file));
			if (fin.is_open())
			{
				while (!fin.eof())
				{
					std::string str;
					std::getline(fin, str);
					OpenWindow(str);
				}
			}
			fin.close();
		}
	}
	void IGManager::Update()
	{
		for (auto iter = m_IGWindowPool.begin(); iter != m_IGWindowPool.end();)
		{
			auto ig_window = iter->second;
			if (ig_window->IsOpen())
			{
				ig_window->Update();
				++iter;
			}
			else
			{
				ig_window->Destroy();
				iter = m_IGWindowPool.erase(iter);
			}
		}
	}
	void IGManager::OnEvent(Event& e)
	{
		for (auto& ig_win : m_IGWindowPool)
		{
			if (ig_win.second->IsOpen())
			{
				ig_win.second->OnEvent(e);
			}
		}
	}
	void IGManager::OnMainMenu()
	{
		std::string select_menu = "";

		// IGWindow Menu
		{
			if (ImGui::BeginMenu("IGWindow"))
			{
				// Editor Menu
				{
					if (ImGui::BeginMenu("Editor"))
					{
						if (ImGui::MenuItem("SceneEditor")) select_menu = "SceneEditor";
						if (ImGui::MenuItem("FileViewer")) select_menu = "FileViewer";
						ImGui::EndMenu();
					}

				}


				// Profile Menu
				{
					if (ImGui::BeginMenu("Profile"))
					{
						if (ImGui::MenuItem("RenderProfile"))  select_menu = "RenderProfile";
						if (ImGui::MenuItem("ConsoleLog")) select_menu = "ConsoleLog";
						ImGui::EndMenu();
					}
				}

			


				ImGui::EndMenu();
			}

		}

		OpenWindow(select_menu);
	}

	void IGManager::OpenWindow(const std::string& select_menu)
	{

		// Editor Window Open
		{
			if (select_menu == "SceneEditor") CreateIGWindow<SceneEditor>("SceneEditor");
			if (select_menu == "FileViewer") CreateIGWindow<FileViewer>("FileViewer");
		}
		// Profile Window Open
		{
			if (select_menu == "RenderProfile") CreateIGWindow<RenderingProfile>("RenderProfile");
			if (select_menu == "ConsoleLog") 	CreateIGWindow<ConsoleLog>("ConsoleLog");

		}

	}
}

