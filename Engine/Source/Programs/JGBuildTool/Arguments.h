#pragma once
#include "Core.h"

class PArguments : public IMemoryObject, public IJsonable
{
public:
	PString EngineWorkDirectory;
	HHashSet<PString> EngineWorkCategories;

	PString UserWorkDirectory;
	HHashSet<PString> UserWorkCategories;

	PString ModuleInfoFileExtension;
	PString ModuleRecognitionName;

	PString ThirdPartyDirectory;
	PString BuildScriptTemplatePath;

	PArguments()
	{
		EngineWorkDirectory = PFileHelper::EngineSourceDirectory();
		EngineWorkCategories.insert("Editor");
		EngineWorkCategories.insert("Runtime");
		EngineWorkCategories.insert("Programs");

		PFileHelper::CombinePath(PFileHelper::EngineSourceDirectory(), "Dummy", &UserWorkDirectory);

		ModuleInfoFileExtension = "json";
		ModuleRecognitionName = "module";

		ThirdPartyDirectory = PFileHelper::EngineThirdPartyDirectory();

		PFileHelper::CombinePath(PFileHelper::EngineProgramsSourceDirectory(), "JGBuildTool/Template/BuildTemplate.lua", &BuildScriptTemplatePath);
	}

	virtual ~PArguments() = default;

protected:
	virtual void WriteJson(PJsonData& json) const
	{
		json.AddMember("EngineWorkDirectory", EngineWorkDirectory);
		json.AddMember("EngineWorkCategories", EngineWorkCategories);

		json.AddMember("UserWorkDirectory", UserWorkDirectory);
		json.AddMember("UserWorkCategories", UserWorkCategories);

		json.AddMember("ModuleInfoFileExtension", ModuleInfoFileExtension);
		json.AddMember("ModuleRecognitionName", ModuleRecognitionName);

		json.AddMember("ThirdPartyDirectory", ThirdPartyDirectory);
		json.AddMember("BuildScriptTemplatePath", BuildScriptTemplatePath);
	}

	virtual void ReadJson(PJsonData& json)
	{
		if (json.GetData("EngineWorkDirectory", &EngineWorkDirectory) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Error, "EngineWorkDirectory: fail read json data");
		}

		if (json.GetData("EngineWorkCategories", &EngineWorkCategories) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Error, "EngineWorkCategories: fail read json data");
		}

		if (json.GetData("UserWorkDirectory", &UserWorkDirectory) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Error, "UserWorkDirectory: fail read json data");
		}

		if (json.GetData("UserWorkCategories", &UserWorkCategories) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Error, "UserWorkCategories: fail read json data");
		}

		if (json.GetData("ModuleInfoFileExtension", &ModuleInfoFileExtension) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Error, "ModuleInfoFileExtension: fail read json data");
		}

		if (json.GetData("ModuleRecognitionName", &ModuleRecognitionName) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Error, "ModuleRecognitionName: fail read json data");
		}

		if (json.GetData("ThirdPartyDirectory", &ThirdPartyDirectory) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Error, "ThirdPartyDirectory: fail read json data");
		}

		if (json.GetData("BuildScriptTemplatePath", &BuildScriptTemplatePath) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Error, "BuildScriptTemplatePath: fail read json data");
		}
	}
};