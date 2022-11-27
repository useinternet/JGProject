#pragma once
#include "Core.h"

enum class EGlobalFilter
{
	DevelopEngine,
	DevelopGame,
	ConfirmGame,
	ReleaseGame,
	Count
};

class HGlobalFilter : public IJsonable
{
public:
	HList<PString> Defines;

public:
	virtual ~HGlobalFilter() = default;

protected:
	virtual void WriteJson(PJsonData& json) const override
	{
		json.AddMember("Defines", Defines);
	}

	virtual void ReadJson(const PJsonData& json) override
	{
		if (json.GetData("Defines", &Defines) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Error, "Defines: fail read json data");
		}
	}
};

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

	HList<PString> Defines;
	HGlobalFilter GlobalFilters[(int32)EGlobalFilter::Count];

	PArguments()
	{
		EngineWorkDirectory = HFileHelper::EngineSourceDirectory();
		EngineWorkCategories.insert("Editor");
		EngineWorkCategories.insert("Runtime");
		EngineWorkCategories.insert("Programs");

		HFileHelper::CombinePath(HFileHelper::EngineSourceDirectory(), "Dummy", &UserWorkDirectory);

		ModuleInfoFileExtension = "json";
		ModuleRecognitionName = "module";

		ThirdPartyDirectory = HFileHelper::EngineThirdPartyDirectory();

		HFileHelper::CombinePath(HFileHelper::EngineProgramsSourceDirectory(), "JGBuildTool/Template/BuildTemplate.lua", &BuildScriptTemplatePath);
	}

	virtual ~PArguments() = default;

protected:
	virtual void WriteJson(PJsonData& json) const override
	{
		json.AddMember("EngineWorkDirectory", EngineWorkDirectory);
		json.AddMember("EngineWorkCategories", EngineWorkCategories);

		json.AddMember("UserWorkDirectory", UserWorkDirectory);
		json.AddMember("UserWorkCategories", UserWorkCategories);

		json.AddMember("ModuleInfoFileExtension", ModuleInfoFileExtension);
		json.AddMember("ModuleRecognitionName", ModuleRecognitionName);

		json.AddMember("ThirdPartyDirectory", ThirdPartyDirectory);
		json.AddMember("BuildScriptTemplatePath", BuildScriptTemplatePath);

		json.AddMember("Defines", Defines);
		json.AddMember("DevelopEngine", GlobalFilters[(int32)EGlobalFilter::DevelopEngine]);
		json.AddMember("DevelopGame", GlobalFilters[(int32)EGlobalFilter::DevelopGame]);
		json.AddMember("ConfirmGame", GlobalFilters[(int32)EGlobalFilter::ConfirmGame]);
		json.AddMember("ReleaseGame", GlobalFilters[(int32)EGlobalFilter::ReleaseGame]);
	}

	virtual void ReadJson(const PJsonData& json) override
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

		if (json.GetData("Defines", &Defines) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Error, "Defines: fail read json data");
		}

		if (json.GetData("DevelopEngine", &GlobalFilters[(int32)EGlobalFilter::DevelopEngine]) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Error, "DevelopEngine: fail read json data");
		}

		if (json.GetData("DevelopGame", &GlobalFilters[(int32)EGlobalFilter::DevelopGame]) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Error, "DevelopGame: fail read json data");
		}

		if (json.GetData("ConfirmGame", &GlobalFilters[(int32)EGlobalFilter::ConfirmGame]) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Error, "ConfirmGame: fail read json data");
		}

		if (json.GetData("ReleaseGame", &GlobalFilters[(int32)EGlobalFilter::ReleaseGame]) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Error, "ReleaseGame: fail read json data");
		}
	}
};