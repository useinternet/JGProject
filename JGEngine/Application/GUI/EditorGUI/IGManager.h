#pragma once

#include "IGWindow.h"
#include "CommonCore.h"
namespace JE
{
	class EditorGUI;
	class IGManager
	{
	private:

	public:
		IGManager(EditorGUI* editor);
		void Load();
		void Update();
		void OnEvent(Event& e);
		void OnMainMenu();
		EditorGUI* GetEditorGUI() const {
			return m_EditorGui;
		}
	private:
		template<typename CustomWindow>
		void CreateIGWindow(const std::string& name)
		{
			if (m_IGWindowPool.find(name) != m_IGWindowPool.end())
			{
				return;
			}
			std::shared_ptr<IGWindow> gui = std::make_shared<CustomWindow>();
			m_IGWindowPool.emplace(name, gui);
			gui->SetName(name);
			gui->Load(this);
		}
	private:
		using IGWindowPool = std::unordered_map<std::string, std::shared_ptr<IGWindow>>;
		IGWindowPool m_IGWindowPool;
		EditorGUI* m_EditorGui;

	};
}



