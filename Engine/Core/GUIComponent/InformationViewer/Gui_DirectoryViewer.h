#pragma once
#include"GuiComponent.h"

namespace JgGui
{

	class Gui_DirectoryViewer : public GuiComponent
	{
		struct DirectoryNode
		{
			std::string name;
			std::string path;
			DirectoryNode* parent = nullptr;
			std::vector<DirectoryNode*> datas;
			int  DirectoryCount = 0;
			bool IsDirectroy = false;
			bool IsClicked = false;
			bool IsOpen = false;
		};
	private:
		bool m_FirstFrame = true;
		std::vector<std::unique_ptr<DirectoryNode>> m_DirectroyNodeMems;
		DirectoryNode* m_RootNode = nullptr;
		DirectoryNode* m_Curr_Clicked_Node = nullptr;
		DirectoryNode* m_Prev_Clicked_Node = nullptr;
		DirectoryNode* m_Selected_Node = nullptr;
		float m_DirNodeItemSize = 0.0f;
		//
		ImVec4 m_Dir_Open_Color = { 0.0f,1.0f,0.0f,1.0f };
		ImVec4 m_Dir_Close_Color = { 0.2f,0.2f,0.2f,1.0f };
		//
		float m_FileBoxSize = 50.0f;
	public:
		Gui_DirectoryViewer(const std::string& name = "DirectoryViewer");
		virtual ~Gui_DirectoryViewer();
		virtual void NewFrame() override;
	private:
		void OnGui_MainMenu();
		void OnGui_DirectroySearchWindow(DirectoryNode* CurrDirNode);
		void OnGui_FileViewerWindow();
	private:
		DirectoryNode* CreateDirectoryNode();
		void SearchRecursiveDirectoryNode(
			const fs::directory_iterator& directoryIter,
			DirectoryNode* directoryNode);
	};
}