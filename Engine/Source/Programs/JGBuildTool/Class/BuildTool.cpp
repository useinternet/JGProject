#include "PCH/PCH.h"
#include "BuildTool.h"
#include<stdio.h>

const PString& PBuildTool::BuildToolDirectory()
{
	static PString buildToolDir;
	if(buildToolDir.Empty() == true)
	{
		HFileHelper::CombinePath(HFileHelper::EngineProgramsSourceDirectory(), "JGBuildTool", &buildToolDir);
	}
	
	return buildToolDir;
}

PBuildTool::PBuildTool(const PArguments& args) : _arguments(args) {}

bool PBuildTool::Run()
{
	const PArguments& arguments = getArguments();

	// Step 1. Engine Module 정보 수집 
	JG_LOG(BuildTool, ELogLevel::Info, "Step 1. Engine Collection Module Infos..");
	if (collectionModuleInfos(arguments.EngineWorkDirectory, arguments.EngineWorkCategories, _engineModuleInfoMap) == false)
	{
		return false;
	}

	// Step 2. User Module 정보 수집 
	JG_LOG(BuildTool, ELogLevel::Info, "Step 2. User Collection Module Infos..");
	if (collectionModuleInfos(arguments.UserWorkDirectory, arguments.UserWorkCategories, _userModuleInfoMap) == false)
	{
		return false;
	}

	// Step 3. lua 코드 작성
	JG_LOG(BuildTool, ELogLevel::Info, "Step 3. Generate Build Script..");
	if (generateBuildScript() == false)
	{
		return false;
	}

	// Step 4. 
	JG_LOG(BuildTool, ELogLevel::Info, "Step 4. Insert Include PCH..");
	insertIncludePCHHeaderCode();

	// Step 5. make Build
	JG_LOG(BuildTool, ELogLevel::Info, "Step 5. Make Project Files..");
	if (makeProjectFiles() == false)
	{
		return false;
	}

	JG_LOG(BuildTool, ELogLevel::Info, "Compelete Build Tool Run..");
	return true;
}

const PArguments& PBuildTool::getArguments() const
{
	return _arguments;
}

bool PBuildTool::collectionModuleInfos(const PString& workDir, const HHashSet<PString>& workCategories, HHashMap<PString, HList<PModuleInfo>>& outModuleInfoMap)
{
	if (HFileHelper::Exists(workDir) == false)
	{
		JG_LOG(BuildTool, ELogLevel::Critical, "work directory doesn't exist.");

		return false;
	}

	HList<PString> workCategoies;
	HFileHelper::FileListInDirectory(workDir, &workCategoies);

	for (const PString& categoryPath : workCategoies)
	{
		PString categoryName;
		HFileHelper::FileName(categoryPath, &categoryName);

		if (HFileHelper::Exists(categoryPath) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Critical, "work category(%s) doesn't exist.", categoryName);
			continue;
		}

		if (HFileHelper::IsDirectory(categoryPath) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Critical, "work category(%s) must directory file", categoryName);
			continue;
		}

		if (workCategories.find(categoryName) == workCategories.end())
		{
			continue;
		}

		collectionModuleInfosInternal(categoryName, categoryPath, outModuleInfoMap);
	}

	uint64 totalCount = 0;
	HHashMap<PString, uint64> moduleCountMap;

	for (const HPair<PString, HList<PModuleInfo>>& pair : outModuleInfoMap)
	{
		moduleCountMap[pair.first] = 0;

		JG_LOG(BuildTool, ELogLevel::Trace, "----------------------------------------------------");
		JG_LOG(BuildTool, ELogLevel::Trace, "%s Category Module List", pair.first);
		JG_LOG(BuildTool, ELogLevel::Trace, "----------------------------------------------------");
		for (const PModuleInfo& moduleInfo : pair.second)
		{
			JG_LOG(BuildTool, ELogLevel::Trace, "%s", moduleInfo.ModuleName);
			moduleCountMap[pair.first] += 1;
			totalCount += 1;
		}

		JG_LOG(BuildTool, ELogLevel::Trace, "----------------------------------------------------");
	}

	for (const HPair<PString, uint64>& pair : moduleCountMap)
	{
		JG_LOG(BuildTool, ELogLevel::Trace, "%s: %d", pair.first, pair.second);
	}

	JG_LOG(BuildTool, ELogLevel::Trace, "Total: %d", totalCount);
	JG_LOG(BuildTool, ELogLevel::Trace, "----------------------------------------------------");
	return true;
}

void PBuildTool::collectionModuleInfosInternal(const PString& categoryName, const PString& inCategoryPath, HHashMap<PString, HList<PModuleInfo>>& outModuleInfoMap)
{
	HList<PString> modulePathList;
	HFileHelper::FileListInDirectory(inCategoryPath, &modulePathList, true);

	PString moduleInfoFileName;
	PString moduleName;

	for (const PString& modulePath : modulePathList)
	{
		HFileHelper::FileName(modulePath, &moduleName);

		if (HFileHelper::Exists(modulePath) == false)
		{
			continue;
		}

		if (HFileHelper::IsDirectory(modulePath) == false)
		{
			continue;
		}

		PModuleInfo moduleInfo;
		if (findModuleInfo(modulePath, &moduleInfo) == false)
		{
			continue;
		}
		
		outModuleInfoMap[categoryName].push_back(moduleInfo);
		_moduleInfoPool[moduleName] = moduleInfo;
	}
}

bool PBuildTool::generateBuildScript()
{
	PString engineModuleScript;
	if (generateBuildScriptInternal(_engineModuleInfoMap, "Engine", engineModuleScript) == false)
	{
		return false;
	}

	PString userModuleScript;
	if (generateBuildScriptInternal(_userModuleInfoMap, getUserProjectName(), userModuleScript) == false)
	{
		return false;
	}


	PString buildScript;
	if (HFileHelper::ReadAllText(getBuildScriptTemplatePath(), &buildScript) == false)
	{
		return false;
	}
	buildScript.AppendLine("");
	buildScript.AppendLine(engineModuleScript);
	buildScript.AppendLine(userModuleScript);

	const PArguments& arguments = getArguments();

	PString resultPath;
	HFileHelper::CombinePath(BuildToolDirectory(), SCRIPT_NAME, &resultPath);
	
	if (HFileHelper::WriteAllText(resultPath, buildScript) == false)
	{
		return false;
	}

	JG_LOG(BuildTool, ELogLevel::Trace, "Generate Build Script: %s", resultPath);
	return true;
}

bool PBuildTool::generateBuildScriptInternal(const HHashMap<PString, HList<PModuleInfo>>& moduleInfoMap, const PString& inGroupName, PString& outScript)
{
	const PArguments& arguments = getArguments();

	outScript = PString();
	for (const HPair<PString, HList<PModuleInfo>>& pair : moduleInfoMap)
	{
		outScript.Append("\t\t");

		PString groupName = inGroupName;
		groupName.Append("/").Append(pair.first);

		// GroupName
		outScript.Append("group \"").Append(groupName).AppendLine("\"");

		for (const PModuleInfo& moduleInfo : pair.second)
		{
			JG_LOG(BuildTool, ELogLevel::Trace, "Module: %s: Start Generate Build Script", moduleInfo.ModuleName);

			// Project
			outScript.Append("\t\t\t");
			outScript.Append("project \"").Append(moduleInfo.ModuleName).AppendLine("\"");
			outScript.Append("\t\t\t\t");

			PString thirdPartyPath = arguments.ThirdPartyDirectory;
			thirdPartyPath.ReplaceAll(HFileHelper::EngineDirectory(), "");

			PString includeDirs;
			includeDirs.Append("\"").Append(moduleInfo.ModulePath).Append("\", ");
			includeDirs.Append("\"").Append(thirdPartyPath).Append("\", ");
			includeDirs.Append("\"").Append("Source/").Append("\", ");
			PString links;
			PString defines;

			PString postcommands = PString::Format(R"(
				postbuildcommands {
					"copy $(TargetDir)%s.dll $(TargetDir)..\\",
					"copy $(TargetDir)%s.lib $(TargetDir)..\\",
					"copy $(TargetDir)%s.exp $(TargetDir)..\\",
					"copy $(TargetDir)%s.pdb $(TargetDir)..\\",
					"copy $(TargetDir)%s.dll $(TargetDir)..\\%s_Dynamic.dll"
})"
, moduleInfo.ModuleName, moduleInfo.ModuleName, moduleInfo.ModuleName,
moduleInfo.ModuleName, moduleInfo.ModuleName, moduleInfo.ModuleName);

			bool bIsSharedLib = moduleInfo.ModuleFormat == "SharedLib";

			for (const PString& moduleName : moduleInfo.ModuleDependencies)
			{
				if (_moduleInfoPool.find(moduleName) == _moduleInfoPool.end())
				{
					JG_LOG(BuildTool, ELogLevel::Error, "Module: %s: is not exists dependency module(%s)", moduleInfo.ModuleName, moduleName);
					continue;
				}

				const PModuleInfo& dependencyModule = _moduleInfoPool[moduleName];

				includeDirs.Append("\"").Append(dependencyModule.ModulePath).Append("\", ");
				links.Append("\"").Append(moduleName).Append("\", ");
			}

			defines.Append("{");
			for (const PString& define : moduleInfo.Defines)
			{
				defines.Append("\"").Append(define).Append("\", ");
			}
			defines.Append("}");

			outScript.Append("includedirs{ ");
			outScript.Append(includeDirs);
			outScript.AppendLine("}");
			outScript.Append("\t\t\t\t");

			outScript.Append("links{ ");
			outScript.Append(links);
			outScript.AppendLine("}");
			outScript.Append("\t\t\t\t");

			if (bIsSharedLib)
			{
				outScript.Append("SetDynamicCPPProjectConfig(");
			}
			else
			{
				outScript.Append("SetCPPProjectConfig(");
			}

			outScript.Append("\"").Append(moduleInfo.ModuleFormat).Append("\", ");
			outScript.Append("\"").Append(moduleInfo.ModulePath).Append("\", ");

			if (defines.Empty() == false)
			{
				outScript.Append(defines);
			}
			outScript.AppendLine(")");
			outScript.Append("\t\t\t\t");

			outScript.AppendLine("filter \"configurations:DevelopEngine\"");
			outScript.Append("\t\t\t\t\t").Append(moduleInfo.ModuleFilters[(int32)EModuleFilter::DevelopEngine].Config).AppendLine("()");
			outScript.Append("\t\t\t\t\t").AppendLine(getDefines(moduleInfo, EModuleFilter::DevelopEngine));
			if (bIsSharedLib)
			{
				outScript.Append("\t\t\t\t\t").AppendLine(postcommands);
			}
			outScript.Append("\t\t\t\t");

			outScript.AppendLine("filter \"configurations:DevelopGame\"");
			outScript.Append("\t\t\t\t\t").Append(moduleInfo.ModuleFilters[(int32)EModuleFilter::DevelopGame].Config).AppendLine("()");
			outScript.Append("\t\t\t\t\t").AppendLine(getDefines(moduleInfo, EModuleFilter::DevelopGame));
			if (bIsSharedLib)
			{
				outScript.Append("\t\t\t\t\t").AppendLine(postcommands);
			}
			outScript.Append("\t\t\t\t");

			outScript.AppendLine("filter \"configurations:ConfirmGame\"");
			outScript.Append("\t\t\t\t\t").Append(moduleInfo.ModuleFilters[(int32)EModuleFilter::ConfirmGame].Config).AppendLine("()");
			outScript.Append("\t\t\t\t\t").AppendLine(getDefines(moduleInfo, EModuleFilter::ConfirmGame));
			if (bIsSharedLib)
			{
				outScript.Append("\t\t\t\t\t").AppendLine(postcommands);
			}
			outScript.Append("\t\t\t\t");

			outScript.AppendLine("filter \"configurations:ReleaseGame\"");
			outScript.Append("\t\t\t\t\t").Append(moduleInfo.ModuleFilters[(int32)EModuleFilter::ReleaseGame].Config).AppendLine("()");
			outScript.Append("\t\t\t\t\t").AppendLine(getDefines(moduleInfo, EModuleFilter::ReleaseGame));
			if (bIsSharedLib)
			{
				outScript.Append("\t\t\t\t\t").AppendLine(postcommands);
			}

			outScript.AppendLine("");
			outScript.AppendLine("");

			JG_LOG(BuildTool, ELogLevel::Trace, "Module: %s: Compelete Generate Build Script", moduleInfo.ModuleName);
		}
	}

	return true;
}

void PBuildTool::insertIncludePCHHeaderCode()
{
	insertIncludePCHHeaderCodeInternal(_engineModuleInfoMap);
	insertIncludePCHHeaderCodeInternal(_userModuleInfoMap);
}

void PBuildTool::insertIncludePCHHeaderCodeInternal(const HHashMap<PString, HList<PModuleInfo>>& moduleInfoMap)
{
	for (const HPair<PString, HList<PModuleInfo>>& pair : moduleInfoMap)
	{
		for (const PModuleInfo& moduleInfo : pair.second)
		{
			PString modulePath = HFileHelper::EngineDirectory() / moduleInfo.ModulePath;

			HList<PString> cppFileList;

			HFileHelper::FileListInDirectory(modulePath, &cppFileList, true, { ".cpp" });

			for (const PString& cppFilePath : cppFileList)
			{
				PString cppText;
				if (HFileHelper::ReadAllText(cppFilePath, &cppText) == false)
				{
					JG_LOG(BuildTool, ELogLevel::Error, "Module: %s: Fail Insert Include PCH.H Code", moduleInfo.ModuleName);
					continue;
				}

				uint64 pos = cppText.Find("#include \"PCH/PCH.h\"");
				if (pos == PString::NPOS)
				{
					cppText.Insert("#include \"PCH/PCH.h\"\n", 0);
				}

				if (HFileHelper::WriteAllText(cppFilePath, cppText) == false)
				{
					JG_LOG(BuildTool, ELogLevel::Error, "Module: %s: Fail Insert Include PCH.H Code", moduleInfo.ModuleName);
					continue;
				}
			}
		}
	}
}

bool PBuildTool::makeProjectFiles()
{
	PString newScriptPath;
	HFileHelper::CombinePath(BuildToolDirectory(), SCRIPT_NAME, &newScriptPath);

	PString oldScriptPath;
	HFileHelper::CombinePath(HFileHelper::EngineDirectory(), SCRIPT_NAME, &oldScriptPath);


	PString newScriptText;
	if (HFileHelper::ReadAllText(newScriptPath, &newScriptText) == false)
	{
		JG_LOG(BuildTool, ELogLevel::Error, "Fail Read New Script");
		return false;
	}

	if (HFileHelper::WriteAllText(oldScriptPath, newScriptText) == false)
	{
		JG_LOG(BuildTool, ELogLevel::Error, "Fail Replace New Script");
		return false;
	}

	PString premakeFileOriginPath;
	HFileHelper::CombinePath(HFileHelper::EngineBuildDirectory(), PREMAKE_FILE_NAME, &premakeFileOriginPath);
	HFileHelper::AbsolutePath(premakeFileOriginPath, &premakeFileOriginPath);

	PString premakeTempFilePath;
	HFileHelper::CombinePath(HFileHelper::EngineDirectory(), PREMAKE_FILE_NAME, &premakeTempFilePath);
	HFileHelper::AbsolutePath(premakeTempFilePath, &premakeTempFilePath);

	if (HFileHelper::CopyFileOrDirectory(premakeFileOriginPath, premakeTempFilePath) == false)
	{
		JG_LOG(BuildTool, ELogLevel::Error, "Fail Copy Premake File");
		return false;
	}

	PString batCommand;
	batCommand.Append("call ").Append("\"").Append(premakeTempFilePath).Append("\"").Append(" vs2022 --file=").Append(oldScriptPath);

	PString batFilePath;
	HFileHelper::CombinePath(HFileHelper::EngineDirectory(), BATCH_NAME, &batFilePath);
	HFileHelper::AbsolutePath(batFilePath, &batFilePath);

	if (HFileHelper::WriteAllText(batFilePath, batCommand) == false)
	{
		JG_LOG(BuildTool, ELogLevel::Error, "Fail Create Batch File");
		return false;
	}
	
	system(batFilePath.GetCStr());

	//HFileHelper::RemoveFileOrDirectory(batFilePath);
	//HFileHelper::RemoveFileOrDirectory(oldScriptPath);
	//HFileHelper::RemoveFileOrDirectory(premakeTempFilePath);

	return true;
}

bool PBuildTool::findModuleInfo(const PString& modulePath, PModuleInfo* outModuleInfo) const
{
	if (outModuleInfo == nullptr)
	{
		return false;
	}

	const PArguments& arguments = getArguments();

	HList<PString> fileListInModule;
	HFileHelper::FileListInDirectory(modulePath, &fileListInModule);

	PString moduleName;
	HFileHelper::FileName(modulePath, &moduleName);

	for (const PString& filePath : fileListInModule)
	{
		if (HFileHelper::IsDirectory(filePath) == true)
		{
			continue;
		}

		PString fileName;
		HFileHelper::FileName(filePath, &fileName);

		HList<PString> split = fileName.Split('.');
		if (split.empty() == false && split.size() == 3)
		{
			PString fileExtension         = split[2];
			PString moduleRecognitionName = split[1];

			if (fileExtension != arguments.ModuleInfoFileExtension ||
				moduleRecognitionName != arguments.ModuleRecognitionName)
			{
				continue;
			}

			if (moduleName != split[0])
			{
				continue;
			}

			PString jsonText;
			if (HFileHelper::ReadAllText(filePath, &jsonText) == false)
			{
				JG_LOG(BuildTool, ELogLevel::Error, "module(%s) fail read json file", moduleName);
				continue;
			}

			PJson json;
			if (PJson::ToObject(jsonText, &json) == false)
			{
				JG_LOG(BuildTool, ELogLevel::Error, "module(%s) fail PJson::ToObject", moduleName);
				continue;
			}

			if (json.GetData("ModuleInfo", outModuleInfo) == false)
			{
				JG_LOG(BuildTool, ELogLevel::Error, "module(%s) fail json getdata", moduleName);
				continue;
			}

			return true;
		}
	}

	return false;
}

PString PBuildTool::getDefines(const PModuleInfo& moduleInfo, EModuleFilter filter) const
{
	const PArguments& args = getArguments();

	PString result;
	result.Append("defines{");

	for (const PString& define : args.Defines)
	{
		result.Append("\"").Append(define).Append("\", ");
	}

	for (const PString& define : args.GlobalFilters[(int32)filter].Defines)
	{
		result.Append("\"").Append(define).Append("\", ");
	}
	for (const PString& define : moduleInfo.ModuleFilters[(int32)filter].Defines)
	{
		result.Append("\"").Append(define).Append("\", ");
	}
	result.Append("}");

	return result;
}

PString PBuildTool::getUserProjectName() const
{
	const PArguments& arguments = getArguments();

	PString UserProjectName;
	HFileHelper::FileName(arguments.UserWorkDirectory, &UserProjectName);

	return UserProjectName;
}

const PString& PBuildTool::getBuildScriptTemplatePath() const
{
	const PArguments& arguments = getArguments();
	return arguments.BuildScriptTemplatePath;
}
