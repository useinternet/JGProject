#include "PCH/PCH.h"


//
#include "Core.h"
#include "Class/Arguments.h"
#include "Class/HeaderTool.h"
#include <iostream>

bool ReadArguments(PArguments* outArguments);


int main()
{
	HCoreSystemArguments coreSystemArgs;
	coreSystemArgs.Flags = ECoreSystemFlags::No_CodeGen;

	GCoreSystem::Create(coreSystemArgs);

	PArguments args;
	if (ReadArguments(&args) == false)
	{
		JG_LOG(HeaderTool, ELogLevel::Critical, "Fail read arguments");
		return -1;
	}
	
	PHeaderTool headerTool = PHeaderTool(args);
	if (headerTool.Run() == false)
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
		JG_LOG(HeaderTool, ELogLevel::Critical, "Arguments is nullptr");
		return false;
	}

	PString headerToolSourcePath = PHeaderTool::HeaderToolDirectory();

	PString argumentsJsonPath;
	HFileHelper::CombinePath(headerToolSourcePath, PHeaderTool::ARGUMENTS_JSON_FILE_NAME, &argumentsJsonPath);

	PString argumentsJsonText;
	if (HFileHelper::ReadAllText(argumentsJsonPath, &argumentsJsonText) == false)
	{
		// Create Default Json File
		PJson json;
		json.AddMember("Arguments", PArguments());

		PString jsonText;
		if (PJson::ToString(json, &jsonText) == false)
		{
			JG_LOG(HeaderTool, ELogLevel::Critical, "Fail read json file");
			return false;
		}

		argumentsJsonText = jsonText;

		if (HFileHelper::WriteAllText(argumentsJsonPath, argumentsJsonText) == false)
		{
			JG_LOG(HeaderTool, ELogLevel::Critical, "Fail write default arguments json file");
			return false;
		}
	}

	PJson json;
	if (PJson::ToObject(argumentsJsonText, &json) == false)
	{
		JG_LOG(HeaderTool, ELogLevel::Critical, "Fail text to json object");
		return false;
	}

	if (json.GetData("Arguments", outArguments) == false)
	{
		JG_LOG(HeaderTool, ELogLevel::Critical, "Fail getData in json");
		return false;
	}

	return true;


}