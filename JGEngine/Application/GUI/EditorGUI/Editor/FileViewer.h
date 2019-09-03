#pragma once
#include"GUI/EditorGUI/IGWindow.h"
#include <stack>

namespace JE
{

	// 트리 문제 다루기
	class FileViewer : public IGWindow
	{
		struct File;
		struct SlectedFileList;
		struct FluctuatingData;
		struct SlectedFileList;
	public:
		virtual void OnLoad() override;
		virtual void OnGUI() override;
		virtual void OnDestroy() override;
	private: // DrawMenu
		void DrawFileContextMenu(const std::string& id, File* file);
		void DrawViewerContextMenu(const std::string& id, File* directory);
	private: // Draw
		void DrawDirectoryTree(File* parent);
		void DrawFile(File* file);
		void DrawInDirectory(File* file);
		void SearchDirectory(File* parent);
	private: // MenuExecute
		void ReName(bool is_rename, File* file);
		void CreateFolder(bool is_create_folder, File* in_directory);
		void RemoveFile(bool is_delete, File* file);
		void CopiedFiles(bool is_copy, File* file);
		void PasteFiles(bool is_paste, File* in_directory);
		void MovedFiles(bool is_move, File* file);
		void FileOpen(bool is_open, File* file);
	private: // utill
		void SearchRemovingDirectory(File* in_directory, std::vector<File*>& out_list);
		void ResetContextMenu(File* file);
		ImVec4 GetFileColor(File* file);
	private:
		void OpenDirectory(File* file);
	private:
		enum ContextMenuType
		{
			ContextMenu_File,
		};
		enum class FileExtention
		{
			none,
			dds,
		};
		struct File
		{
			File* Parent = nullptr;
			std::string Path;
			std::string Name;
			std::string Extension;
			std::map<std::string, std::shared_ptr<File>> OwnedFiles;
			bool Is_Directory;
			bool Is_Selected = false;
			void Remove(const std::string& name)
			{
				OwnedFiles.erase(name);
			}
			void Remove(File* file)
			{
				OwnedFiles.erase(file->Name);
			}
			bool Find(File* file)
			{
				return OwnedFiles.find(file->Name) != OwnedFiles.end();
			}
			bool FindSameName(const std::string& name)
			{
				return OwnedFiles.find(name) != OwnedFiles.end();
			}
		};

		struct ContextMenu
		{
			ContextMenuType Menu;
			std::string ID;;
			File* pFile = nullptr;
		};
		struct FluctuatingData
		{
			std::string Filter_FileName;
			File* SelectedFile = nullptr;
			bool  IsSelectedFileForContextMenu = false;
			bool  IsCtrl = false;
			bool  OpenViewerPopUp = true;
			float TableSize = 0.0f;
			float TextSize = 15.0f;
		};
		struct SlectedFileList
		{
			std::map<std::string, File*> FileList;
			void Reset()
			{
				for (auto& file : FileList)
				{
					file.second->Is_Selected = false;
				}
				ENGINE_LOG_TRACE("reset");
				FileList.clear();
			}
			void Add(bool is_ctrl, File* file, bool is_context)
			{
				if (FileList.find(file->Name) != FileList.end())
					return;
				if (!is_ctrl && !is_context)
				{
					ENGINE_LOG_TRACE("not ctrl add");
					Reset();
				}
				file->Is_Selected = true;
				FileList.emplace(file->Name, file);
			}
		};
	private:
		std::shared_ptr<File>  m_Contents;
		std::shared_ptr<File>  m_Sources;
		std::shared_ptr<ContextMenu> m_CurrContextMenu;
		FluctuatingData m_FluctuatingData;
		SlectedFileList m_SelectedFiles;
		
		std::stack<File*>  m_DirectoryHistory;
		std::queue<File*>  m_DirectoryTemp;
	private:
		inline FileExtention ToEnum(const std::string& extension) {
			if (_stricmp(extension.c_str(), "dds") == 0) return FileExtention::dds;
			else return FileExtention::none;
		}

	};
}