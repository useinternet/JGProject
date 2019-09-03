#include "pch.h"
#include "IGManager.h"
using namespace std;
// profile
#include "Profile/RenderingProfile.h"
#include "Profile/ConsoleLog.h"
#include "Profile/Performance.h"
// editor
#include "Editor/SceneEditor.h"
#include "Editor/FileViewer.h"






#define SELECT_IGWINDOW(str) if (ImGui::MenuItem(#str)) select_menu = #str;
#define OPEN_IGWINDOW(str)   if (select_menu == #str) CreateIGWindow<##str>(#str);

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
			window_pair.second->DataSave(fout);
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
					if (m_IGWindowPool.find(str) != m_IGWindowPool.end())
						m_IGWindowPool[str]->DataLoad(fin);
				}
			}
			fin.close();
		}
	}
	void IGManager::Update()
	{
		ENGINE_PERFORMANCE_TIMER("Editor", "IGManager::Update");
		for (auto iter = m_IGWindowPool.begin(); iter != m_IGWindowPool.end();)
		{
			auto ig_window = iter->second;
			if (ig_window->IsOpen())
			{
				ENGINE_PERFORMANCE_TIMER("IGManager::Update", ig_window->GetName());
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
						SELECT_IGWINDOW(SceneEditor);
						SELECT_IGWINDOW(FileViewer);
						ImGui::EndMenu();
					}

				}


				// Profile Menu
				{
					if (ImGui::BeginMenu("Profile"))
					{
						SELECT_IGWINDOW(RenderingProfile);
						SELECT_IGWINDOW(ConsoleLog);
						SELECT_IGWINDOW(Performance);
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
			OPEN_IGWINDOW(SceneEditor);
			OPEN_IGWINDOW(FileViewer);
		}
		// Profile Window Open
		{
			OPEN_IGWINDOW(RenderingProfile);
			OPEN_IGWINDOW(ConsoleLog);
			OPEN_IGWINDOW(Performance);
		}

	}
}

