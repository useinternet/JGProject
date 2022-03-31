#include "pch.h"
#include "AssetManager.h"
#include "Application.h"
#include "Class/FileIO.h"
#include "Graphics/Mesh.h"
namespace JG
{
	AssetManager::~AssetManager()
	{
		if (AssetDataBase::IsValid() == true)
		{
			for (auto& asset : mAssetPool)
			{
				AssetDataBase::GetInstance().UnLoadAsset(asset.first);
			}
		}

		mAssetPool.clear();
	}
	EAssetFormat AssetManager::GetAssetFormat(const String& path)
	{
		return AssetDataBase::GetInstance().GetAssetFormat(path);
	}
}