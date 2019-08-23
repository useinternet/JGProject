#include "pch.h"
#include "IGManager.h"

// profile
#include "Profile/RenderingProfile.h"
#include "Profile/ConsoleLog.h"

// editor
#include "Editor/SceneEditor.h"
namespace JE
{
	IGManager::IGManager(EditorGUI* editor) : m_EditorGui(editor)
	{

	}
	void IGManager::Load()
	{



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
				ig_win.second->ReceiveEvent(e);
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


		// Editor Window Open
		{
			if (select_menu == "SceneEditor") CreateIGWindow<SceneEditor>("SceneEditor");
		}
		// Profile Window Open
		{
			if (select_menu == "RenderProfile") CreateIGWindow<RenderingProfile>("RenderProfile");
			if (select_menu == "ConsoleLog") 	CreateIGWindow<ConsoleLog>("ConsoleLog");
		}

	}
}