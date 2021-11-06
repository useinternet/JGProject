#include "pch.h"
#include "ProjectSetting.h"
#include "Application.h"

#define CATEGORY_START_SETTING_KEY  "StartSettings"


#define VALUE_START_GAMEWORLD  "StartGameWorld"
namespace JG
{
	ProjectSetting::ProjectSetting()
	{
		Load();
	}
	ProjectSetting::~ProjectSetting()
	{
		Save();
	}
	String ProjectSetting::GetStartGameWorldPath() const
	{
		return mStartGameWorldPath;
	}
	void ProjectSetting::SetStartGameWorldPath(const String& path)
	{
		mStartGameWorldPath = path;
	}
	void ProjectSetting::Save()
	{
		auto json = CreateSharedPtr<Json>();
		Save_StartSetting(json);


		auto path = CombinePath(Application::GetConfigPath(), "ProjectSetting.json");
		Json::Write(path, json);
	}
	void ProjectSetting::Load()
	{
		auto json = CreateSharedPtr<Json>();
		auto path = CombinePath(Application::GetConfigPath(), "ProjectSetting.json");
		bool result = Json::Read(path, json);

		if (result == true)
		{
			Load_StartSetting(json);
		}

	}
	void ProjectSetting::Save_StartSetting(SharedPtr<Json> json)
	{
		auto jsonData = json->CreateJsonData();
		jsonData->AddMember(VALUE_START_GAMEWORLD, mStartGameWorldPath);





		json->AddMember(CATEGORY_START_SETTING_KEY, jsonData);
	}
	void ProjectSetting::Load_StartSetting(SharedPtr<Json> json)
	{
		if (json == nullptr)
		{
			return;
		}

		auto jsonData = json->GetMember(CATEGORY_START_SETTING_KEY);
		if (jsonData == nullptr)
		{
			return;
		}


		auto val = jsonData->GetMember(VALUE_START_GAMEWORLD);
		if (val)
		{
			mStartGameWorldPath = val->GetString();
		}



	}
}