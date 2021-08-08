#pragma once
#include "Common/Define.h"
#include "Asset.h"
namespace JG
{
	template<class T>
	class AssetHandle
	{
		friend class AssetManager;
	private:
		mutable Asset<T>* mAsset = nullptr;
		AssetID   mID;
	public:
		Asset<T>* GetAsset() const {
			if (mAsset == nullptr && mID.IsValid())
			{
				mAsset = AssetDataBase::GetInstance().GetAsset<T>(mID);
			}
			return mAsset;
		}
		AssetID GetID() const {
			return mID;
		}
		bool IsValid() const  {
			if (GetAsset() == nullptr)
			{
				return false;
			}
			return mAsset->Get() != nullptr;
		}
	};

	// ÂüÁ¶¸¸
	class AssetManager
	{
		struct AssetIDHash
		{
			std::size_t operator()(const AssetID& id) const {
				return id.ID;
			}
		};

		std::unordered_set<AssetID, AssetIDHash> mAssetIDPool;
	public:
		~AssetManager();
	public:
		template<class T>
		SharedPtr<AssetHandle<T>> RequestOriginAsset(const String& path)
		{
			auto handle   = CreateSharedPtr<AssetHandle<T>>();
			handle->mID    = AssetDataBase::GetInstance().LoadOriginAsset(path);
			handle->mAsset = AssetDataBase::GetInstance().GetAsset<T>(handle->mID);

			mAssetIDPool.insert(handle->mID);
			return handle;
		}
		template<class T>
		SharedPtr<AssetHandle<T>> RequestRWAsset(const String& path)
		{
			auto originID = AssetDataBase::GetInstance().LoadOriginAsset(path);
			auto handle = CreateSharedPtr<AssetHandle<T>>();
			handle->mID    = AssetDataBase::GetInstance().LoadReadWriteAsset(originID);
			handle->mAsset = AssetDataBase::GetInstance().GetAsset<T>(handle->mID);

			mAssetIDPool.insert(handle->mID);
			return handle;
		}
		EAssetFormat GetAssetFormat(const String& path);
	};
}