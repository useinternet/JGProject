#include "PCH/PCH.h"
#include "Core.h"
#include "Class/BuildTool.h"

bool ReadArguments(PArguments* outArguments);
void CreateModuleInfoTemplate();

int32 main(int32 argc, char* argv[])
{
	HCoreSystemArguments coreSystemArgs;
	coreSystemArgs.Flags = ECoreSystemFlags::No_CodeGen;

	GCoreSystem::Create(coreSystemArgs);

	PArguments args;
	if (ReadArguments(&args) == false)
	{
		JG_LOG(BuildTool, ELogLevel::Critical, "Fail read arguments");
		return -1;
	}

	CreateModuleInfoTemplate();

	PBuildTool BuildTool = PBuildTool(args);
	if (BuildTool.Run() == false)
	{
		return -1;
	}

	GCoreSystem::Destroy();

	return 0;
}

bool ReadArguments(PArguments* outArguments)
{
	if (outArguments == nullptr)
	{
		JG_LOG(BuildTool, ELogLevel::Critical, "Arguments is nullptr");
		return false;
	}

	PString buildToolSourcePath = PBuildTool::BuildToolDirectory();

	PString argumentsJsonPath;
	HFileHelper::CombinePath(buildToolSourcePath, PBuildTool::ARGUMENTS_JSON_FILE_NAME, &argumentsJsonPath);

	PString argumentsJsonText;
	if (HFileHelper::ReadAllText(argumentsJsonPath, &argumentsJsonText) == false)
	{
		// Create Default Json File
		PJson json;
		json.AddMember("Arguments", PArguments());

		PString jsonText;
		if (PJson::ToString(json, &jsonText) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Critical, "Fail read json file");
			return false;
		}

		argumentsJsonText = jsonText;

		if (HFileHelper::WriteAllText(argumentsJsonPath, argumentsJsonText) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Critical, "Fail write default arguments json file");
			return false;
		}
	}

	PJson json;
	if (PJson::ToObject(argumentsJsonText, &json) == false)
	{
		JG_LOG(BuildTool, ELogLevel::Critical, "Fail text to json object");
		return false;
	}

	if (json.GetData<PArguments>("Arguments", outArguments) == false)
	{
		JG_LOG(BuildTool, ELogLevel::Critical, "Fail getData in json");
		return false;
	}

	return true;
}

void CreateModuleInfoTemplate()
{
	PString buildToolSourcePath;
	HFileHelper::CombinePath(HFileHelper::EngineProgramsSourceDirectory(), "JGBuildTool", &buildToolSourcePath);

	PString moduleInfoTemplatePath;
	HFileHelper::CombinePath(buildToolSourcePath, "ModuleInfoTemplate.json", &moduleInfoTemplatePath);

	PModuleInfo info;
	info.ModuleFormat = "SharedLib/StaticLib/ConsoleApp";
	info.ModuleDependencies.push_back("Module_A");
	info.ModuleDependencies.push_back("Module_B");

	info.ModuleName = "Name";
	info.ModulePath = "Source/[Category]/[ModuleName]";

	info.Defines.push_back("DEFINE_A");
	info.Defines.push_back("DEFINE_B");

	info.ModuleFilters[(int32)EModuleFilter::DevelopEngine].Config = "DebugConfig/ConfirmConfig/ReleaseConfig";
	info.ModuleFilters[(int32)EModuleFilter::DevelopEngine].Defines.push_back("DEFINE_A");

	info.ModuleFilters[(int32)EModuleFilter::DevelopGame].Config = "DebugConfig/ConfirmConfig/ReleaseConfig";
	info.ModuleFilters[(int32)EModuleFilter::DevelopGame].Defines.push_back("DEFINE_A");

	info.ModuleFilters[(int32)EModuleFilter::ConfirmGame].Config = "DebugConfig/ConfirmConfig/ReleaseConfig";
	info.ModuleFilters[(int32)EModuleFilter::ConfirmGame].Defines.push_back("DEFINE_A");

	info.ModuleFilters[(int32)EModuleFilter::ReleaseGame].Config = "DebugConfig/ConfirmConfig/ReleaseConfig";
	info.ModuleFilters[(int32)EModuleFilter::ReleaseGame].Defines.push_back("DEFINE_A");

	PJson json;
	json.AddMember("ModuleInfo", info);

	PString jsonText;
	PJson::ToString(json, &jsonText);

	HFileHelper::WriteAllText(moduleInfoTemplatePath, jsonText);
}