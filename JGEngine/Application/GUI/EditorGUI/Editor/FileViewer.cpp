#include "pch.h"
#include "FileViewer.h"
#include <cctype>
#define TABLE_ID(file) "##" + file.Name + "Block"
using namespace std;



namespace JE
{
	void FileViewer::OnLoad()
	{
		m_Contents = std::make_shared<File>();
		m_Sources = std::make_shared<File>();
		m_Contents->Path = GlobalLinkData::_EngineConfig->InContents("");
		m_Contents->Path = m_Contents->Path.substr(0, m_Contents->Path.length() - 1);
		m_Contents->Name = "Contents";
		m_Contents->Is_Directory = true;
		m_Sources->Path = GlobalLinkData::_EngineConfig->InSource("");
		m_Sources->Path = m_Sources->Path.substr(0, m_Sources->Path.length() - 1);
		m_Sources->Name = "Source";
		m_Sources->Is_Directory = true;
		SearchDirectory(m_Contents.get());
		SearchDirectory(m_Sources.get());
		
		m_DirectoryHistory.push(m_Contents.get());

	}
	void FileViewer::OnGUI()
	{
		m_FluctuatingData = FluctuatingData();

		//ImGui::Columns(1);
		static bool is_init_first = true;
		static uint32_t prev_history_size = (uint32_t)m_DirectoryHistory.size();
		if (ImGui::GetIO().KeyCtrl && ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
		{
			m_FluctuatingData.IsCtrl = true;
		}
		// 
		{
			if (ImGui::Button("<"))
			{
				if (m_DirectoryHistory.size() != 1)
				{
					m_DirectoryTemp.push(m_DirectoryHistory.top());
					m_DirectoryHistory.pop();
				}

			}ImGui::SameLine();
			if (ImGui::Button(">"))
			{
				if (!m_DirectoryHistory.size() != 1 && !m_DirectoryTemp.empty())
				{
					m_DirectoryHistory.push(m_DirectoryTemp.front());
					m_DirectoryTemp.pop();
				}
			}ImGui::SameLine();


			ImGui::Text(m_DirectoryHistory.top()->Path.c_str());
		}
		

		
		ImGui::Separator();
		ImGui::Columns(2);

		// Directory Tree
		{
			if (is_init_first)
			{
				ImGui::SetColumnWidth(0, 200);
				is_init_first = false;
			}
			if (ImGui::BeginChild("DirectoryTree"))
			{

				DrawDirectoryTree(m_Contents.get());
				DrawDirectoryTree(m_Sources.get());
			}
			ImGui::EndChild();

		}

		ImGui::NextColumn();


	

		{
			ImGui::BeginGroup();
			if (ImGui::BeginChild("InputChildWindow", { ImGui::GetColumnWidth() - 10.0f, 30 }, false))
			{
			
				char search_input[128] = {};
				ImGui::InputText("##testinput", search_input, 128);

				m_FluctuatingData.Filter_FileName = search_input;
			}
			ImGui::EndChild();
			ImGui::EndGroup();
		}
		// InDirectory
		{
			ImGui::BeginGroup();
		
			if (ImGui::BeginChild("InDirectory", { 0,0 },false, ImGuiWindowFlags_HorizontalScrollbar))
			{
				m_FluctuatingData.TableSize = 200.0f;
				DrawInDirectory(m_DirectoryHistory.top());


				// Viewer Menu
				const std::string popup_id = "ViewerContextsMenu";
				if (m_FluctuatingData.OpenViewerPopUp && ImGui::IsMouseReleased(1) && ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows))
				{
					ImGui::OpenPopup(popup_id.c_str());
				}
				DrawViewerContextMenu(popup_id, m_DirectoryHistory.top());
			}

		}

		if (m_FluctuatingData.SelectedFile)
		{
			m_SelectedFiles.Add(
				m_FluctuatingData.IsCtrl,
				m_FluctuatingData.SelectedFile,
				m_FluctuatingData.IsSelectedFileForContextMenu);
		}
		else 
		{
			if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(0))
			{
				m_SelectedFiles.Reset();
			}
		}

		if (m_CurrContextMenu)
		{
			switch (m_CurrContextMenu->Menu)
			{
			case ContextMenu_File:
				DrawFileContextMenu(m_CurrContextMenu->ID, m_CurrContextMenu->pFile);
				break;
			}
		}
	
		ImGui::EndChild();
		ImGui::EndGroup();


		//
		if (prev_history_size != m_DirectoryHistory.size())
		{
			prev_history_size = (uint32_t)m_DirectoryHistory.size();
			m_SelectedFiles.Reset();
		}
	}
	void FileViewer::OnDestroy()
	{

	}

	void FileViewer::DrawFileContextMenu(const std::string& id, File* file)
	{
		bool is_rename = false;
		bool is_remove = false;
		bool is_open = false;
		bool is_copy = false;
		bool is_move = false;
		if (ImGui::BeginPopup(id.c_str()))
		{
			is_open = ImGui::MenuItem("Open");
			is_copy = ImGui::MenuItem("Copy");
			is_move = ImGui::MenuItem("Move");
			ImGui::Separator();
			is_rename = ImGui::MenuItem("ReName");
			ImGui::Separator();
			is_remove = ImGui::MenuItem("Remove");
			ImGui::EndPopup();
		}
		ReName(is_rename, file);
		//RemoveFile(is_remove, file);
		FileOpen(is_open, file);
		//CopiedFiles(is_copy, file);
		//MovedFiles(is_move, file);
	}

	void FileViewer::DrawViewerContextMenu(const std::string& id, File* directory)
	{
		bool is_create_floder = false;
		bool is_paste = false;
		if (ImGui::BeginPopup(id.c_str()))
		{
			is_paste = ImGui::MenuItem("Paste");
			ImGui::Separator();
			is_create_floder = ImGui::MenuItem("Create Folder");
			ImGui::Separator();
			if (ImGui::BeginMenu("Shader"))
			{
				ImGui::MenuItem("ShaderModule");

				ImGui::EndMenu();
			}

			ImGui::EndPopup();
		}
		CreateFolder(is_create_floder, directory);
		PasteFiles(is_paste, directory);
	}
	void FileViewer::DrawDirectoryTree(File* parent)
	{
		bool is_open = ImGui::TreeNodeEx(parent->Name.c_str(), ImGuiTreeNodeFlags_OpenOnArrow);
		if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered(ImGuiHoveredFlags_None))
		{
			if (m_DirectoryHistory.top() != parent)
			{
				m_DirectoryHistory.push(parent);
				m_DirectoryTemp = std::queue<File*>();
			}
			ENGINE_LOG_TRACE("IsMouseDoubleClicked {0}", parent->Name);
		}
		if (is_open)
		{
			for (auto& file : parent->OwnedFiles)
			{
				if (!file.second->Is_Directory)
					continue;
				DrawDirectoryTree(file.second.get());
			}
			ImGui::TreePop();
		}
	}
	void FileViewer::DrawFile(File* file)
	{
		ImGui::BeginGroup();
		ImGui::PushStyleColor(ImGuiCol_Text, GetFileColor(file));
		bool is_click = ImGui::Selectable(file->Name.c_str(), 
			file->Is_Selected, ImGuiSelectableFlags_AllowDoubleClick, 
			{ m_FluctuatingData.TableSize,m_FluctuatingData.TextSize });
		ImGui::PopStyleColor();
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip(file->Name.c_str());
		}
		// PopUp 오픈
		std::string popup_id = (file->Path + "ContextMenus");
		if (ImGui::OpenPopupOnItemClick(popup_id.c_str(), 1))
		{
			if (m_CurrContextMenu) m_CurrContextMenu.reset();
			m_CurrContextMenu = make_shared<ContextMenu>();
			m_CurrContextMenu->Menu = ContextMenu_File;
			m_CurrContextMenu->ID = popup_id;
			m_CurrContextMenu->pFile = file;
			m_FluctuatingData.IsSelectedFileForContextMenu = true;
			m_FluctuatingData.SelectedFile = file;
		}
		// Viewer PopUp 오픈 여부
		if (ImGui::IsMouseReleased(1) && ImGui::IsItemHovered(ImGuiHoveredFlags_None))
		{
			m_FluctuatingData.OpenViewerPopUp = false;
		}

		// 클릭시 
		if (is_click)
		{
			// 더블 클릭
			if (ImGui::IsMouseDoubleClicked(0))
			{
				if (m_DirectoryHistory.top() != file && file->Is_Directory)
				{
					m_DirectoryHistory.push(file);
					m_DirectoryTemp = std::queue<File*>();
				}
				ENGINE_LOG_TRACE("IsMouseDoubleClicked {0}", file->Name);
				m_FluctuatingData.SelectedFile = file;
			} // 그냥 클릭
			else
			{
				ENGINE_LOG_TRACE("Click {0}", file->Name);
				m_FluctuatingData.SelectedFile = file;
			}
		}
		ImGui::EndGroup();
	}
	void FileViewer::DrawInDirectory(File* file)
	{
		for (auto& file : file->OwnedFiles)
		{
			auto str = file.first;
			auto filter = m_FluctuatingData.Filter_FileName;

			std::transform(str.begin(), str.end(), str.begin(), tolower);
			std::transform(filter.begin(), filter.end(), filter.begin(), tolower);
			if (str.find(filter) == std::string::npos)
				continue;

			DrawFile(file.second.get());
		}
	}
	void FileViewer::SearchDirectory(File* parent)
	{
		auto parent_it = fs::directory_iterator(parent->Path);
		for (auto& file_it : parent_it)
		{
			
			auto fs_path = file_it.path();
			auto file = std::make_shared<File>();
			file->Parent = parent;
			file->Path = fs_path.string();
			file->Extension = fs_path.extension().string();
			file->Is_Directory = fs::is_directory(fs_path);
			file->Name = fs_path.filename().string();
			if (file->Is_Directory)
			{
				SearchDirectory(file.get());
			}
			parent->OwnedFiles.emplace(file->Name, file);
		}
	}
	void FileViewer::ReName(bool is_rename, File* file)
	{
		std::string rename_popup_id = "##ReNameModal";
		static ImVec2 mouse_pos = { 0.0f,0.0f };
		static char str[128] = {};
		if (is_rename)
		{
			ImGui::OpenPopup(rename_popup_id.c_str());
			mouse_pos = ImGui::GetMousePos();
			strcpy_s(str, file->Name.c_str());
		}
		if (ImGui::BeginPopupModal(rename_popup_id.c_str(), 0, ImGuiWindowFlags_NoResize))
		{
			float height = ImGui::GetWindowSize().y;
			ImGui::SetWindowPos({ mouse_pos.x - 75.0f, mouse_pos.y - (height / 2.0f) });
			
	
			ImGui::PushItemWidth(150.0f);
			if (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
				ImGui::SetKeyboardFocusHere(0);
			ImGui::InputText("##ReNameInputText", str, 128);

			ImGui::PopItemWidth();
			ImVec2 window_size = ImGui::GetWindowSize();
			ImVec2 bt_size = { window_size.x / 2 - 12.0f, 20.0f };
			if (ImGui::Button("Ok", bt_size) || ImGui::IsKeyPressed(ImGuiKey_Enter))
			{
				std::string old_path = file->Path;
				std::string temp_path = file->Path.substr(0, file->Path.length() - file->Name.length());

				file->Name = str;
				file->Path = temp_path + file->Name;
				fs::rename(old_path, file->Path);
				ImGui::CloseCurrentPopup();
			}ImGui::SameLine();
			if (ImGui::Button("Cancel", bt_size))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
	void FileViewer::CreateFolder(bool is_create_folder, File* in_directory)
	{
		static ImVec2 mouse_pos = { 0.0f,0.0f };
		static char str[128] = {};
		std::string id = "##" + in_directory->Path + "CreateDirectory";
		if (is_create_folder)
		{
			ImGui::OpenPopup(id.c_str());
			mouse_pos = ImGui::GetMousePos();
			ZeroMemory(str, 128);
		}

		if (ImGui::BeginPopupModal(id.c_str(), 0, ImGuiWindowFlags_NoResize))
		{
			float height = ImGui::GetWindowSize().y;
			ImGui::SetWindowPos({ mouse_pos.x - 75.0f, mouse_pos.y - (height / 2.0f) });
			ImGui::PushItemWidth(150.0f);

			if (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
				ImGui::SetKeyboardFocusHere(0);
			ImGui::InputText("##DirectoryNameInputText", str, 128);
			ImGui::PopItemWidth();
			ImVec2 window_size = ImGui::GetWindowSize();
			ImVec2 bt_size = { window_size.x / 2 - 12.0f, 20.0f };
			if (ImGui::Button("Ok", bt_size) || ImGui::IsKeyPressed(ImGuiKey_Enter))
			{
				auto directory = std::make_shared<File>();
				directory->Name = str;

	
				while (in_directory->FindSameName(directory->Name))
				{
					directory->Name += ("_new");
				}
				directory->Is_Directory = true;
				directory->Path = in_directory->Path +"\\"+ directory->Name;
				directory->Parent = in_directory;
				
				in_directory->OwnedFiles.emplace(directory->Name, directory);
				fs::create_directories(directory->Path);


				ImGui::CloseCurrentPopup();
			}ImGui::SameLine();
			if (ImGui::Button("Cancel", bt_size))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
	void FileViewer::RemoveFile(bool is_delete, File* file)
	{
		if (!is_delete)
			return;
		auto file_list = m_SelectedFiles.FileList;
		m_SelectedFiles.Reset();
		if (file->Is_Directory)
		{
			std::queue<File*> history_que;
			std::stack<File*> temp_stack;
			std::vector<File*> removing_directories;

			for (auto& selected_file : file_list)
			{
				SearchRemovingDirectory(selected_file.second, removing_directories);
			}
			// histroy 공략
			while (m_DirectoryHistory.size() != 1)
			{
				bool is_find = false;
				auto directory = m_DirectoryHistory.top(); 		m_DirectoryHistory.pop();
				for (auto& iter : removing_directories)
				{
					if (iter == directory)
					{
						is_find = true;
						break;
					}
				}

				// 찾지 못했다면 저장
				if (!is_find)
				{
					history_que.push(directory);
				}
			}
			while (!history_que.empty())
			{
				m_DirectoryHistory.push(history_que.front());
				history_que.pop();
			}
			while (!m_DirectoryTemp.empty())
			{
				bool is_find = false;
				auto directory = m_DirectoryTemp.front(); m_DirectoryTemp.pop();
				for (auto& iter : removing_directories)
				{
					if (iter == directory)
					{
						is_find = true;
						break;
					}

				}
				// 찾지 못했다면 저장
				if (!is_find)
				{
					temp_stack.push(directory);
				}
			}
			while (!temp_stack.empty())
			{
				m_DirectoryTemp.push(temp_stack.top());
				temp_stack.pop();
			}
		}
		std::vector<File> remove_file_list;
		for (auto& selected_file : file_list)
		{
			ENGINE_LOG_TRACE("remove {0}", selected_file.second->Path);
			File remove_file;
			remove_file.Parent = selected_file.second->Parent;
			ResetContextMenu(selected_file.second);

			remove_file.Name = selected_file.second->Name;
			remove_file.Path = selected_file.second->Path;
			remove_file_list.push_back(remove_file);
		}
		for (auto& file_info : remove_file_list)
		{
			file_info.Parent->Remove(file_info.Name);
			fs::remove_all(file_info.Path);
		}
	}
	void FileViewer::CopiedFiles(bool is_copy, File* file)
	{
		if (!is_copy)
			return;

		// 나중에 구현
	}
	void FileViewer::PasteFiles(bool is_paste, File* in_directory)
	{
		if (!is_paste)
			return;

		// 나중에 구현
	}
	void FileViewer::MovedFiles(bool is_move, File* file)
	{
		if (!is_move)
			return;

		// 나중에 구현
	}
	void FileViewer::FileOpen(bool is_open, File* file)
	{
		if (!is_open)
			return;

		if (file->Is_Directory)
		{
			OpenDirectory(file);
			return;
		}
		else
		{
			auto fileEnum = ToEnum(file->Extension);

			switch (fileEnum)
			{
			case FileExtention::dds:
				return;
			}
		}
	}
	void FileViewer::SearchRemovingDirectory(File* file, std::vector<File*>& out_list)
	{
		if (file->Is_Directory)
		{
			out_list.push_back(file);
		}
		for (auto& f : file->OwnedFiles)
		{
			SearchRemovingDirectory(f.second.get(), out_list);
		}
	}
	void FileViewer::ResetContextMenu(File* file)
	{
		if (m_CurrContextMenu)
		{
			if (m_CurrContextMenu->pFile == file)
			{
				m_CurrContextMenu.reset();
				m_CurrContextMenu = nullptr;
			}
		}
	}
	ImVec4 FileViewer::GetFileColor(File* file)
	{
		if (file->Is_Directory)
			return { 0.0f,1.0f,0.0f,1.0f };
		else
		{

			return { 1.0f,1.0f,1.0f,1.0f };
		}
	}
	void FileViewer::OpenDirectory(File* file)
	{
		if (m_DirectoryHistory.top() != file)
		{
			m_DirectoryHistory.push(file);
			m_DirectoryTemp = std::queue<File*>();
		}
	}
}