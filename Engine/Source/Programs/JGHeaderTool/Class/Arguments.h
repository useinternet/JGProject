#pragma once
#include "Core.h"


class PArguments : public IMemoryObject, public IJsonable
{
public:
	PString EngineWorkDirectory;
	PString EngineCodeGenCppFilePath;
	HHashSet<PString> EngineWorkCategories;
	

	PString UserWorkDirectory;
	HHashSet<PString> UserWorkCategories;

	PArguments()
	{
		EngineWorkDirectory = HFileHelper::EngineSourceDirectory();
		EngineWorkCategories.insert("Editor");
		EngineWorkCategories.insert("Runtime");
		EngineWorkCategories.insert("Programs");

		HFileHelper::CombinePath(EngineWorkDirectory, "Runtime/Core/CodeGen/JG.generate.cpp", &EngineCodeGenCppFilePath);
		HFileHelper::CombinePath(HFileHelper::EngineSourceDirectory(), "Dummy", &UserWorkDirectory);
	}

	virtual ~PArguments() = default;
protected:
	virtual void WriteJson(PJsonData& json) const
	{
		json.AddMember("EngineWorkDirectory", EngineWorkDirectory);
		json.AddMember("EngineWorkCategories", EngineWorkCategories);
		json.AddMember("EngineCodeGenCppFilePath", EngineCodeGenCppFilePath);
		json.AddMember("UserWorkDirectory", UserWorkDirectory);
		json.AddMember("UserWorkCategories", UserWorkCategories);
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

		if (json.GetData("EngineCodeGenCppFilePath", &EngineCodeGenCppFilePath) == false)
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
	}
};