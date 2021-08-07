#include "pch.h"
#include "AssetManager.h"
#include "Application.h"
#include "Class/FileIO.h"
#include "Graphics/Mesh.h"
namespace JG
{
	AssetManager::~AssetManager()
	{
		for (auto& id : mAssetIDPool)
		{
			AssetDataBase::GetInstance().UnLoadAsset(id);
		}
	}
	EAssetFormat AssetManager::GetAssetFormat(const String& path)
	{
		return AssetDataBase::GetInstance().GetAssetFormat(path);
	}
}