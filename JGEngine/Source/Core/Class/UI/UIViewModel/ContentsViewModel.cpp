#include "pch.h"
#include "ContentsViewModel.h"
#include "Class/FileIO.h"
#include "Class/Asset/AssetImporter.h"

namespace JG
{
	void ContentsViewModel::Initialize()
	{
		mSelectedDir.clear();
		mSelectedFileListInSelectedDirectory.clear();
		mSelectedFileList.clear();
		mTargetNode = nullptr;


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

				mContentsFileInfoPool   = std::move(mThreadLoadData->ContentsFileInfoPool);
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
		Import    = CreateUniquePtr<Command<String>>();
		Create_Folder = CreateUniquePtr<Command<>>();
		Create_Material_Surface = CreateUniquePtr<Command<>>();
		Subscribe();
	}

	void ContentsViewModel::Destroy()
	{
		UnSubscribe();
		Import->Clear();    Import = nullptr;
		Copy->Clear();	    Copy = nullptr;
		Paste->Clear();	    Paste = nullptr;
		Move->Clear();	    Move = nullptr;
		Delete->Clear();	Delete = nullptr;

		Create_Folder->Clear(); Create_Folder = nullptr;
		Create_Material_Surface->Clear(); Create_Material_Surface = nullptr;


		mTargetNode = nullptr;
		mSelectedFileList.clear();


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

	void ContentsViewModel::ForEach(const std::function<void(ContentsFileNode*)>& guiAction)
	{
		if (GetSelectedContentsDirectory().length() == 0) {
			return;
		}




		// 현재 디렉토리 위치에 있는 파일을 긁어온다.
		auto currSelectedFileInfo = GetContentsFileInfo(GetSelectedContentsDirectory());
		if (currSelectedFileInfo == nullptr)
		{
			return;
		}

		for (auto& fileNode : mSelectedFileListInSelectedDirectory)
		{
			fileNode.FileInfo = GetContentsFileInfo(fileNode.Path);
			if (fileNode.FileInfo == nullptr) continue;
			guiAction(&fileNode);
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

	void ContentsViewModel::SetSelectedContentsDirectory(const String& dir)
	{
		auto fileInfo = GetContentsFileInfo(dir);
		if (fileInfo == nullptr) return;
		if (mSelectedDir != dir) 
		{
			mSelectedFileListInSelectedDirectory.clear();
			
			ContentsFileNode node;
			for (auto& dir : fileInfo->DirectoryList)
			{
				node.Path = dir->Path;
				mSelectedFileListInSelectedDirectory.push_back(node);
			}
			for (auto& file : fileInfo->FileList)
			{
				node.Path = file->Path;
				mSelectedFileListInSelectedDirectory.push_back(node);
			}
		}
		mSelectedDir = dir;
	}

	const String& ContentsViewModel::GetSelectedContentsDirectory() const
	{
		return mSelectedDir;
	}
	void ContentsViewModel::ReleaseSelectedContentsFiles()
	{
		for (auto& directoryNode : mContentsDirectoryNodePool)
		{
			directoryNode.second.IsSelected = false;
		}
	}
	bool ContentsViewModel::IsSelectedContentsFile(ContentsFileInfo* info) const
	{
		if (info == nullptr) return false;

		return mSelectedFileList.find(info) != mSelectedFileList.end();
	}
	void ContentsViewModel::SelectedAssetFile(const String& path)
	{
		if (mSelectedAssetPath == path)
		{
			return;
		}


		mSelectedAssetPath = path;
		NotifySelectedAssetInEditorEvent e;
		e.SelectedAsset = AssetDataBase::GetInstance().LoadOriginAsset(mSelectedAssetPath);
		SendEvent(e);
	}
	void ContentsViewModel::ForEeach(ContentsDirectoryNode* CurrNode, const std::function<bool(ContentsDirectoryNode*)>& pushAction, const std::function<void(ContentsDirectoryNode*)>& action, const std::function<void(ContentsDirectoryNode*)>& popAction)
	{
		if (CurrNode == nullptr)
		{
			return;
		}
		auto fileInfo = GetContentsFileInfo(CurrNode->Path);
		bool isOpen = pushAction(CurrNode);
		action(CurrNode);


	
		if (CurrNode->IsSelected == true && CurrNode->IsIgnoreSelect == false)
		{
			SetSelectedContentsDirectory(CurrNode->Path);
			mSelectedFileList.insert(fileInfo);
		}
		else
		{
			mSelectedFileList.erase(fileInfo);
		}
		if (CurrNode->IsTarget == true)
		{
			mTargetNode = CurrNode;
		}
		else
		{
			if (mTargetNode == CurrNode) mTargetNode = nullptr;
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
	void ContentsViewModel::Subscribe()
	{
		Create_Folder->Subscribe(this, [&]()
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
		Copy->Subscribe(this, [&]()
		{
			mCopyFileList.clear();
			if (mSelectedFileList.empty() == true)
			{
				return;
			}


			for (auto& fileInfo : mSelectedFileList)
			{
				mCopyFileList.insert(fileInfo->Path);
			}
		});
		Paste->Subscribe(this, [&]()
		{
			if (mTargetNode == nullptr)
			{
				return;
			}
			if (mCopyFileList.empty() == true)
			{
				return;
			}

			auto fileInfo = GetContentsFileInfo(mTargetNode->Path);
			if (fileInfo == nullptr)
			{
				return;
			}

			for (auto& path : mCopyFileList)
			{
				fs::path p = path;
				if (fs::exists(p) == false) continue;

				std::error_code err;
				fs::copy(mTargetNode->Path, p, fs::copy_options::recursive, err);
				if (err.message().length() > 0)
				{
					JG_CORE_ERROR("Paste Error : {0}", err.message());
				}
			}
			mCopyFileList.clear();
		});
		Move->Subscribe(this, [&]()
		{
			if (mTargetNode == nullptr)
			{
				return;
			}
			if (mCopyFileList.empty() == true)
			{
				return;
			}
			auto fileInfo = GetContentsFileInfo(mTargetNode->Path);
			if (fileInfo == nullptr)
			{
				return;
			}

			for (auto& path : mCopyFileList)
			{
				fs::path p = path;
				if (fs::exists(p) == false) continue;
				std::error_code err;

				auto new_p = CombinePath(mTargetNode->Path, p.filename().string());
				fs::rename(p, new_p, err);
				if (err.message().length() > 0)
				{
					JG_CORE_ERROR("Move Error : {0}", err.message());
				}
			}


		});

		Import->Subscribe(this, [&](String path)
		{
			String destPath;
			if (mTargetNode == nullptr)
			{
				destPath = Application::GetAssetPath();
			}
			else {
				destPath = mTargetNode->Path;
			}

			fs::path srcPath = path;
			auto srcExtension = srcPath.extension().string();
			std::transform(srcExtension.begin(), srcExtension.end(), srcExtension.begin(), ::tolower);


			if (fs::exists(srcPath) == false)
			{
				JG_CORE_ERROR("Not Exist Import File : {0}", srcPath.string());
				return;
			}
			// Mesh
			EAssetImportResult result;
			if (srcExtension == ".fbx")
			{
				FBXAssetImportSettings settings;
				settings.AssetPath  = srcPath.string();
				settings.OutputPath = destPath;
				settings.Flags = EFBXAssetImportFlags::None;
				result = AssetImporter::Import(settings);
			}
			// Texture
			else if (srcExtension == ".png" || srcExtension == ".jpg" || srcExtension == ".tga")
			{
				TextureAssetImportSettings settings;
				settings.AssetPath = srcPath.string();
				settings.OutputPath = destPath;
				settings.Flags = ETextureAssetImportFlags::None;
				result = AssetImporter::Import(settings);
			}
			else
			{
				JG_CORE_ERROR("Not Supported Import File : {0}", srcPath.string());
			}

			if (result != EAssetImportResult::Success)
			{
				JG_CORE_ERROR("Failed File Import : {0}", srcPath.string());
			}
		});

		Delete->Subscribe(this, [&]()
		{
			if (mSelectedFileList.empty() == true)
			{
				return;
			}
			for (auto& fileInfo : mSelectedFileList)
			{
				if (fs::exists(fileInfo->Path))
				{
					std::error_code err;
					if (fileInfo->FileFormat == EAssetFormat::Directory)
					{
						fs::remove_all(fileInfo->Path, err);
					}
					else
					{
						fs::remove(fileInfo->Path, err);
					}
					if (err.message().length() > 0)
					{
						JG_CORE_ERROR("Delete Error : {0}", err.message());
					}
				}

				mCopyFileList.insert(fileInfo->Path);
			}

			mSelectedFileList.clear();
		});
		Create_Material_Surface->Subscribe(this, [&]()
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
	void ContentsViewModel::UnSubscribe()
	{

		Copy->UnSubscribe(this);
		Paste->UnSubscribe(this);
		Move->UnSubscribe(this);
		Delete->UnSubscribe(this);
		Import->UnSubscribe(this);
		Create_Material_Surface->UnSubscribe(this);
		Create_Folder->UnSubscribe(this);
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
				EAssetFormat fileFormat = AssetDataBase::GetInstance().GetAssetFormat(filePath.string(), false);
				Async_CreateContentsFileInfo(filePath.filename().string(), filePath.string(), fileFormat, currentFileInfo, false);
			}
			else
			{
				// 다른 에셋이라면
				Async_CreateContentsFileInfo(filePath.filename().string(), filePath.string(), EAssetFormat::None, currentFileInfo, false);
			}

		}
	}
}