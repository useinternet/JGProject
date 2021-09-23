#pragma once
#include "Class/UI/UIViewModel.h"
#include "Class/Asset/Asset.h"



namespace JG
{
	// History ����
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

	class ContentsFileNode
	{
	public:
		ContentsFileInfo* FileInfo = nullptr;
		String Path;
		u64  UserFlags = 0;
		bool IsSelected = false;
		bool IsTarget   = false;
	};

	class ContentsDirectoryNode
	{
	public:
		String Path;
		u64  UserFlags  = 0;
		bool IsTreePop  = true;
		bool IsIgnoreSelect = false;
		bool IsSelected     = false;
		bool IsTarget = false;
	};

	

	class ContentsViewModel : public UIViewModel
	{
	private:
		struct ThreadLoadData
		{
			// �Ѱ��־���� ������
			Dictionary<String, ContentsDirectoryNode> ContentsDirectoryNodePool;

			// �ֹ߼� ������
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

		// 1�ʿ� �ѹ� �� �˻�
		Dictionary<ContentsFileInfo*, UniquePtr<ContentsFileInfo>> mContentsFileInfoPool;
		Dictionary<String, ContentsFileInfo*>                      mContentsFileInfoByPath;

		//
		Dictionary<String, ContentsDirectoryNode> mContentsDirectoryNodePool;
		ContentsDirectoryNode mContentsDirRootNode;

		Queue<String> mHistoryQueue;
		List<ContentsFileNode> mSelectedFileListInSelectedDirectory;
		String mSelectedDir;




		HashSet<String> mSelectedFileList;
		HashSet<String> mCopyFileList;



		ContentsDirectoryNode* mTargetNode = nullptr;
		String mSelectedAssetPath;
	public:

		UniquePtr<Command<>> Copy;
		UniquePtr<Command<>> Paste;
		UniquePtr<Command<>> Move;
		UniquePtr<Command<>> Delete;

		UniquePtr<Command<>> Create_Folder;
		UniquePtr<Command<>> Create_Material_Surface;


		UniquePtr<Command<String>> Import;
	protected:
		virtual void Initialize() override;
		virtual void Destroy() override;
		virtual void OnEvent(IEvent& e) override;
	public:
		void ForEeach(
			const std::function<bool(ContentsDirectoryNode*)>& pushAction,
			const std::function<void(ContentsDirectoryNode*)>& action,
			const std::function<void(ContentsDirectoryNode*)>& popAction);
		void ForEach(const std::function<void(ContentsFileNode*)>& guiAction);


		ContentsFileInfo* GetContentsFileInfo(const String& path) const;


		void SetSelectedContentsDirectory(const String& dir);
		const String& GetSelectedContentsDirectory() const;



		void ReleaseSelectedContentsFiles();
		bool IsSelectedContentsFile(ContentsFileInfo* info) const;
		void SelectedAssetFile(const String& path);
	




	private:
		void ForEeach(
			ContentsDirectoryNode* CurrNode,
			const std::function<bool(ContentsDirectoryNode*)>& pushAction,
			const std::function<void(ContentsDirectoryNode*)>& action,
			const std::function<void(ContentsDirectoryNode*)>& popAction);
		void Subscribe();
		void UnSubscribe();

		// ���� �Լ�
	private:
		void CreateSurfaceMaterial(const String& parentDir);



		// �񵿱� �Լ�
	private:
		ContentsFileInfo* Async_CreateContentsFileInfo(
			const String& name, const String& path, EAssetFormat fileFormat, ContentsFileInfo* ownerDirectory, bool isDirectory);
		void Async_UpdateContentsDirectory();
		void Async_UpdateContentsDirectory(ContentsFileInfo* currentFileInfo, const fs::directory_iterator& currentDirIter);
	};
}