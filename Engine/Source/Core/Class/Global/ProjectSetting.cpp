#include "pch.h"
#include "ProjectSetting.h"
#include "InputManager.h"
#include "Application.h"

#define CATEGORY_START_SETTING_KEY  "StartSettings"
#define CATEGORY_INPUT_SETTING_KEY  "InputSettings"

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
		Save_InputSetting(json);

		auto path = PathExtend::CombinePath(Application::GetConfigPath(), "ProjectSetting.json");
		Json::Write(path, json);
	}
	void ProjectSetting::Load()
	{
		auto json = CreateSharedPtr<Json>();
		auto path = PathExtend::CombinePath(Application::GetConfigPath(), "ProjectSetting.json");
		bool result = Json::Read(path, json);

		if (result == true)
		{
			Load_StartSetting(json);
			Load_InputSetting(json);
		}

	}
	void ProjectSetting::Save_StartSetting(SharedPtr<Json> json)
	{
		auto jsonData = json->CreateJsonData();
		jsonData->AddMember(VALUE_START_GAMEWORLD, mStartGameWorldPath);





		json->AddMember(CATEGORY_START_SETTING_KEY, jsonData);
	}
	void ProjectSetting::Save_InputSetting(SharedPtr<Json> json)
	{
		auto inputSettingJson = json->CreateJsonData();
		auto axisMappingListJson = inputSettingJson->CreateJsonData();
		InputManager::GetInstance().ForEach([&](AxisMappingData* data)
		{
			auto mappingJson = axisMappingListJson->CreateJsonData();
			mappingJson->AddMember("Name", data->Name);

			auto keyDataListJson = mappingJson->CreateJsonData();
			for (auto keyData : data->KeyList)
			{
				auto keyDataJson = keyDataListJson->CreateJsonData();
				keyDataJson->AddMember("KeyCode", KeyCodeToString(keyData.Code));
				keyDataJson->AddMember("Scale", keyData.Scale);
				keyDataListJson->AddMember(keyDataJson);
			}
			mappingJson->AddMember("KeyDataList", keyDataListJson);

			axisMappingListJson->AddMember(mappingJson);
		});
		inputSettingJson->AddMember("AxisMappingDataList", axisMappingListJson);



		auto actionMappingListJson = inputSettingJson->CreateJsonData();
		InputManager::GetInstance().ForEach([&](ActionMappingData* data)
		{
			auto mappingJson = axisMappingListJson->CreateJsonData();
			mappingJson->AddMember("Name", data->Name);

			auto keyDataListJson = mappingJson->CreateJsonData();
			for (auto keyData : data->KeyList)
			{
				auto keyDataJson = keyDataListJson->CreateJsonData();
				keyDataJson->AddMember("KeyCode", KeyCodeToString(keyData.Code));
				keyDataJson->AddMember("IsShift", keyData.IsShift);
				keyDataJson->AddMember("IsCtrl",  keyData.IsCtrl);
				keyDataJson->AddMember("IsAlt",  keyData.IsAlt);
				keyDataListJson->AddMember(keyDataJson);
			}
			mappingJson->AddMember("KeyDataList", keyDataListJson);

			actionMappingListJson->AddMember(mappingJson);
		});
		inputSettingJson->AddMember("ActionMappingDataList", actionMappingListJson);

		json->AddMember(CATEGORY_INPUT_SETTING_KEY, inputSettingJson);
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
	void ProjectSetting::Load_InputSetting(SharedPtr<Json> json)
	{
		if (json == nullptr)
		{
			return;
		}

		auto jsonData = json->GetMember(CATEGORY_INPUT_SETTING_KEY);
		if (jsonData == nullptr)
		{
			return;
		}


		// Axis Mapping Data
		auto axisMappingDataListJson = jsonData->GetMember("AxisMappingDataList");
		if (axisMappingDataListJson == nullptr)
		{
			return;
		}

		axisMappingDataListJson->ForEach([&](SharedPtr<JsonData> mappingJson)
		{
			auto val = mappingJson->GetMember("Name");
			AxisMappingData* data = nullptr;
			if (val && val->IsString())
			{
				data = InputManager::GetInstance().AddAxisMappings(val->GetString());
			}
			else
			{
				data = InputManager::GetInstance().AddAxisMappings("None");
			}

			auto keyDataListJson = mappingJson->GetMember("KeyDataList");
			if (keyDataListJson)
			{
				keyDataListJson->ForEach([&](SharedPtr<JsonData> keyDataJson)
				{
					AxisMappingData::KeyData keyData;
					auto val = keyDataJson->GetMember("KeyCode");
					if (val && val->IsString())
					{
						keyData.Code = StringToKeyCode(val->GetString());
					}
					val = keyDataJson->GetMember("Scale");
					if (val && val->IsFloat())
					{
						keyData.Scale = val->GetFloat();
					}
					data->KeyList.push_back(keyData);
				});


			}
		});






		// Action Mapping Data
		auto actionMappingDataListJson = jsonData->GetMember("ActionMappingDataList");
		if (actionMappingDataListJson == nullptr)
		{
			return;
		}


		actionMappingDataListJson->ForEach([&](SharedPtr<JsonData> mappingJson)
		{
			auto val = mappingJson->GetMember("Name");
			ActionMappingData* data = nullptr;
			if (val && val->IsString())
			{
				data = InputManager::GetInstance().AddActionMappings(val->GetString());
			}
			else
			{
				data = InputManager::GetInstance().AddActionMappings("None");
			}

			auto keyDataListJson = mappingJson->GetMember("KeyDataList");
			if (keyDataListJson)
			{
				keyDataListJson->ForEach([&](SharedPtr<JsonData> keyDataJson)
				{
					ActionMappingData::KeyData keyData;
					auto val = keyDataJson->GetMember("KeyCode");
					if (val && val->IsString())
					{
						keyData.Code = StringToKeyCode(val->GetString());
					}
					val = keyDataJson->GetMember("IsShift");
					if (val && val->IsBool())
					{
						keyData.IsShift = val->GetBool();
					}
					val = keyDataJson->GetMember("IsCtrl");
					if (val && val->IsBool())
					{
						keyData.IsCtrl = val->GetBool();
					}
					val = keyDataJson->GetMember("IsAlt");
					if (val && val->IsBool())
					{
						keyData.IsAlt = val->GetBool();
					}

					data->KeyList.push_back(keyData);
				});


			}
		});


	}
}