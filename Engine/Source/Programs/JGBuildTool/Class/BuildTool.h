#pragma once
#include "Core.h"
#include "Arguments.h"
#include "ModuleInfo.h"

/* 스케줄러 완료 후 작업 시작
local PCH_HEADER = "PCH.h"
local PCH_HEADER_PATH = "Source/PCH/PCH.h"
local PCH_CPP_PATH    = "Source/PCH/PCH.cpp"

		pchheader (PCH_HEADER)
		pchsource (PCH_CPP_PATH)
		files {
			path .. "**.h",
			path .. "**.cpp",
			path .. "**.ico",
			path .. "**.rc",
			PCH_HEADER_PATH,
			PCH_CPP_PATH,
		}
*/

class PBuildTool : public IMemoryObject
{
public:
	static constexpr char const* ARGUMENTS_JSON_FILE_NAME = "buildtool_arguments.json";
	static constexpr char const* SCRIPT_NAME = "jgengine.lua";
	static constexpr char const* BATCH_NAME = "jgengine.bat";
	static constexpr char const* PREMAKE_FILE_NAME = "premake5.exe";
	static const PString& BuildToolDirectory();
private:
	// json 파일 분석
	// lua  코드 생성
	// bat  실행
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
	bool makeProjectFiles();
	bool findModuleInfo(const PString& modulePath, PModuleInfo* outModuleInfo) const;
	
	PString getUserProjectName() const;
	const PString& getBuildScriptTemplatePath() const;
};