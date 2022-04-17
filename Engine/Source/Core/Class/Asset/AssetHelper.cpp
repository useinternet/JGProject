#include "pch.h"
#include "AssetHelper.h"
#include "Application.h"
#include "Asset.h"


namespace JG
{
	namespace AssetHelper
	{
		bool WriteAsset(EAssetFormat assetFormat, const String& targetPath, const std::function<void(SharedPtr<JsonData>)>& action)
		{
			auto jsonMain = CreateSharedPtr<Json>();
			auto jsonData = jsonMain->CreateJsonData();

			if (action != nullptr)
			{
				action(jsonData);
			}
			jsonMain->AddMember(JG_ASSET_KEY, jsonData);

			fs::path p(targetPath);
			if (p.extension() != JG_ASSET_FORMAT)
			{
				p += JG_ASSET_FORMAT;
			}
			return AssetDataBase::GetInstance().WriteAsset(p.string(), assetFormat, jsonMain);
		}

		bool ReadAsset(EAssetFormat assetFormat, const String& path, const std::function<void(SharedPtr<JsonData>)>& action)
		{
			EAssetFormat    readFormat = EAssetFormat::None;
			SharedPtr<Json> json = nullptr;

			bool result = AssetDataBase::GetInstance().ReadAsset(path, &readFormat, &json);
			if (result == false)
			{
				return false;
			}
			if (readFormat != assetFormat)
			{
				JG_LOG_ERROR("AssetFormat Not Match : {0}", path);
				return false;
			}
			if (json == nullptr)
			{
				JG_LOG_ERROR("Fail Read Asset, Json is nullptr : {0}", path);
				return false;
			}
			SharedPtr<JsonData> jsonData = json->GetMember(JG_ASSET_KEY);
			if (jsonData == nullptr)
			{
				JG_LOG_ERROR("Is not exist JG_ASSET_KEY : {0}", path);
				return false;
			}

			if (action != nullptr)
			{
				action(jsonData);
			}

			return true;
		}

		EAssetFormat GetAssetFormat(const String& path)
		{
			return AssetDataBase::GetInstance().GetAssetFormat(path);
		}

		bool GetResourcePath(const String& path, String* out_absolutePath, String* out_resourcePath)
		{
			
			auto originPath = path;
			auto absolutePath = fs::absolute(path).string();
			auto absoluteAssetPath = fs::absolute(Application::GetAssetPath()).string();
			auto homePath = fs::current_path().string();

			originPath = StringHelper::ReplaceAll(originPath, "\\", "/");
			absolutePath = StringHelper::ReplaceAll(absolutePath, "\\", "/");
			absoluteAssetPath = StringHelper::ReplaceAll(absoluteAssetPath, "\\", "/");
			homePath = StringHelper::ReplaceAll(homePath, "\\", "/");


			String resourcePath;

			// 
			if (absolutePath.find(absoluteAssetPath) == String::npos)
			{
				if (path.find_first_of("Asset/") != String::npos) {

					resourcePath = StringHelper::ReplaceAll(absolutePath, homePath + "/", "");
					absolutePath = StringHelper::ReplaceAll(absolutePath, homePath + "/", "");
					absolutePath = StringHelper::ReplaceAll(absolutePath, "Asset/", "");
					absolutePath = PathHelper::CombinePath(absoluteAssetPath, absolutePath);
					if (out_absolutePath != nullptr)
					{
						*out_absolutePath = absolutePath;
					}
					if (out_resourcePath != nullptr)
					{
						*out_resourcePath = resourcePath;
					}
					return true;
				}
			}
			// in AssetPath
			else
			{
				resourcePath = StringHelper::ReplaceAll(absolutePath, absoluteAssetPath, "Asset");
				if (out_absolutePath != nullptr)
				{
					*out_absolutePath = absolutePath;
				}
				if (out_resourcePath != nullptr)
				{
					*out_resourcePath = resourcePath;
				}
				return true;
			}
			return false;
		}
	}
}