#pragma once


#include "UI/UIView.h"


namespace JG
{
	
	class AssetFinderInitData
	{
	public:
		EAssetFormat AssetFormat;

		AssetFinderInitData() = default;
		AssetFinderInitData(EAssetFormat format) : AssetFormat(format) {}
	};

	class AssetFinderContextView : public ContextUIView<AssetFinderInitData>
	{
		JGCLASS
	private:
		struct PathData
		{
			String Name;
			String Path;

			PathData(const fs::path& p);
		};
		static const u32 mMaxReceivePathCount = 10;

		List<PathData> mFilteredAssetList;
		EAssetFormat   mFilterFormat;

		Queue<PathData>  mFilteredAssetQueue_Thread;
		std::atomic_bool mLock;
		SharedPtr<ScheduleHandle> mFindAssetThreadSH;

		String mSelectedAssetPath;
	public:
		virtual ~AssetFinderContextView() = default;
		const String& GetSelectedAssetPath() const;
	protected:
		virtual void Initialize(const AssetFinderInitData& data) override;
		virtual void Destroy() override;
		virtual bool OnGUI()   override;


	private:
		void Lock()   { mLock = true; }
		void UnLock() { mLock = false; }
		bool IsLock() { return mLock; }
		void FindAsset();
		void FindAsset_Thread();
	};
}
