#pragma once
#include "Class/UI/UIViewModel.h"
#include "Class/Asset/Asset.h"



namespace JG
{
	// History 구현
	// 
	class ContentsFileInfo
	{
	public:
		String Name;
		String Path;
		EAssetFormat FileFormat;

		ContentsFileInfo* OwnerDirectory = nullptr;
		List<ContentsFileInfo*> DirectoryList;
		List<ContentsFileInfo*> FileList;
	};

	struct ContentsDirectoryNode : public ISubscriber
	{
	public:
		String Path;
		u64  UserFlags  = 0;
		bool IsTreePop  = true;
		bool IsIgnoreSelect = false;
		bool IsSelected     = false;
		bool IsTarget = false;
	public:
		virtual ~ContentsDirectoryNode() = default;
	};

	

	class ContentsViewModel : public UIViewModel
	{
	private:
		struct ThreadLoadData
		{
			// 넘겨주어야할 데이터
			Dictionary<String, ContentsDirectoryNode> ContentsDirectoryNodePool;

			// 휘발성 데이터
			Dictionary<ContentsFileInfo*, UniquePtr<ContentsFileInfo>> ContentsFileInfoPool;
			Dictionary<String, ContentsFileInfo*> ContentsFileInfoByPath;
			List<ContentsFileInfo*> DirectoryList;
			List<String> AddedDirectoryList;
			List<String> RemovedDirectoryList;
	

			ContentsFileInfo* ContentsRoot = nullptr;
		};
	private:
		UniquePtr<ThreadLoadData> mThreadLoadData = nullptr;

		//
		SharedPtr<ScheduleHandle> mControlUpdateHandle;
		SharedPtr<ScheduleHandle> mAsyncUpdateHandle;

		// 1초에 한번 식 검사
		Dictionary<ContentsFileInfo*, UniquePtr<ContentsFileInfo>> mContentsFileInfoPool;
		Dictionary<String, ContentsFileInfo*>                      mContentsFileInfoByPath;

		//
		Dictionary<String, ContentsDirectoryNode> mContentsDirectoryNodePool;
		ContentsDirectoryNode mContentsDirRootNode;

		Queue<String> mHistoryQueue;
		String mSelectedDir;

		HashSet<ContentsDirectoryNode*> mTargetNodeList;
		ContentsDirectoryNode* mTargetNode = nullptr;

		bool mIsCtrl = false;


		String mSelectedAssetPath;
	public:

		UniquePtr<Command<>> Copy;
		UniquePtr<Command<>> Paste;
		UniquePtr<Command<>> Move;
		UniquePtr<Command<>> Delete;

		UniquePtr<Command<>> Create_Folder;
		UniquePtr<Command<>> Create_Material_Surface;
	protected:
		virtual void Initialize() override;
		virtual void Destroy() override;
		virtual void OnEvent(IEvent& e) override;
	public:
		void ForEeach(
			const std::function<bool(ContentsDirectoryNode*)>& pushAction,
			const std::function<void(ContentsDirectoryNode*)>& action,
			const std::function<void(ContentsDirectoryNode*)>& popAction);
		void ForEach(const std::function<void(ContentsFileInfo*)>& guiAction);


		ContentsFileInfo* GetContentsFileInfo(const String& path) const;
		const String& GetSelectedContentsDirectory() const;

		bool IsSelectedContentsDirectory(ContentsFileInfo* info) const;
		void SelectedAssetFile(const String& path);
	private:
		void ForEeach(
			ContentsDirectoryNode* CurrNode,
			const std::function<bool(ContentsDirectoryNode*)>& pushAction,
			const std::function<void(ContentsDirectoryNode*)>& action,
			const std::function<void(ContentsDirectoryNode*)>& popAction);
		void Subscribe(ContentsDirectoryNode* node);
		void UnSubscribe(ContentsDirectoryNode* node, bool is_remove_hashset = true);
		void UnSubscribe();
		// 생성 함수
	private:
		void CreateSurfaceMaterial(const String& parentDir);

		// 비동기 함수
	private:
		ContentsFileInfo* Async_CreateContentsFileInfo(
			const String& name, const String& path, EAssetFormat fileFormat, ContentsFileInfo* ownerDirectory, bool isDirectory);
		void Async_UpdateContentsDirectory();
		void Async_UpdateContentsDirectory(ContentsFileInfo* currentFileInfo, const fs::directory_iterator& currentDirIter);
	};
}