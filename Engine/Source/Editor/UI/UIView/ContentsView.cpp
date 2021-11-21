#include "pch.h"
#include "ContentsView.h"
#include "Common/DragAndDrop.h"
#include "ExternalImpl/JGImGui.h"
#include "ExternalImpl/ImFileDialog.h"
#include "Class/Asset/AssetImporter.h"
#include "GameObjectFactory.h"
#include "Components/Camera.h"
#include "GameNode.h"
#include "GameWorld.h"
#include "Graphics/Material.h"
#include "Graphics/Shader.h"
#include "UI/ModalUI/ProgressBarModalView.h"
#include "UI/ModalUI/MessageBoxModalView.h"
#include "UI/UIManager.h"
namespace JG
{
	ContentsView::ContentsView()
	{
		UIManager::GetInstance().RegisterMainMenuItem("Windows/ContentsView", 0, [&]()
		{
			Open();
		}, nullptr);
	}
	void ContentsView::Load()
	{
		// Copy Paste Delete Move 이정도만 일단 생성
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Create/Folder", 0, [&]() {
			CreateFolder(GetTargetDirectory());
		}, nullptr);
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Create/GameWorld", 0, [&]() {
			CreateGameWorld(GetTargetDirectory());
		}, nullptr);
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Create/Material/Surface", 0, [&]() {
			CreateSurfaceMaterial(GetTargetDirectory());
		}, nullptr);
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Create/Material/Lighting", 0, [&]() {}, nullptr);
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Copy", 20, [&]() {
			Copy();
		}, nullptr);
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Paste", 20, [&]() {
			Paste();
		}, nullptr);
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Move", 20, [&]() {
			Move();
		}, nullptr);
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Delete", 20, [&]() {
			Delete();
		}, nullptr);
		LoadIcons();


	}
	void ContentsView::Initialize()
	{
		mHistroyIndex = -1;
		mSelectedFilesInFilePanel.clear();
		mSelectedFilesInDirPanel.clear();
		mDirectoryHistory.clear();

		SetTargetDirectory(Application::GetAssetPath());
		mTaskFlags = Task_ColumnInit;
	}
	void ContentsView::OnGUI()
	{
		mIsCtrl = ImGui::IsKeyDown((i32)EKeyCode::Ctrl);
		UpdateNode();

		ImGui::Begin("ContentsView", &mOpenGUI);
		OnGUI_Top();
		ImGui::Separator();
		OnGUI_Bottom();
		ImGui::End();


		UpdateGUI();
		UpdateTask();
		if (mOpenGUI == false)
		{
			mOpenGUI = true;
			Close();
		}
	}
	void ContentsView::Destroy()
	{
		if (mAsyncUpdateDirectory.ThreadTaskHandle != nullptr)
		{
			mAsyncUpdateDirectory.ThreadTaskHandle->Reset();
			mAsyncUpdateDirectory.ThreadTaskHandle = nullptr;
		}
		if (mAsyncUpdateImport.ThreadTaskHandle != nullptr)
		{
			mAsyncUpdateImport.ThreadTaskHandle->Reset();
			mAsyncUpdateImport.ThreadTaskHandle = nullptr;
		}
		if (mAsyncUpdatePaste.ThreadTaskHandle != nullptr)
		{
			mAsyncUpdatePaste.ThreadTaskHandle->Reset();
			mAsyncUpdatePaste.ThreadTaskHandle = nullptr;
		}
	}
	void ContentsView::OnGUI_Top()
	{
		ImGui::Columns(2, nullptr, true);

		if (ImGui::Button("Import") == true)
		{
			ImGui::FileDialog::Instance().Open(GetType().GetName(), "Import FileDialog", "*.*", true, fs::absolute(Application::GetAssetPath()).string());
		}

		ImGui::NextColumn();




		if (ImGui::ArrowButton("##Prev_Button", ImGuiDir_Left) == true)
		{
			PrevTargetDirectory();
		}ImGui::SameLine();

		if (ImGui::ArrowButton("##Next_Button", ImGuiDir_Right) == true)
		{
			NextTargetDirectory();
		}ImGui::SameLine();

		if (ImGui::ArrowButton("##Up_Button", ImGuiDir_Up) == true)
		{
			auto node = GetTargetDirectoryNode();
			auto pDir = node->OwnerDirectory;
			if (pDir != nullptr)
			{
				SetTargetDirectory(pDir->Path);
			}

		}ImGui::SameLine();
		ImGui::Dummy(ImVec2(20.0f, 0.0f)); ImGui::SameLine();

		auto contentsPath = ReplaceAll(GetTargetDirectory(), Application::GetAssetPath(), "Asset");
		auto dirList = Split(contentsPath, '/');


		i32 index = 0;
		String path = Application::GetAssetPath();
		for (auto& folder : dirList)
		{
			if (index > 0)
			{
				path = CombinePath(path, folder);
			}
			if (ImGui::Button(folder.c_str()) == true) 
			{
				SetTargetDirectory(path);
			}ImGui::SameLine();
			if (ImGui::ArrowButton("##right", ImGuiDir_Right) == true)
			{
				//
			}
			++index;
			if (index < dirList.size()) {
				ImGui::SameLine();
			}
		}
		ImGui::NextColumn();
	}
	void ContentsView::OnGUI_Bottom()
	{
		ImGui::Columns(2, nullptr, true);

		if (mTaskFlags & Task_ColumnInit)
		{
			mTaskFlags &= ~Task_ColumnInit;
			f32 winWidth = ImGui::GetWindowWidth();
			ImGui::SetColumnWidth(0, winWidth * 0.25f);
		}




		OnGUI_DirectoryPanel();
		ImGui::NextColumn();
		OnGUI_FilePanel_TopBar();
		OnGUI_FilePanel();



	}
	void ContentsView::OnGUI_DirectoryPanel()
	{
		ImGui::BeginChild("DirectoryPanel", ImVec2(0, 0), true);

		if (mRootDirectory != nullptr)
		{
			OnGUI_Directory(mRootDirectory);
		}


		if (ImGui::IsAnyItemHovered() == false)
		{
			if (ImGui::IsMouseClicked(0))
			{
				SetTargetFile("");
				mSelectedFilesInFilePanel.clear();
				mSelectedFilesInDirPanel.clear();
			}
		}

		ImGui::EndChild();
	}
	void ContentsView::OnGUI_Directory(DirectoryNode* dirNode)
	{
	
		auto treeNodeState = GetDirectoryNodeState(dirNode->Path);
		if (treeNodeState == nullptr) {
			return;
		}

		u64 flags = ImGuiTreeNodeFlags_OpenOnArrow;
		if (dirNode->DirectoryList.empty() == true)
		{
			flags = (u64)(flags | ImGuiTreeNodeFlags_Leaf);
		}
		if (treeNodeState->IsState(NodeState_Selected))
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}
		if (dirNode == mRootDirectory)
		{
			flags |= ImGuiTreeNodeFlags_DefaultOpen;
		}

		bool isOpen = ImGui::TreeNodeEx(dirNode->FileName.c_str(), flags);
		bool isItemClick    = ImGui::IsItemClicked();
		bool isClearSelectedFiles = mIsCtrl == false;
		if (UIManager::GetInstance().ShowContextMenu(GetType()) == true)
		{
			if (mIsCtrl == false && treeNodeState->IsState(NodeState_Selected) == false)
			{
				isClearSelectedFiles = true;
			}
			else
			{
				isClearSelectedFiles = false;
			}
			isItemClick = true;
		}
		if (isItemClick) {
			mSelectedFilesInFilePanel.clear();
			if (isClearSelectedFiles) {
				mSelectedFilesInDirPanel.clear();
			}
			mSelectedFilesInDirPanel.insert(dirNode->Path);
			SetTargetDirectory(dirNode->Path);
		}


		if (isOpen)
		{
			for each (auto dir in dirNode->DirectoryList)
			{
				OnGUI_Directory(dir);
			}
			ImGui::TreePop();
		}
		treeNodeState->Off(NodeState_Selected);
	}
	void ContentsView::OnGUI_FilePanel_TopBar()
	{
		ImGui::BeginChild("FilePanel_TopBar", ImVec2(0, 38), true);


		char search[256] = { 0, };
		f32 scaleSliderWidth = 200.0f;



		ImGui::Button("Filter"); ImGui::SameLine();
		auto btSize = ImGui::GetItemRectSize();


		f32 w = ImGui::GetWindowSize().x - btSize.x - scaleSliderWidth - (ImGui::GetStyle().FramePadding.x * 4);
		ImGui::SetNextItemWidth(w);
		ImGui::InputText("##SearchAsset", search, 256); ImGui::SameLine();
		ImGui::SetNextItemWidth(200.0f);
		ImGui::SliderFloat("##FileCellSizeSlider", &mFileCellScale, 0.5f, 2.0F, "%.3f");
		mFileCellSize = JVector2(mOriginFileCellSize.x * mFileCellScale, mOriginFileCellSize.y * mFileCellScale);

		ImGui::EndChild();
	}
	void ContentsView::OnGUI_FilePanel()
	{
		ImGui::BeginChild("FilePanel", ImVec2(0, 0), true);

		auto panelSize = ImGui::GetWindowSize();
		auto targetNode = GetTargetDirectoryNode();
		if (targetNode != nullptr)
		{
			i32 index = 0;
			i32 dirCnt = targetNode->DirectoryList.size();

			auto startPos = ImGui::GetCursorPos();
			f32 offsetW = mFileCellSize.x + ImGui::GetStyle().FramePadding.x * 2;
			f32 offsetH = mFileCellSize.y + ImGui::GetStyle().FramePadding.y;
			f32 accW    = 0.0f;

			for (auto& dir : targetNode->DirectoryList)
			{
				auto cursorPos = ImGui::GetCursorPos();
				OnGUI_File(JVector2(cursorPos.x, cursorPos.y), "##" + dir->Path, dir->FileName, dir->Path, EAssetFormat::Directory);
				ImVec2 nextCursorPos = cursorPos;
				accW += offsetW;


				if (accW + offsetW < panelSize.x)
				{
					nextCursorPos.x += offsetW;
	
				}
				else
				{
					accW = 0.0f;
					nextCursorPos.x = startPos.x;
					nextCursorPos.y += offsetH;
				}
				ImGui::SetCursorPos(nextCursorPos);
			}
			for (auto& file : targetNode->FileList)
			{
				auto cursorPos = ImGui::GetCursorPos();
				OnGUI_File(JVector2(cursorPos.x, cursorPos.y), "##" + file->Path, file->FileName, file->Path, file->Format);

				ImVec2 nextCursorPos = cursorPos;
				accW += offsetW;
				if (accW + offsetW < panelSize.x)
				{
					nextCursorPos.x += offsetW;
				}
				else
				{
					accW = 0.0f;
					nextCursorPos.x = startPos.x;
					nextCursorPos.y += offsetH;
				}
				ImGui::SetCursorPos(nextCursorPos);
			}
		}
		if (ImGui::IsAnyItemHovered() == false)
		{
			if (ImGui::IsMouseClicked(0))
			{
				SetTargetFile("");
				mSelectedFilesInFilePanel.clear();
				mSelectedFilesInDirPanel.clear();
			}
			mTaskFlags |= Task_OpenContextMenu;
		}

		if (mTaskFlags & Task_OpenContextMenu)
		{
			if (UIManager::GetInstance().ShowContextMenu(GetType(), false) == false)
			{
				mTaskFlags &= ~Task_OpenContextMenu;
			}
		}
		ImGui::EndChild();
	}



	void ContentsView::OnGUI_File(const JVector2& cursorPos, const String& id, const String& FileName, const String& filePath, EAssetFormat format)
	{
		auto treeNodeState = GetFileNodeState(filePath);
		ImVec2 windowPos = ImGui::GetWindowPos();
		f32 padding = 2.0f;
		f32 textHeight = 20.0f;
		f32 textWidth = mFileCellSize.x - padding * 2;

		ImVec2 originCursorPos = ImVec2(cursorPos.x, cursorPos.y);


		bool isItemClick          = false;
		bool isClearSelectedFiles = mIsCtrl == false;
		bool isSelected = treeNodeState->IsState(NodeState_Selected);
		bool isRenmaing = treeNodeState->IsState(NodeState_Renaming);
		isItemClick = ImGui::Selectable(id.c_str(), &isSelected, ImGuiSelectableFlags_AllowItemOverlap, ImVec2(mFileCellSize.x, mFileCellSize.y));
		if (isItemClick && format == EAssetFormat::Material)
		{
			NotifySelectedAssetInEditorEvent e;
			e.SelectedAssetPath = filePath;
			SendEvent(e);
		}



		DragAndDropSource<DDDContentsFile>([&](DDDContentsFile* ddd)
		{
			ddd->FilePath = filePath;
			ImGui::Text(ddd->FilePath.c_str());
		});

		if (ImGui::IsItemHovered() == true)
		{
			ImGui::SetTooltip(FileName.c_str());
			ImGui::BeginTooltip();
			ImGui::EndTooltip();
		}


		if (UIManager::GetInstance().ShowContextMenu(GetType()) == true)
		{
			isItemClick = true;
			if (mIsCtrl == false && isSelected == false)
			{
				isClearSelectedFiles = true;
			}
			else
			{
				isClearSelectedFiles = false;
			}
		}
		if (isItemClick)
		{
			mSelectedFilesInDirPanel.clear();
			if (isClearSelectedFiles) {
				mSelectedFilesInFilePanel.clear();
			}
			mSelectedFilesInFilePanel.insert(filePath);
			SetTargetFile(filePath);
		}
		if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered(ImGuiHoveredFlags_None))
		{
			File_OnDoubleClick(filePath, format);
		}



		ImVec2 iconCursorPos = originCursorPos;
		iconCursorPos.x += padding;
		iconCursorPos.y += padding;
		ImVec2 iconSize = ImVec2(mFileCellSize.x - padding * 2, mFileCellSize.y - padding * 3 - textHeight);


		ImGui::SetCursorPos(iconCursorPos);
		ImTextureID texID = GetIconTextureID(GetIconID(format), filePath);
		if (texID != 0)
		{
			ImGui::Image(texID, iconSize);
		}



		ImVec2 textCursorPos = originCursorPos;
		textCursorPos.x += padding;
		textCursorPos.y = iconCursorPos.y + iconSize.y + padding * 3;


		String text = FileName;
		if (isRenmaing == false)
		{
			ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
			f32 offsetX = (textWidth - textSize.x) * 0.5f;
			if (offsetX >= 0.0f)
			{
				textCursorPos.x += offsetX;
			}
			else
			{
				ImVec2 dotSize = ImGui::CalcTextSize("..");
				textSize = dotSize;
				text = "";



				for (auto c : FileName)
				{
					String tmp; tmp += c;
					ImVec2 charSize = ImGui::CalcTextSize(tmp.c_str());
					textSize.x += charSize.x;
					text += c;

					offsetX = (textWidth - textSize.x) * 0.5f;
					if (offsetX < 0.0f)
					{
						break;
					}
				}
				text += "..";
			}
		}
	
		ImGui::SetCursorPos(textCursorPos);
		if (isRenmaing)
		{
			text.resize(512);
			ImGui::SetNextItemWidth(iconSize.x);
			ImGui::SetKeyboardFocusHere();
			ImGui::InputText((id + "inputText").c_str(), text.data(), 512);
			if (ImGui::IsItemDeactivated() == true)
			{
				auto extension = fs::path(filePath).extension().string();
				auto len = strlen(text.c_str());
				text = text.substr(0, len);
				text += extension;

				Rename(filePath, text);
				treeNodeState->Off(NodeState_Renaming);
			}
		}
		else
		{
			ImGui::Text(text.c_str());
		}

		treeNodeState->Off(NodeState_Selected);
	}

	void ContentsView::UpdateGUI()
	{
		if (ImGui::FileDialog::Instance().IsDone(GetType().GetName()) == true)
		{
			if (ImGui::FileDialog::Instance().HasResult() == true)
			{
				auto results = ImGui::FileDialog::Instance().GetResults();
				for (auto& p : results)
				{
					mImportFileList.push_back(p.string());
				}
				mTaskFlags |= Task_Import;
			}
			ImGui::FileDialog::Instance().Close();
		}

		if (ImGui::IsWindowFocused())
		{
			if (mIsCtrl && ImGui::IsKeyPressed((i32)EKeyCode::C))
			{
				Copy();
			}

			if (mIsCtrl && ImGui::IsKeyPressed((i32)EKeyCode::X))
			{
				Move();
			}

			if (mIsCtrl && ImGui::IsKeyPressed((i32)EKeyCode::V))
			{
				Paste();
			}
		}



	}

	void ContentsView::UpdateNode()
	{

		for (auto& dir : mSelectedFilesInDirPanel)
		{
			auto treeNodeState = GetDirectoryNodeState(dir);
			if (treeNodeState == nullptr) {
				continue;
			}
			treeNodeState->On(NodeState_Selected);
		}
		for (auto& file : mSelectedFilesInFilePanel)
		{
			auto treeNodeState = GetFileNodeState(file);
			if (treeNodeState == nullptr) {
				continue;
			}
			treeNodeState->On(NodeState_Selected);
		}


		auto targetNode = GetFileNodeState(GetTargetFile());
		if (targetNode == nullptr || targetNode->IsState(NodeState_Selected) == false)
		{
			targetNode = GetDirectoryNodeState(GetTargetDirectory());
		}
		if (targetNode != nullptr && targetNode->IsState(NodeState_Selected) && ImGui::IsKeyPressed((i32)EKeyCode::F2))
		{
			targetNode->On(NodeState_Renaming);
		}




		if (mAsyncUpdateDirectory.ThreadTaskHandle && mAsyncUpdateDirectory.ThreadTaskHandle->GetState() != EScheduleState::Compelete)
		{
			return;
		}
		
		if (mAsyncUpdateDirectory.ThreadData != nullptr)
		{
			mRootDirectory = mAsyncUpdateDirectory.ThreadData->AssetDirectory;
			mDirectoryPool = mAsyncUpdateDirectory.ThreadData->DirectoryPool;
			for (auto& path : mAsyncUpdateDirectory.ThreadData->AddDirNodes)
			{
				mDirNodeStateDic.emplace(path, CreateSharedPtr<NodeState>());
			}
			for (auto& path : mAsyncUpdateDirectory.ThreadData->RemoveDirNodes)
			{
				mDirNodeStateDic.erase(path);
			}
			for (auto& path : mAsyncUpdateDirectory.ThreadData->AddFileNodes)
			{
				mFileNodeStateDic.emplace(path, CreateSharedPtr<NodeState>());
			}
			for (auto& path : mAsyncUpdateDirectory.ThreadData->RemoveFileNodes)
			{
				mFileNodeStateDic.erase(path);
			}
		}
		mAsyncUpdateDirectory.ThreadData.reset();
		mAsyncUpdateDirectory.ThreadData = nullptr;



		mAsyncUpdateDirectory.ThreadTaskHandle = Scheduler::GetInstance().ScheduleAsync([&]()
		{
			mAsyncUpdateDirectory.ThreadData = CreateUniquePtr<ThreadData_UpdateDirectory>();
			Async_UpdateAssetDirectory();
		});
	}

	void ContentsView::UpdateTask()
	{
		if (mTaskFlags & Task_ShowProgressBar)
		{
			UIManager::GetInstance().OpenPopupUIView<ProgressBarModalView>(ProgressBarInitData(mProgressBarTitle));
		}
		if(mTaskFlags & Task_CloseProgressBar)
		{
			auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
			if (progressBar->IsOpen())
			{
				progressBar->Close();
			}
			mTaskFlags &= ~Task_CloseProgressBar;
		}
		// 복사 
		if (mAsyncUpdatePaste.ThreadTaskHandle == nullptr && mTaskFlags & Task_Paste)
		{
			mAsyncUpdatePaste.ThreadData = CreateUniquePtr<ThreadData_UpdatePaste>();
			mAsyncUpdatePaste.ThreadData->TaskFlags = mTaskFlags;
			mAsyncUpdatePaste.ThreadData->FileList  = mCopyOrMoveFileList;
			mTaskFlags &= ~Task_Paste;
			mTaskFlags &= ~Task_Copy;
			mTaskFlags &= ~Task_Move;
			mTaskFlags |= Task_ShowProgressBar;
			mCopyOrMoveFileList.clear();
			mAsyncUpdatePaste.ThreadTaskHandle = Scheduler::GetInstance().ScheduleAsync([&]()
			{
				// Copy 나 Move 시작
				std::lock_guard<std::mutex> lock(mUpdateDirectoryMutex);
				i32 taskFlags = mAsyncUpdatePaste.ThreadData->TaskFlags;
				u64 totalCnt = mAsyncUpdatePaste.ThreadData->FileList.size();
				f32 oneUnit  = 1.0f / (f32)totalCnt;
				f32 ratio    = 0.0f;
				auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
			
				// 파일 중복 검사
		
				bool isDuplicate = false;
				HashSet<String> fileNameSet;
				for (auto& file : mAsyncUpdatePaste.ThreadData->FileList)
				{
					auto fileName = fs::path(file).filename().string();
					if (fileNameSet.find(fileName) != fileNameSet.end())
					{
						isDuplicate = true;
						break;
					}
					fileNameSet.insert(fileName);
				}

				if (isDuplicate == false)
				{
					auto to = GetTargetDirectory();
					auto dirIter = fs::directory_iterator(to);
					for (auto& file : dirIter)
					{
						auto fileName = fs::path(file).filename().string();
						if (fileNameSet.find(fileName) != fileNameSet.end())
						{
							isDuplicate = true;
							break;
						}
					}
				}
				if (isDuplicate == false)
				{
					for (auto& file : mAsyncUpdatePaste.ThreadData->FileList)
					{
						auto to = GetTargetDirectory();
						std::error_code err;
						if (taskFlags & Task_Copy)
						{
							auto from = file;
							auto fileName = fs::path(from).filename().string();
							if (fs::is_directory(from))
							{
								to = CombinePath(to, fileName);
								if (fs::create_directory(to) == false)
								{
									JG_CORE_ERROR("Fail Create Directory");
									continue;
								}
							}
							fs::copy(from, to, fs::copy_options::recursive, err);
						}
						else if (taskFlags & Task_Move)
						{
							auto _old = file;
							auto _new = CombinePath(to, fs::path(_old).filename().string());
							fs::rename(_old, _new, err);
						}
						ratio += oneUnit;
						progressBar->Display(file, ratio);
						if (err.message().length() > 0)
						{
							JG_CORE_ERROR("Error : {0}", err.message());
						}
				
					}
				}
				else
				{
					UIManager::GetInstance().OpenPopupUIView<MessageBoxModalView>(MessageBoxInitData("Warning", "Duplicate files exist."));
				}
			});



			return;
		}
		if (mAsyncUpdatePaste.ThreadTaskHandle && mAsyncUpdatePaste.ThreadTaskHandle->GetState() == EScheduleState::Compelete)
		{
			mAsyncUpdatePaste.ThreadTaskHandle = nullptr;
			mTaskFlags &= ~Task_ShowProgressBar;
			mTaskFlags |= Task_CloseProgressBar;
		}


		if (mAsyncUpdateImport.ThreadTaskHandle == nullptr && mTaskFlags & Task_Import)
		{
			mAsyncUpdateImport.ThreadData = CreateUniquePtr<ThreadData_UpdateImport>();
			mAsyncUpdateImport.ThreadData->ImportFileList = std::move(mImportFileList);
			mTaskFlags &= ~Task_Import;
			mTaskFlags |= Task_ShowProgressBar;
			mProgressBarTitle = "Import";


			mAsyncUpdatePaste.ThreadTaskHandle = Scheduler::GetInstance().ScheduleAsync([&]()
			{
				auto progressBar = UIManager::GetInstance().GetPopupUIView<ProgressBarModalView>();
				i32 total = mAsyncUpdateImport.ThreadData->ImportFileList.size();
				f32 oneUnit = 1.0f / (f32)total;
				f32 ratio = 0.0f;
				for (auto importFile : mAsyncUpdateImport.ThreadData->ImportFileList)
				{
					if (Import(importFile) == false)
					{
						JG_CORE_INFO("Fail Import File : {0}", importFile);
					}
					ratio += oneUnit;
					progressBar->Display(importFile, ratio);
				}
			});
		}
		if (mAsyncUpdateImport.ThreadTaskHandle && mAsyncUpdateImport.ThreadTaskHandle->GetState() == EScheduleState::Compelete)
		{
			mAsyncUpdateImport.ThreadTaskHandle = nullptr;
			mTaskFlags &= ~Task_ShowProgressBar;
			mTaskFlags |= Task_CloseProgressBar;
		}
	}

	void ContentsView::File_OnDoubleClick(const String& path, EAssetFormat format)
	{
		switch (format)
		{
		case EAssetFormat::Directory:
			SetTargetDirectory(path);
			break;
		case EAssetFormat::GameWorld:
		{
			RequestLoadGameWorldEvent e;
			e.AssetPath = path;
			SendEvent(e);
		}
			break;
		}


		JG_CORE_INFO("Double Click : {0}", path);
	}
	void ContentsView::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		UIView::MakeJson(jsonData);
	}
	
	void ContentsView::LoadJson(SharedPtr<JsonData> jsonData)
	{
		UIView::LoadJson(jsonData);
	}
	SharedPtr<ContentsView::NodeState> ContentsView::GetDirectoryNodeState(const String& path)
	{
		auto iter = mDirNodeStateDic.find(path);
		if (iter == mDirNodeStateDic.end())
		{
			return nullptr;
		}
		return iter->second;
	}
	SharedPtr<ContentsView::NodeState> ContentsView::GetFileNodeState(const String& path)
	{
		auto iter = mFileNodeStateDic.find(path);
		if (iter == mFileNodeStateDic.end())
		{
			return nullptr;
		}
		return iter->second;
	}

	
	SharedPtr<ContentsView::DirectoryNode> ContentsView::GetTargetDirectoryNode()
	{
		auto iter = mDirectoryPool.find(GetTargetDirectory());
	
		if (iter == mDirectoryPool.end())
		{
			return nullptr;
		}
		return iter->second;
	}


	void ContentsView::SetTargetDirectory(const String& path)
	{
		if (mDirectoryHistory.empty() == false)
		{
			auto prevTargetDir = GetTargetDirectory();
			if (prevTargetDir == path) return;
		}



		i32 historyCnt = (i32)mDirectoryHistory.size();
		// 최신 상태이면 그냥
		bool isLatest = (historyCnt - 1 == mHistroyIndex);
		// 최신 상태가 아니라면
		if (isLatest == false)
		{
			i32 removeIndex = mHistroyIndex + 1;
			mDirectoryHistory.erase(mDirectoryHistory.begin() + removeIndex, mDirectoryHistory.end());
		}
		mDirectoryHistory.push_back(path);
		mHistroyIndex = ((i32)mDirectoryHistory.size() - 1);
	}
	void ContentsView::PrevTargetDirectory()
	{
		if (mHistroyIndex <= 0) return;
		mHistroyIndex--;
	}
	void ContentsView::NextTargetDirectory()
	{
		if (mHistroyIndex >= mDirectoryHistory.size() - 1) return;
		mHistroyIndex++;
	}
	void ContentsView::SetTargetFile(const String& path)
	{
		auto targetNode = GetFileNodeState(mTargetFile);
		if (targetNode)
		{
			targetNode->Off(NodeState_Renaming);
		}
		mTargetFile = path;
	}

	const String& ContentsView::GetTargetDirectory() const
	{
		return mDirectoryHistory[mHistroyIndex];
	}
	const String& ContentsView::GetTargetFile() const
	{
		return mTargetFile;
	}

	bool ContentsView::Import(const String& path)
	{
		auto p = fs::path(path);
		auto extension = p.extension();
	
		if (extension == ".fbx" || extension == ".FBX")
		{
			FBXAssetImportSettings settings;
			settings.Flags = EFBXAssetImportFlags::None;
			settings.AssetPath = p.string();
			settings.OutputPath = GetTargetDirectory();
	
			return AssetImporter::Import(settings) == EAssetImportResult::Success;
		}
		else if (extension == ".jpg" || extension == ".JPG" || extension == ".png" || extension == ".PNG" || extension == ".tga" || extension == ".TGA")
		{
			TextureAssetImportSettings settings;
			settings.Flags = ETextureAssetImportFlags::None;
			settings.AssetPath = p.string();
			settings.OutputPath = GetTargetDirectory();
	
			return AssetImporter::Import(settings) == EAssetImportResult::Success;
		}
		else
		{
			JG_CORE_ERROR("not supported import format : {0}", p.string());
			return false;
		}
	
		return true;
	}

	void ContentsView::CreateFolder(const String& targetDir)
	{
		auto path = CombinePath(targetDir, "NewFolder");
		path = GetUniqueFileName(path);


		std::lock_guard<std::mutex> lock(mUpdateDirectoryMutex);
		fs::create_directory(path);
	}

	void ContentsView::CreateGameWorld(const String& targetDir)
	{
		static GameWorld* gameWorld = nullptr;
		if (gameWorld != nullptr)
		{
			return;
		}
		Scheduler::GetInstance().ScheduleOnceByFrame(0, SchedulePriority::EndSystem, [&]()->EScheduleResult
		{
			gameWorld = GameObjectFactory::GetInstance().CreateObject<GameWorld>();
			gameWorld->AddNode("MainCamera")->AddComponent<EditorCamera>();

			auto json = CreateSharedPtr<Json>();
			auto assetJson = json->CreateJsonData();

			gameWorld->MakeJson(assetJson);
			json->AddMember(JG_ASSET_KEY, assetJson);

			auto path = CombinePath(targetDir, std::string("NewGameWorld") + JG_ASSET_FORMAT);
			path = GetUniqueFileName(path);

			

			std::lock_guard<std::mutex> lock(mUpdateDirectoryMutex);
			AssetDataBase::GetInstance().WriteAsset(path, EAssetFormat::GameWorld, json);

			Scheduler::GetInstance().ScheduleOnceByFrame(10, SchedulePriority::EndSystem, [&]()->EScheduleResult
			{
				GameObjectFactory::GetInstance().DestroyObject(gameWorld);
				gameWorld = nullptr;
				return EScheduleResult::Continue;
			});
	
			return EScheduleResult::Continue;
		});

	}

	void ContentsView::CreateSurfaceMaterial(const String& targetDir)
	{
		auto path = CombinePath(targetDir, std::string("NewMaterial") + JG_ASSET_FORMAT);
		path = GetUniqueFileName(path);

		auto p = fs::path(path);
		auto extension = p.extension().string();
		

		MaterialAssetStock stock;
		stock.Name = ReplaceAll(p.filename().string(), extension, "");
		stock.ShaderTemplate = JG_SHADER_3D_STANDARD_TEMPLATE;
		stock.ShaderScript   = JG_SHADER_STANDARD_SURFACE_SCRIPT;
		auto shader          = ShaderLibrary::GetInstance().GetShader(stock.ShaderTemplate, { stock.ShaderScript });
		auto material = IMaterial::Create("tmp", shader);
		auto propertyList    = material->GetPropertyList();
		auto materialJson = CreateSharedPtr<Json>();
		for (auto& property : propertyList)
		{
			auto type = property.first;
			auto name = property.second;
			auto dataJson = materialJson->CreateJsonData();
			switch (property.first)
			{
			case EShaderDataType::_int:
			{
				i32 value = 0;
				material->GetInt(name, &value);
				dataJson->SetInt32(value);
			}
			break;
			case EShaderDataType::_int2:
			{
				JVector2Int value = 0;
				material->GetInt2(name, &value);
				dataJson->SetVector2Int(value);
			}
			break;
			case EShaderDataType::_int3:
			{
				JVector3Int value = 0;
				material->GetInt3(name, &value);
				dataJson->SetVector3Int(value);
			}
			break;
			case EShaderDataType::_int4:
			{
				JVector4Int value = 0;
				material->GetInt4(name, &value);
				dataJson->SetVector4Int(value);
			}
			break;
			case EShaderDataType::_uint:
			{
				u32 value = 0;
				material->GetUint(name, &value);
				dataJson->SetUint32(value);
			}
			break;
			case EShaderDataType::_uint2:
			{
				JVector2Uint value;
				material->GetUint2(name, &value);
				dataJson->SetVector2Uint(value);
			}
			break;
			case EShaderDataType::_uint3:
			{
				JVector3Uint value;
				material->GetUint3(name, &value);
				dataJson->SetVector3Uint(value);
			}
			break;
			case EShaderDataType::_uint4:
			{
				JVector4Uint value;
				material->GetUint4(name, &value);
				dataJson->SetVector4Uint(value);
			}
			break;
			case EShaderDataType::_float:
			{

				f32 value;
				material->GetFloat(name, &value);
				dataJson->SetFloat(value);
			}
			break;
			case EShaderDataType::_float2:
			{
				JVector2 value;
				material->GetFloat2(name, &value);
				dataJson->SetVector2(value);
			}
			break;
			case EShaderDataType::_float3:
			{
				JVector3 value;
				material->GetFloat3(name, &value);
				dataJson->SetVector3(value);
			}
			break;
			case EShaderDataType::_float4:
			{
				JVector4 value;
				material->GetFloat4(name, &value);
				dataJson->SetVector4(value);
			}
			break;
			case EShaderDataType::texture2D:
			{
				dataJson->SetString(ITexture::NullTexture()->GetName());
			}
			break;
			default:
				break;
			}

			stock.MaterialDatas[name] = std::pair<EShaderDataType, SharedPtr<JsonData>>(type, dataJson);
		}
		auto json = CreateSharedPtr<Json>();
		auto assetJson = json->CreateJsonData();
		stock.MakeJson(assetJson);
		json->AddMember(JG_ASSET_KEY, assetJson);


		std::lock_guard<std::mutex> lock(mUpdateDirectoryMutex);
		AssetDataBase::GetInstance().WriteAsset(path, EAssetFormat::Material, json);
	}

	void ContentsView::Rename(const String& path, const String& name)
	{
		auto p = fs::path(path);
		if (fs::exists(p) == false)
		{
			return;
		}
		auto filename = p.filename();
		auto dir      = p.remove_filename();

		auto dest = CombinePath(dir.string(), filename.string());
		auto src = CombinePath(dir.string(), name);



		std::lock_guard<std::mutex> lock(mUpdateDirectoryMutex);
		std::error_code err;
		fs::rename(dest, src, err );

		if (err.message().length() > 0)
		{
			JG_CORE_ERROR("ReName Error : {0}", err.message());
		}

		auto assetID = AssetDataBase::GetInstance().GetAssetOriginID(dest);
		AssetDataBase::GetInstance().RefreshAssetName(assetID, src);
		//AssetDataBase""
	}

	void ContentsView::Copy()
	{
		// Copy의 경우
		mCopyOrMoveFileList.clear();
		if (mSelectedFilesInDirPanel.empty() == false)
		{
		
			for (auto& dir : mSelectedFilesInDirPanel)
			{
				mCopyOrMoveFileList.insert(dir);
			}
		}
		else if (mSelectedFilesInFilePanel.empty() == false)
		{
			for (auto& file : mSelectedFilesInFilePanel)
			{
				mCopyOrMoveFileList.insert(file);
			}
		}
		mTaskFlags &= ~Task_Move;
		mTaskFlags |= Task_Copy;
	}

	void ContentsView::Paste()
	{
		mTaskFlags |= Task_Paste;
	}

	void ContentsView::Move()
	{
		mCopyOrMoveFileList.clear();
		if (mSelectedFilesInDirPanel.empty() == false)
		{

			for (auto& dir : mSelectedFilesInDirPanel)
			{
				mCopyOrMoveFileList.insert(dir);
			}
		}
		else if (mSelectedFilesInFilePanel.empty() == false)
		{
			for (auto& file : mSelectedFilesInFilePanel)
			{
				mCopyOrMoveFileList.insert(file);
			}
		}
		mTaskFlags &= ~Task_Copy;
		mTaskFlags |= Task_Move;;
	}

	void ContentsView::Delete()
	{
		std::lock_guard<std::mutex> lock(mUpdateDirectoryMutex);
		if (mSelectedFilesInDirPanel.empty() == false)
		{
			for (auto& dir : mSelectedFilesInDirPanel)
			{
				if (fs::exists(dir) == false) continue;
				fs::remove_all(dir);
			}
		}
		else if (mSelectedFilesInFilePanel.empty() == false)
		{
			for (auto& file : mSelectedFilesInFilePanel)
			{
				if (fs::exists(file) == false) continue;
				fs::remove_all(file);
			}
		}
		else
		{
			return;
		}

	}



	void ContentsView::Async_UpdateAssetDirectory()
	{

		auto assetPath = fs::path(Application::GetAssetPath());
		if (fs::exists(assetPath) == false)
		{
			return;
		}

		for (auto& _pair : mFileNodeStateDic)
		{
			mAsyncUpdateDirectory.ThreadData->TreeNodePool.insert(_pair.first);
			mAsyncUpdateDirectory.ThreadData->RemoveFileNodes.insert(_pair.first);
		}
		for (auto& _pair : mDirNodeStateDic)
		{
			mAsyncUpdateDirectory.ThreadData->RemoveDirNodes.insert(_pair.first);
		}
	
	
	
		mAsyncUpdateDirectory.ThreadData->AssetDirectory = Async_CreateDirectoryNode(nullptr, assetPath.string());
		std::lock_guard<std::mutex> lock(mUpdateDirectoryMutex);
		Async_UpdateAssetDirectory(mAsyncUpdateDirectory.ThreadData->AssetDirectory);
	}

	void ContentsView::Async_UpdateAssetDirectory(DirectoryNode* parentDirNode)
	{
		auto curPath = ReplaceAll(fs::path(parentDirNode->Path).string(), "\\", "/");
		auto dirIter = fs::directory_iterator(curPath);
	
	
		if (mAsyncUpdateDirectory.ThreadData->TreeNodePool.find(curPath) == mAsyncUpdateDirectory.ThreadData->TreeNodePool.end())
		{
			mAsyncUpdateDirectory.ThreadData->AddDirNodes.insert(curPath);
			mAsyncUpdateDirectory.ThreadData->AddFileNodes.insert(curPath);
		}
		mAsyncUpdateDirectory.ThreadData->RemoveDirNodes.erase(curPath);
		mAsyncUpdateDirectory.ThreadData->RemoveFileNodes.erase(curPath);
	
	
		for (auto& file : dirIter)
		{
			auto filePath = ReplaceAll(file.path().string(), "\\", "/");
			if (fs::is_directory(filePath) == true)
			{
				auto dirNode = Async_CreateDirectoryNode(parentDirNode, filePath);
				Async_UpdateAssetDirectory(dirNode);
			}
			else
			{
				if (mAsyncUpdateDirectory.ThreadData->TreeNodePool.find(filePath) == mAsyncUpdateDirectory.ThreadData->TreeNodePool.end())
				{
					mAsyncUpdateDirectory.ThreadData->AddFileNodes.insert(filePath);
				}
				mAsyncUpdateDirectory.ThreadData->RemoveFileNodes.erase(filePath);
	
				EAssetFormat format = EAssetFormat::None;
				auto extension = file.path().extension();
	
				if (extension == JG_ASSET_FORMAT)
				{
					format = AssetDataBase::GetInstance().GetAssetFormat(filePath);
				}
				auto fileNode = Async_CreateFileNode(parentDirNode, filePath, format);
			}
	
		}
	}

	ContentsView::DirectoryNode* ContentsView::Async_CreateDirectoryNode(DirectoryNode* ownerDir, const String& path)
	{
		auto dirNode = CreateSharedPtr<DirectoryNode>();
		dirNode->OwnerDirectory = ownerDir;
		dirNode->Path = path;
	
		auto p = fs::path(path);
		dirNode->FileName = p.filename().string();
	
	
		if (ownerDir != nullptr)
		{
			ownerDir->DirectoryList.push_back(dirNode.get());
		}
	
	
		mAsyncUpdateDirectory.ThreadData->DirectoryPool.emplace(path, dirNode);
		return dirNode.get();
	}

	SharedPtr<ContentsView::FileNode> ContentsView::Async_CreateFileNode(DirectoryNode* ownerDir, const String& path, EAssetFormat format)
	{
		auto fileNode = CreateSharedPtr<FileNode>();
		fileNode->OwnerDirectory = ownerDir;
		fileNode->Path = path;
		fileNode->Format = format;
		auto p = fs::path(path);
		fileNode->FileName = ReplaceAll(p.filename().string(), p.extension().string(), "");
		
		ownerDir->FileList.push_back(fileNode);
		return fileNode;
	}

	void ContentsView::LoadIcons()
	{
		mIcons.resize(MAX_ICON);
		mIcons[ICON_NONE]      = UIManager::GetInstance().GetIcon("Icon_Text");
		mIcons[ICON_IMPORT]    = UIManager::GetInstance().GetIcon("Icon_Import");
		mIcons[ICON_DIRECTORY] = UIManager::GetInstance().GetIcon("Icon_Directory");
		mIcons[ICON_MATERIAL]  = UIManager::GetInstance().GetIcon("Icon_Material");
		mIcons[ICON_GAMEWORLD] = UIManager::GetInstance().GetIcon("Icon_GameWorld");
		mIcons[ICON_TEXTURE] = UIManager::GetInstance().GetIcon("Icon_Texture");
		mIcons[ICON_MESH]      = UIManager::GetInstance().GetIcon("Icon_Mesh");
	}
	i32 ContentsView::GetIconID(EAssetFormat format)
	{
		switch (format)
		{
		case JG::EAssetFormat::Directory: return ICON_DIRECTORY;
		case JG::EAssetFormat::Material:  return ICON_MATERIAL;
		case JG::EAssetFormat::Texture:   return ICON_TEXTURE;
		case JG::EAssetFormat::GameWorld: return ICON_GAMEWORLD;
		case JG::EAssetFormat::Mesh:   return ICON_MESH;
		case JG::EAssetFormat::Skeletal:
		default:
			return ICON_NONE;
		}
		return -1;
	}
	ImTextureID ContentsView::GetIconTextureID(i32 iconEnum, const String& filePath)
	{
		if (iconEnum == -1) return 0;
		ImTextureID id = 0;
		
		if (mFileCellScale >= 0.75f)
		{
			switch (iconEnum)
			{
			case ICON_TEXTURE:
			{
				auto iter = mTextureAssetPool.find(filePath);
				if (iter == mTextureAssetPool.end())
				{
					auto asset = AssetDataBase::GetInstance().LoadOriginAsset(filePath);
					if (asset && asset->Is<Asset<ITexture>>())
					{
						mAssetPool.push_back(asset);
						mTextureAssetPool.emplace(filePath, asset->As<Asset<ITexture>>());
					}
				}

				auto tex = mTextureAssetPool[filePath]->Get();

				if (tex && tex->IsValid())
				{
					auto texID = tex->GetTextureID();
					id = (ImTextureID)JGImGui::GetInstance().ConvertImGuiTextureID(texID);
				}
			}
			break;
			}

			if (id == 0)
			{
				if (mIcons[iconEnum] && mIcons[iconEnum]->Get() && mIcons[iconEnum]->Get()->IsValid())
				{
					id = (ImTextureID)JGImGui::GetInstance().ConvertImGuiTextureID(mIcons[iconEnum]->Get()->GetTextureID());
				}
			}
		}
		else
		{
			if (mIcons[iconEnum] && mIcons[iconEnum]->Get() && mIcons[iconEnum]->Get()->IsValid())
			{
				id = (ImTextureID)JGImGui::GetInstance().ConvertImGuiTextureID(mIcons[iconEnum]->Get()->GetTextureID());
			}
		}
		return id;
	}
}

