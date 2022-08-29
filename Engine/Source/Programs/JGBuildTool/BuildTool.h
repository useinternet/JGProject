#pragma once
#include "Core.h"
#include "Arguments.h"
#include "ModuleInfo.h"

class PBuildTool
{
public:
	static constexpr char const* ARGUMENTS_JSON_FILE_NAME = "buildtool_arguments.json";

	static const PString& BuildToolDirectory();
private:
	// json 颇老 盒籍
	// lua  内靛 积己
	// bat  角青
	PArguments _arguments;

	PHashMap<PString, PList<PModuleInfo>> _engineModuleInfoMap;
	PHashMap<PString, PList<PModuleInfo>> _userModuleInfoMap;

	PHashMap<PString, PModuleInfo> _moduleInfoPool;
public:
	PBuildTool(const PArguments& args);

	bool Run();
private:
	const PArguments& getArguments() const;
	bool collectionModuleInfos(const PString& workDir, const PHashSet<PString>& workCategories, PHashMap<PString, PList<PModuleInfo>>& outModuleInfoMap);
	void collectionModuleInfosInternal(const PString& categoryName, const PString& inCategoryPath, PHashMap<PString, PList<PModuleInfo>>& outModuleInfoMap);
	bool generateBuildScript();
	bool generateBuildScriptInternal(const PHashMap<PString, PList<PModuleInfo>>& moduleInfoMap, const PString& inGroupName, PString& outScript);
	bool makeBuild();
	bool findModuleInfo(const PString& modulePath, PModuleInfo* outModuleInfo) const;
	
	PString getUserProjectName() const;
	const PString& getBuildScriptTemplatePath() const;
};