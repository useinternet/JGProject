#pragma once
#include "Core.h"
#include "Arguments.h"
#include "ModuleInfo.h"

class PBuildTool : public IMemoryObject
{
public:
	static constexpr char const* ARGUMENTS_JSON_FILE_NAME = "buildtool_arguments.json";
	static constexpr char const* SCRIPT_NAME = "jgengine.lua";
	static constexpr char const* BATCH_NAME = "jgengine.bat";
	static constexpr char const* PREMAKE_FILE_NAME = "premake5.exe";
	static const PString& BuildToolDirectory();
private:
	PArguments _arguments;

	HHashMap<PString, HList<PModuleInfo>> _engineModuleInfoMap;
	HHashMap<PString, HList<PModuleInfo>> _userModuleInfoMap;

	HHashMap<PString, PModuleInfo> _moduleInfoPool;
public:
	PBuildTool(const PArguments& args);
	virtual ~PBuildTool() = default;

	bool Run();
private:
	const PArguments& getArguments() const;
	bool collectionModuleInfos(const PString& workDir, const HHashSet<PString>& workCategories, HHashMap<PString, HList<PModuleInfo>>& outModuleInfoMap);
	void collectionModuleInfosInternal(const PString& categoryName, const PString& inCategoryPath, HHashMap<PString, HList<PModuleInfo>>& outModuleInfoMap);
	bool generateBuildScript();
	bool generateBuildScriptInternal(const HHashMap<PString, HList<PModuleInfo>>& moduleInfoMap, const PString& inGroupName, PString& outScript);
	void insertIncludePCHHeaderCode();
	void insertIncludePCHHeaderCodeInternal(const HHashMap<PString, HList<PModuleInfo>>& moduleInfoMap);
	bool makeProjectFiles();
	bool findModuleInfo(const PString& modulePath, PModuleInfo* outModuleInfo) const;
	PString getDefines(const PModuleInfo& moduleInfo, EModuleFilter filter) const;
	PString getUserProjectName() const;
	const PString& getBuildScriptTemplatePath() const;
};