#pragma once
#include "Core.h"

class PModuleInfo : public IJsonable
{
public:
	/*
	* ex) SharedLib, StaticLib, ConsoleApp*/
	PString ModuleFormat;

	PList<PString> ModuleDependencies;

	PString ModuleName;
	PString ModulePath;
	PList<PString> Defines;

	PString DevelopEngineFilter;
	PString DevelopGameFilter;
	PString DevelopConfirmGameFilter;
	PString DevelopReleaseGameFilter;

protected:
	virtual void WriteJson(PJsonData& json) const
	{
		json.AddMember("ModuleFormat", ModuleFormat);
		json.AddMember("ModuleDependencies", ModuleDependencies);
		json.AddMember("ModuleName", ModuleFormat);
		json.AddMember("ModulePath", ModuleDependencies);
		json.AddMember("Defines", Defines);
		json.AddMember("DevelopEngineFilter", DevelopEngineFilter);
		json.AddMember("DevelopGameFilter", DevelopGameFilter);
		json.AddMember("DevelopConfirmGameFilter", DevelopConfirmGameFilter);
		json.AddMember("DevelopReleaseGameFilter", DevelopReleaseGameFilter);
	}

	virtual void ReadJson(const PJsonData& json)
	{
		if (json.GetData("ModuleFormat", &ModuleFormat) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Error, "ModuleFormat: fail read json data");
		}

		if (json.GetData("ModuleDependencies", &ModuleDependencies) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Error, "ModuleDependencies: fail read json data");
		}

		if (json.GetData("ModuleName", &ModuleName) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Error, "ModuleName: fail read json data");
		}

		if (json.GetData("ModulePath", &ModulePath) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Error, "ModulePath: fail read json data");
		}

		if (json.GetData("Defines", &Defines) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Error, "Defines: fail read json data");
		}

		if (json.GetData("DevelopEngineFilter", &DevelopEngineFilter) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Error, "DevelopEngineFilter: fail read json data");
		}

		if (json.GetData("DevelopGameFilter", &DevelopGameFilter) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Error, "DevelopGameFilter: fail read json data");
		}

		if (json.GetData("DevelopConfirmGameFilter", &DevelopConfirmGameFilter) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Error, "DevelopConfirmGameFilter: fail read json data");
		}

		if (json.GetData("DevelopReleaseGameFilter", &DevelopReleaseGameFilter) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Error, "DevelopReleaseGameFilter: fail read json data");
		}
	}
};