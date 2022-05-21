#pragma once


#include "UI/UIView.h"

namespace JG
{
	class ContentsView : public UIView
	{
		JGCLASS
		class Directory {};
	private:
		enum
		{
			ICON_NONE,
			ICON_DIRECTORY,
			ICON_MATERIAL,
			ICON_GAMEWORLD,
			ICON_TEXTURE,
			ICON_MESH,
			ICON_IMPORT,
			ICON_PREFAB,
			MAX_ICON
		};
		enum
		{
			Task_Copy = 0x00001,
			Task_Move = 0x00002,
			Task_Paste = 0x00004,
			Task_Import = 0x00008,
			Task_OpenContextMenu = 0x00010,
			Task_ColumnInit = 0x00020,
			Task_ShowProgressBar = 0x00040,
			Task_CloseProgressBar = 0x00080,
		};
		enum
		{
			NodeState_Selected = 0x0001,
			NodeState_Renaming = 0x0002,
		};
		struct DirectoryNode;
		struct FileNode
		{
			String       Path;
			String       FileName;
			EAssetFormat Format;
			DirectoryNode* OwnerDirectory = nullptr;
		};
		struct DirectoryNode
		{
			String Path;
			String FileName;
			DirectoryNode* OwnerDirectory = nullptr;
			List<DirectoryNode*> DirectoryList;
			List<SharedPtr<FileNode>>      FileList;
		};
		struct NodeState
		{
			i32 State = 0;
		public:
			bool IsState(i32 state) {
				return State & state;
			}
			void On(i32 state) {
				State |= state;
			}
			void Off(i32 state) {
				State &= ~state;
			}
		};


	private:
		List<Asset<ITexture>*> mIcons;
	private:
		bool mIsCtrl = false;

		f32      mFileCellScale = 1.0f;
		JVector2 mFileCellSize  = JVector2(84, 106);
		JVector2 mOriginFileCellSize = JVector2(84, 106);



		DirectoryNode* mRootDirectory = nullptr;


		Dictionary<String, SharedPtr<DirectoryNode>> mDirectoryPool;
		Dictionary<String, SharedPtr<NodeState>> mDirNodeStateDic;
		Dictionary<String, SharedPtr<NodeState>> mFileNodeStateDic;
		List<SharedPtr<IAsset>> mAssetPool;
		Dictionary<String, Asset<ITexture>*>     mTextureAssetPool;

		HashSet<String> mSelectedFilesInDirPanel;
		HashSet<String> mSelectedFilesInFilePanel;
		HashSet<String> mCopyOrMoveFileList;
		List<String>    mImportFileList;
		List<String>    mDirectoryHistory;
		i32 mHistroyIndex = -1;
		i32 mTaskFlags = 0;

		String       mTargetFile;
		String mProgressBarTitle;
	public:
		ContentsView();
		virtual ~ContentsView() = default;
	protected:
		virtual void Load() override;
		virtual void Initialize() override;
		virtual void OnGUI() override;
		virtual void PreOnGUI() override;
		virtual void LateOnGUI() override;
		virtual void Destroy() override;
	private:
		void OnGUI_Top();
		void OnGUI_Bottom();
		void OnGUI_DirectoryPanel();
		void OnGUI_Directory(DirectoryNode* dirNode);
		void OnGUI_FilePanel_TopBar();
		void OnGUI_FilePanel();
		void OnGUI_File(const JVector2& curCursorPos, const String& id, const String& FileName, const String& filePath, EAssetFormat format);
	private:
		void UpdateGUI();
		void UpdateNode();
		void UpdateTask();
		void File_OnDoubleClick(const String& path, EAssetFormat format);
	private:
		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	private:

		SharedPtr<NodeState> GetDirectoryNodeState(const String& path);
		SharedPtr<NodeState> GetFileNodeState(const String& path);
		SharedPtr<DirectoryNode> ContentsView::GetTargetDirectoryNode();

		void SetTargetDirectory(const String& path);
		void PrevTargetDirectory();
		void NextTargetDirectory();
		void SetTargetFile(const String& path);

	public:
		const String& GetTargetDirectory() const;
		const String& GetTargetFile() const;
	private:
		bool Import(const String& path);
		void CreateFolder(const String& targetDir);
		void CreateGameWorld(const String& targetDir);
		void CreateSurfaceMaterial(const String& targetDir);
		void CreateAnimation(const String& targetDir);
		void CreateAnimationBlendSpace1D(const String& targetDir);
		void CreateAnimationBlendSpace(const String& targetDir);
		void Rename(const String& path, const String& name);
		void CopyToPath();
		void Copy();
		void Paste();
		void Move();
		void Delete();
	private:
		struct ThreadData_UpdateDirectory
		{
			DirectoryNode* AssetDirectory;
			Dictionary<String, SharedPtr<DirectoryNode>> DirectoryPool;
			HashSet<String> TreeNodePool;


			HashSet<String> AddDirNodes;
			HashSet<String> RemoveDirNodes;
	
			HashSet<String> AddFileNodes;
			HashSet<String> RemoveFileNodes;
		
		};
		struct Async_UpdateDirectory
		{
			UniquePtr<ThreadData_UpdateDirectory> ThreadData = nullptr;
			SharedPtr<ScheduleHandle> ThreadTaskHandle = nullptr;
		};

		Async_UpdateDirectory mAsyncUpdateDirectory;
		std::mutex mUpdateDirectoryMutex;


		void Async_UpdateAssetDirectory();
		void Async_UpdateAssetDirectory(DirectoryNode* parentDirNode);
		DirectoryNode* Async_CreateDirectoryNode(DirectoryNode* ownerDir, const String& path);
		SharedPtr<FileNode> Async_CreateFileNode(DirectoryNode* ownerDir, const String& path, EAssetFormat format);
	private:
		struct ThreadData_UpdateImport
		{
			List<String> ImportFileList;
		};  
		struct Async_UpdateImport
		{
			UniquePtr<ThreadData_UpdateImport> ThreadData = nullptr;
			SharedPtr<ScheduleHandle> ThreadTaskHandle = nullptr;
		};
		Async_UpdateImport mAsyncUpdateImport;
	private:
		struct ThreadData_UpdatePaste
		{
			i32 TaskFlags = 0;
			HashSet<String> FileList;
		};
		struct Async_UpdatePaste
		{
			UniquePtr<ThreadData_UpdatePaste>    ThreadData;
			SharedPtr<ScheduleHandle> ThreadTaskHandle = nullptr;
		};
		Async_UpdatePaste mAsyncUpdatePaste;
	private:
		void LoadIcons();
		i32  GetIconID(EAssetFormat format);
		ImTextureID GetIconTextureID(i32 iconEnum, const String& filePath);
	};
}