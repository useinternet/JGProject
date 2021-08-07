#include "pch.h"
#include "ContentsViewModel.h"
#include "Class/FileIO.h"
#include "Class/Asset/AssetImporter.h"

namespace JG
{
	void ContentsViewModel::Initialize()
	{
		mSelectedDir.clear();
		mTargetNode = nullptr;
		mTargetNodeList.clear();

		if (mThreadLoadData == nullptr)
		{
			mThreadLoadData = CreateUniquePtr<ThreadLoadData>();
		}
		mControlUpdateHandle = Scheduler::GetInstance().Schedule(0.0f, 0.5f, -1, SchedulePriority::Default,
			[&]()-> EScheduleResult
		{


			EScheduleState currState = EScheduleState::Wait;
			if (mAsyncUpdateHandle != nullptr)
			{
				currState = mAsyncUpdateHandle->GetState();
			}

			// 해당 업무가 완료가 되었거나 처음 할당되는 업무라면 ㄱㄱ
			if (currState == EScheduleState::Compelete || mAsyncUpdateHandle == nullptr)
			{

				mContentsFileInfoPool = std::move(mThreadLoadData->ContentsFileInfoPool);
				mContentsFileInfoByPath = std::move(mThreadLoadData->ContentsFileInfoByPath);

				// 추가된어진 파일 추가
				for (auto& path : mThreadLoadData->AddedDirectoryList)
				{
					if (mContentsDirectoryNodePool.find(path) == mContentsDirectoryNodePool.end())
					{
						auto& node = mContentsDirectoryNodePool[path];
						node.Path = path;
					}
				}

				// 제거되어진 파일 추가
				for (auto& path : mThreadLoadData->RemovedDirectoryList)
				{
					if (mContentsDirectoryNodePool.find(path) != mContentsDirectoryNodePool.end())
					{
						mContentsDirectoryNodePool.erase(path);
					}
				}

				if (mThreadLoadData->ContentsRoot != nullptr)
				{
					mContentsDirRootNode.Path = mThreadLoadData->ContentsRoot->Path;
				}
				 
				// 쓰레드 데이터 주입
				mThreadLoadData->ContentsDirectoryNodePool = mContentsDirectoryNodePool;

				// 쓰레드 데이터 초기화
				mThreadLoadData->ContentsFileInfoByPath.clear();
				mThreadLoadData->ContentsFileInfoPool.clear();
				mThreadLoadData->ContentsRoot = nullptr;
				mThreadLoadData->DirectoryList.clear();
				mThreadLoadData->AddedDirectoryList.clear();
				mThreadLoadData->RemovedDirectoryList.clear();

				mAsyncUpdateHandle.reset();
				mAsyncUpdateHandle = nullptr;
				mAsyncUpdateHandle = Scheduler::GetInstance().ScheduleAsync([&](void* threadData)
				{
					Async_UpdateContentsDirectory();
				});
			}



			return EScheduleResult::Continue;
		});
		


		Copy	  = CreateUniquePtr<Command<>>();
		Paste	  = CreateUniquePtr<Command<>>();
		Move	  = CreateUniquePtr<Command<>>();
		Delete	  = CreateUniquePtr<Command<>>();

		Create_Folder = CreateUniquePtr<Command<>>();
		Create_Material_Surface = CreateUniquePtr<Command<>>();
	}

	void ContentsViewModel::Destroy()
	{

		Copy->Clear();	    Copy = nullptr;
		Paste->Clear();	    Paste = nullptr;
		Move->Clear();	    Move = nullptr;
		Delete->Clear();	Delete = nullptr;

		Create_Folder->Clear(); Create_Folder = nullptr;
		Create_Material_Surface->Clear(); Create_Material_Surface = nullptr;


		mTargetNode = nullptr;
		mTargetNodeList.clear();


		mControlUpdateHandle->Reset();
		mControlUpdateHandle = nullptr;

		mAsyncUpdateHandle->Reset();
		mAsyncUpdateHandle = nullptr;
	}

	void ContentsViewModel::OnEvent(IEvent& e)
	{


	}

	void ContentsViewModel::ForEeach(const std::function<bool(ContentsDirectoryNode*)>& pushAction, const std::function<void(ContentsDirectoryNode*)>& action, const std::function<void(ContentsDirectoryNode*)>& popAction)
	{
		if (mContentsDirRootNode.Path.empty())
		{
			return;
		}
		ForEeach(&mContentsDirRootNode, pushAction, action, popAction);
	}

	void ContentsViewModel::ForEach(const std::function<void(ContentsFileInfo*)>& guiAction)
	{
		if (mSelectedDir.length() == 0) {
			return;
		}
		auto currSelectedFileInfo = GetContentsFileInfo(mSelectedDir);
		if (currSelectedFileInfo == nullptr)
		{
			return;
		}

		for (auto& dir : currSelectedFileInfo->DirectoryList)
		{
			guiAction(dir);
		}
		for (auto& file : currSelectedFileInfo->FileList)
		{
			guiAction(file);
		}
	}

	ContentsFileInfo* ContentsViewModel::GetContentsFileInfo(const String& path) const
	{
		auto iter = mContentsFileInfoByPath.find(path);
		if (iter == mContentsFileInfoByPath.end())
		{
			return nullptr;
		}
		return iter->second;
	}

	const String& ContentsViewModel::GetSelectedContentsDirectory() const
	{
		return mSelectedDir;
	}
	bool ContentsViewModel::IsSelectedContentsDirectory(ContentsFileInfo* info) const
	{
		if (info == nullptr) return false;

		auto selectedInfo = GetContentsFileInfo(mSelectedDir);
		if (selectedInfo == nullptr) return false;

		return info == selectedInfo;
	}
	void ContentsViewModel::SelectedAssetFile(const String& path)
	{
		if (mSelectedAssetPath == path)
		{
			return;
		}


		mSelectedAssetPath = path;
		NotifySelectedAssetInEditorEvent e;
		e.SelectedAsset = CreateSharedPtr<AssetID>();
		(*e.SelectedAsset) = AssetDataBase::GetInstance().LoadOriginAsset(mSelectedAssetPath);
		SendEvent(e);
	}
	void ContentsViewModel::ForEeach(ContentsDirectoryNode* CurrNode, const std::function<bool(ContentsDirectoryNode*)>& pushAction, const std::function<void(ContentsDirectoryNode*)>& action, const std::function<void(ContentsDirectoryNode*)>& popAction)
	{
		if (CurrNode == nullptr)
		{
			return;
		}

		bool isOpen = pushAction(CurrNode);
		action(CurrNode);

		if (CurrNode->IsSelected == true && CurrNode->IsIgnoreSelect == false)
		{
			mSelectedDir = CurrNode->Path;
		}
		if (CurrNode->IsTarget == true)
		{
			mTargetNode = CurrNode;
			Subscribe(CurrNode);
		}
		else
		{
			if (mTargetNode == CurrNode) mTargetNode = nullptr;
			UnSubscribe(CurrNode);
		}
		// Select File
		if (isOpen == true)
		{
			auto dirFileInfo = GetContentsFileInfo(CurrNode->Path);
			if (dirFileInfo)
			{
				for (auto& fileInfo : dirFileInfo->DirectoryList)
				{
					ForEeach(&mContentsDirectoryNodePool[fileInfo->Path], pushAction, action, popAction);
				}
			}
		}

		if (CurrNode->IsTreePop)
		{
			popAction(CurrNode);
		}
	}
	void ContentsViewModel::Subscribe(ContentsDirectoryNode* node)
	{
		if (node == nullptr)
		{
			return;
		}
		if (mTargetNodeList.find(node) != mTargetNodeList.end())
		{
			return;
		}
		mTargetNodeList.insert(node);
		Create_Folder->Subscribe(node, [&]()
		{
			if (mTargetNode == nullptr)
			{
				return;
			}
			auto fileInfo = GetContentsFileInfo(mTargetNode->Path);
			if (fileInfo == nullptr)
			{
				return;
			}
			if (fs::exists(fileInfo->Path))
			{
				fs::path newFloderPath = CombinePath(fileInfo->Path, "NewFolder");
				fs::path newPath = newFloderPath;
				i32 i = 0;
				while (fs::exists(newPath) == true)
				{
					newPath = newFloderPath.string() + "(" + std::to_string(++i) + ")";
				}

				fs::create_directory(newPath);
			}
		});
		Copy->Subscribe(node, [&]()
		{
			if (mTargetNode == nullptr)
			{
				return;
			}
			auto fileInfo = GetContentsFileInfo(mTargetNode->Path);
			if (fileInfo == nullptr)
			{
				return;
			}

			// TODO Copy

		});
		Paste->Subscribe(node, [&]()
		{
			if (mTargetNode == nullptr)
			{
				return;
			}
			auto fileInfo = GetContentsFileInfo(mTargetNode->Path);
			if (fileInfo == nullptr)
			{
				return;
			}

			// TODO Paste

		});
		Move->Subscribe(node, [&]()
		{
			if (mTargetNode == nullptr)
			{
				return;
			}
			auto fileInfo = GetContentsFileInfo(mTargetNode->Path);
			if (fileInfo == nullptr)
			{
				return;
			}
		});
		Delete->Subscribe(node, [&]()
		{
			for (auto node : mTargetNodeList)
			{
				auto fileInfo = GetContentsFileInfo(node->Path);
				if (fileInfo == nullptr)
				{
					return;
				}
				if (fs::exists(fileInfo->Path))
				{
					if (fileInfo->FileFormat == EAssetFormat::Directory)
					{
						fs::remove_all(fileInfo->Path);
					}
					else
					{
						fs::remove(fileInfo->Path);
					}
				}
			}
		});
		Create_Material_Surface->Subscribe(node, [&]()
		{
			if (mTargetNode == nullptr)
			{
				return;
			}
			auto fileInfo = GetContentsFileInfo(mTargetNode->Path);
			if (fileInfo == nullptr)
			{
				return;
			}
			if (fs::exists(fileInfo->Path))
			{
				CreateSurfaceMaterial(fileInfo->Path);
			}
		});
	}
	void ContentsViewModel::UnSubscribe(ContentsDirectoryNode* node, bool is_remove_hashset)
	{
		if (node == nullptr)
		{
			return;
		}
		if (is_remove_hashset == true)
		{
			mTargetNodeList.erase(node);
		}

		Copy->UnSubscribe(node);
		Paste->UnSubscribe(node);
		Move->UnSubscribe(node);
		Delete->UnSubscribe(node);
		Create_Material_Surface->UnSubscribe(node);
		Create_Folder->UnSubscribe(node);
	}
	void ContentsViewModel::UnSubscribe()
	{
		for (auto& node : mTargetNodeList)
		{
			UnSubscribe(node, false);
		}
		mTargetNodeList.clear();
	}
	void ContentsViewModel::CreateSurfaceMaterial(const String& parentDir)
	{

		MaterialAssetImportSettings  settings;
		settings.OutputPath = parentDir;
		settings.Shader = ShaderScript::Template::Standard3DShader;
		settings.ShaderScript = "Surface/Standard";
		settings.FileName = "DefaultMaterial";

		i32 cnt = 0;
		while (true)
		{
			auto filePath = CombinePath(parentDir, settings.FileName + JG_ASSET_FORMAT);
			if (fs::exists(filePath) == false) 
			{
				break;
			}
			else 
			{
				++cnt;
				settings.FileName += "_" + std::to_string(cnt);
			}
		}

		AssetImporter::Import(settings);
	}
	ContentsFileInfo* ContentsViewModel::Async_CreateContentsFileInfo(const String& name, const String& path, EAssetFormat fileFormat, ContentsFileInfo* ownerDirectory, bool isDirectory)
	{
		auto contentsInfo = CreateUniquePtr<ContentsFileInfo>();
		auto result = contentsInfo.get();

		mThreadLoadData->ContentsFileInfoPool[result] = std::move(contentsInfo);


		result->Name = name;
		result->Path = path;
		result->FileFormat = fileFormat;
		result->OwnerDirectory = ownerDirectory;

		if (result->OwnerDirectory != nullptr)
		{
			if (isDirectory == true)
			{
				result->OwnerDirectory->DirectoryList.push_back(result);
			}
			else
			{
				result->OwnerDirectory->FileList.push_back(result);
			}
		}

		mThreadLoadData->ContentsFileInfoByPath[path] = result;
		return result;
	}


	void ContentsViewModel::Async_UpdateContentsDirectory()
	{
		auto contentsPath = fs::path(Application::GetAssetPath());
		if (fs::exists(contentsPath) == false)
		{
			return;
		}


		mThreadLoadData->ContentsRoot = Async_CreateContentsFileInfo("Asset", contentsPath.string(), EAssetFormat::Directory, nullptr, true);
		Async_UpdateContentsDirectory(mThreadLoadData->ContentsRoot, fs::directory_iterator(contentsPath));

		// 추가된 데이터 제거된 데이터 비교
		for (auto& dirInfo : mThreadLoadData->DirectoryList)
		{
			if (mThreadLoadData->ContentsDirectoryNodePool.find(dirInfo->Path) == mThreadLoadData->ContentsDirectoryNodePool.end())
			{
				mThreadLoadData->AddedDirectoryList.push_back(dirInfo->Path);
			}
		}
		for (auto& pair : mThreadLoadData->ContentsDirectoryNodePool)
		{
			if (mThreadLoadData->ContentsFileInfoByPath.find(pair.first) == mThreadLoadData->ContentsFileInfoByPath.end())
			{
				mThreadLoadData->RemovedDirectoryList.push_back(pair.first);
			}
		}
	}
	void ContentsViewModel::Async_UpdateContentsDirectory(ContentsFileInfo* currentFileInfo, const fs::directory_iterator& currentDirIter)
	{
		for (auto& file : currentDirIter)
		{
			auto filePath = file.path();

			// 디렉토리라면 파고든다.
			if (fs::is_directory(filePath) == true)
			{
				auto ContentsFileInfo = Async_CreateContentsFileInfo(filePath.filename().string(), filePath.string(), EAssetFormat::Directory, currentFileInfo, true);
				mThreadLoadData->DirectoryList.push_back(ContentsFileInfo);
				Async_UpdateContentsDirectory(ContentsFileInfo, fs::directory_iterator(filePath));
			}
			else if (filePath.extension() == JG_ASSET_FORMAT)
			{
				EAssetFormat fileFormat = EAssetFormat::None;
				Async_CreateContentsFileInfo(filePath.filename().string(), filePath.string(), fileFormat, currentFileInfo, false);
			}
			else
			{
				// 다른 에셋이라면
				Async_CreateContentsFileInfo(filePath.filename().string(), filePath.string(), EAssetFormat::None , currentFileInfo, false);
			}

		}
	}
}