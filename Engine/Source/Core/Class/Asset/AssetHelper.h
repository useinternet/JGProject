#pragma once
#include "JGCore.h"


namespace JG
{
	enum class EAssetFormat;
	namespace AssetHelper
	{
		bool WriteAsset(EAssetFormat assetFormat, const String& path, const std::function<void(SharedPtr<JsonData>)>& action);
		bool ReadAsset(EAssetFormat assetFormat, const String& path, const std::function<void(SharedPtr<JsonData>)>& action);
		

		EAssetFormat GetAssetFormat(const String& path);
		bool GetResourcePath(const String& path, String* out_absolutePath, String* out_resourcePath);

	}

}