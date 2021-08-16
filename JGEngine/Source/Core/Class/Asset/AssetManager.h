#pragma once
#include "Common/Define.h"
#include "Asset.h"
namespace JG
{
	class AssetManager
	{
		struct AssetIDHash
		{
			std::size_t operator()(const AssetID& id) const {
				return id.ID;
			}
		};
		
		std::unordered_map<AssetID, SharedPtr<IAsset>, AssetIDHash> mAssetPool;
	public:
		~AssetManager();
	public:
		template<class T>
		Asset<T>* RequestOriginAsset(const String& path)
		{
			auto asset = AssetDataBase::GetInstance().LoadOriginAsset(path);
			if (asset == nullptr)
			{
				return nullptr;
			}
			if (mAssetPool.find(asset->GetAssetID()) == mAssetPool.end())
			{
				mAssetPool[asset->GetAssetID()] = asset;
			}
			return mAssetPool[asset->GetAssetID()]->As<T>();
		}
		template<class T>
		Asset<T>* RequestRWAsset(const String& path)
		{
			auto originAsset  = AssetDataBase::GetInstance().LoadOriginAsset(path);
			if (originAsset == nullptr)
			{
				return nullptr;
			}

			auto rwAsset = AssetDataBase::GetInstance().LoadReadWriteAsset(originAsset->GetAssetID());
			if (rwAsset == nullptr)
			{
				return nullptr;
			}
			if (mAssetPool.find(rwAsset->GetAssetID()) == mAssetPool.end())
			{
				mAssetPool[rwAsset->GetAssetID()] = rwAsset;
			}
			return mAssetPool[rwAsset->GetAssetID()]->As<T>();
		}
		EAssetFormat GetAssetFormat(const String& path);
	};
}