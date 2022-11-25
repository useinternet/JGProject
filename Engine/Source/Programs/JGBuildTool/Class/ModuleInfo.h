#pragma once
#include "Core.h"
#include "Arguments.h"

using EModuleFilter = EGlobalFilter;
class HModuleFilter : public IJsonable
{
public:
	PString Config;
	HList<PString> Defines;

public:
	virtual ~HModuleFilter() = default;

protected:
	virtual void WriteJson(PJsonData& json) const override
	{
		json.AddMember("Config", Config);
		json.AddMember("Defines", Defines);
	}

	virtual void ReadJson(const PJsonData& json) override
	{
		if (json.GetData("Config", &Config) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Error, "Defines: fail read json data");
		}

		if (json.GetData("Defines", &Defines) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Error, "Defines: fail read json data");
		}
	}
};

class PModuleInfo : public IMemoryObject, public IJsonable
{
public:
	PString ModuleFormat;

	HList<PString> ModuleDependencies;

	PString ModuleName;
	PString ModulePath;
	HList<PString> Defines;

	HModuleFilter ModuleFilters[(int32)EModuleFilter::Count];

	//PString DevelopEngineFilter;
	//PString DevelopGameFilter;
	//PString DevelopConfirmGameFilter;
	//PString DevelopReleaseGameFilter;

	virtual ~PModuleInfo() = default;
protected:
	virtual void WriteJson(PJsonData& json) const
	{
		json.AddMember("ModuleFormat", ModuleFormat);
		json.AddMember("ModuleDependencies", ModuleDependencies);
		json.AddMember("ModuleName", ModuleFormat);
		json.AddMember("ModulePath", ModuleDependencies);
		json.AddMember("Defines", Defines);


		json.AddMember("DevelopEngine", ModuleFilters[(int32)EModuleFilter::DevelopEngine]);
		json.AddMember("DevelopGame", ModuleFilters[(int32)EModuleFilter::DevelopGame]);
		json.AddMember("ConfirmGame", ModuleFilters[(int32)EModuleFilter::ConfirmGame]);
		json.AddMember("ReleaseGame", ModuleFilters[(int32)EModuleFilter::ReleaseGame]);
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

		if (json.GetData("DevelopEngine", &ModuleFilters[(int32)EModuleFilter::DevelopEngine]) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Error, "DevelopEngine: fail read json data");
		}

		if (json.GetData("DevelopGame", &ModuleFilters[(int32)EModuleFilter::DevelopGame]) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Error, "DevelopGame: fail read json data");
		}

		if (json.GetData("ConfirmGame", &ModuleFilters[(int32)EModuleFilter::ConfirmGame]) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Error, "ConfirmGame: fail read json data");
		}

		if (json.GetData("ReleaseGame", &ModuleFilters[(int32)EModuleFilter::ReleaseGame]) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Error, "ReleaseGame: fail read json data");
		}
	}
};