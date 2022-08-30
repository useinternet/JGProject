#include "BuildTool.h"
#include<stdio.h>

const PString& PBuildTool::BuildToolDirectory()
{
	static PString buildToolDir;
	if(buildToolDir.Empty() == true)
	{
		PFileHelper::CombinePath(PFileHelper::EngineProgramsSourceDirectory(), "JGBuildTool", &buildToolDir);
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

	// Step 4. make Build
	JG_LOG(BuildTool, ELogLevel::Info, "Step 4. Make Build..");
	if (makeBuild() == false)
	{
		return false;
	}
	

	return true;
}

const PArguments& PBuildTool::getArguments() const
{
	return _arguments;
}

bool PBuildTool::collectionModuleInfos(const PString& workDir, const PHashSet<PString>& workCategories, PHashMap<PString, PList<PModuleInfo>>& outModuleInfoMap)
{
	if (PFileHelper::Exists(workDir) == false)
	{
		JG_LOG(BuildTool, ELogLevel::Critical, "work directory doesn't exist.");

		return false;
	}

	PList<PString> workCategoies;
	PFileHelper::FileListInDirectory(workDir, &workCategoies);

	for (const PString& categoryPath : workCategoies)
	{
		PString categoryName;
		PFileHelper::FileName(categoryPath, &categoryName);

		if (PFileHelper::Exists(categoryPath) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Critical, "work category(%s) doesn't exist.", categoryName);
			continue;
		}

		if (PFileHelper::IsDirectory(categoryPath) == false)
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
	PHashMap<PString, uint64> moduleCountMap;

	for (const PPair<PString, PList<PModuleInfo>>& pair : outModuleInfoMap)
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

	for (const PPair<PString, uint64>& pair : moduleCountMap)
	{
		JG_LOG(BuildTool, ELogLevel::Trace, "%s: %d", pair.first, pair.second);
	}

	JG_LOG(BuildTool, ELogLevel::Trace, "Total: %d", totalCount);
	JG_LOG(BuildTool, ELogLevel::Trace, "----------------------------------------------------");
	return true;
}

void PBuildTool::collectionModuleInfosInternal(const PString& categoryName, const PString& inCategoryPath, PHashMap<PString, PList<PModuleInfo>>& outModuleInfoMap)
{
	PList<PString> modulePathList;
	PFileHelper::FileListInDirectory(inCategoryPath, &modulePathList);

	PString moduleInfoFileName;
	PString moduleName;

	for (const PString& modulePath : modulePathList)
	{
		PFileHelper::FileName(modulePath, &moduleName);

		if (PFileHelper::Exists(modulePath) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Critical, "module(%s) doesn't exist.", moduleName);
			continue;
		}

		if (PFileHelper::IsDirectory(modulePath) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Critical, "module(%s) must directory file", moduleName);
			continue;
		}

		PModuleInfo moduleInfo;
		if (findModuleInfo(modulePath, &moduleInfo) == false)
		{
			JG_LOG(BuildTool, ELogLevel::Warning, "module(%s) can't find module info", moduleName);
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
	if (PFileHelper::ReadAllText(getBuildScriptTemplatePath(), &buildScript) == false)
	{
		return false;
	}
	buildScript.AppendLine("");
	buildScript.AppendLine(engineModuleScript);
	buildScript.AppendLine(userModuleScript);

	const PArguments& arguments = getArguments();

	PString resultPath;
	PFileHelper::CombinePath(BuildToolDirectory(), SCRIPT_NAME, &resultPath);
	
	if (PFileHelper::WriteAllText(resultPath, buildScript) == false)
	{
		return false;
	}

	JG_LOG(BuildTool, ELogLevel::Trace, "Generate Build Script: %s", resultPath);
	return true;
}

bool PBuildTool::generateBuildScriptInternal(const PHashMap<PString, PList<PModuleInfo>>& moduleInfoMap, const PString& inGroupName, PString& outScript)
{
	const PArguments& arguments = getArguments();

	outScript = PString();
	for (const PPair<PString, PList<PModuleInfo>>& pair : moduleInfoMap)
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
			thirdPartyPath.ReplaceAll(PFileHelper::EngineDirectory(), "");

			PString includeDirs;
			includeDirs.Append("\"").Append(moduleInfo.ModulePath).Append("\", ");
			includeDirs.Append("\"").Append(thirdPartyPath).Append("\", ");

			PString links;
			PString defines;

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

			outScript.Append("SetCPPProjectConfig(");
			outScript.Append("\"").Append(moduleInfo.ModuleFormat).Append("\", ");
			outScript.Append("\"").Append(moduleInfo.ModulePath).Append("\", ");

			if (defines.Empty() == false)
			{
				outScript.Append(defines);
			}
			outScript.AppendLine(")");
			outScript.Append("\t\t\t\t");

			outScript.AppendLine("filter \"configurations:DevelopEngine\"");
			outScript.Append("\t\t\t\t\t").Append(moduleInfo.DevelopEngineFilter).AppendLine("()");
			outScript.Append("\t\t\t\t");

			outScript.AppendLine("filter \"configurations:DevelopGame\"");
			outScript.Append("\t\t\t\t\t").Append(moduleInfo.DevelopGameFilter).AppendLine("()");
			outScript.Append("\t\t\t\t");

			outScript.AppendLine("filter \"configurations:ConfirmGame\"");
			outScript.Append("\t\t\t\t\t").Append(moduleInfo.DevelopConfirmGameFilter).AppendLine("()");
			outScript.Append("\t\t\t\t");

			outScript.AppendLine("filter \"configurations:ReleaseGame\"");
			outScript.Append("\t\t\t\t\t").Append(moduleInfo.DevelopReleaseGameFilter).AppendLine("()");

			outScript.AppendLine("");
			outScript.AppendLine("");

			JG_LOG(BuildTool, ELogLevel::Trace, "Module: %s: Compelete Generate Build Script", moduleInfo.ModuleName);
		}
	}

	return true;
}

bool PBuildTool::makeBuild()
{
	PString newScriptPath;
	PFileHelper::CombinePath(BuildToolDirectory(), SCRIPT_NAME, &newScriptPath);

	PString oldScriptPath;
	PFileHelper::CombinePath(PFileHelper::EngineDirectory(), SCRIPT_NAME, &oldScriptPath);


	PString newScriptText;
	if (PFileHelper::ReadAllText(newScriptPath, &newScriptText) == false)
	{
		JG_LOG(BuildTool, ELogLevel::Error, "Fail Read New Script");
		return false;
	}

	PString oldScriptText;
	if (PFileHelper::ReadAllText(oldScriptPath, &oldScriptText) == false)
	{
		JG_LOG(BuildTool, ELogLevel::Error, "Fail Read Old Script");
		return false;
	}

	if (PFileHelper::WriteAllText(oldScriptPath, newScriptText) == false)
	{
		JG_LOG(BuildTool, ELogLevel::Error, "Fail Replace New Script");
		return false;
	}

	PString premakeFilePath;
	PFileHelper::CombinePath(PFileHelper::EngineDirectory(), PREMAKE_FILE_NAME, &premakeFilePath);
	PFileHelper::AbsolutePath(premakeFilePath, &premakeFilePath);

	PString batCommand;
	batCommand.Append("call ").Append("\"").Append(premakeFilePath).Append("\"").Append(" vs2022 --file=").Append(oldScriptPath);

	PString batFilePath;
	PFileHelper::CombinePath(PFileHelper::EngineDirectory(), BATCH_NAME, &batFilePath);
	PFileHelper::AbsolutePath(batFilePath, &batFilePath);

	if (PFileHelper::WriteAllText(batFilePath, batCommand) == false)
	{
		JG_LOG(BuildTool, ELogLevel::Error, "Fail Create Batch File");
		return false;
	}
	
	system(batFilePath.GetCStr());

	PFileHelper::RemoveFileOrDirectory(batFilePath);

	return true;
}

bool PBuildTool::findModuleInfo(const PString& modulePath, PModuleInfo* outModuleInfo) const
{
	if (outModuleInfo == nullptr)
	{
		return false;
	}

	const PArguments& arguments = getArguments();

	PList<PString> fileListInModule;
	PFileHelper::FileListInDirectory(modulePath, &fileListInModule);

	PString moduleName;
	PFileHelper::FileName(modulePath, &moduleName);

	for (const PString& filePath : fileListInModule)
	{
		if (PFileHelper::IsDirectory(filePath) == true)
		{
			continue;
		}

		PString fileName;
		PFileHelper::FileName(filePath, &fileName);

		PList<PString> split = fileName.Split('.');
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
			if (PFileHelper::ReadAllText(filePath, &jsonText) == false)
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

PString PBuildTool::getUserProjectName() const
{
	const PArguments& arguments = getArguments();

	PString UserProjectName;
	PFileHelper::FileName(arguments.UserWorkDirectory, &UserProjectName);

	return UserProjectName;
}

const PString& PBuildTool::getBuildScriptTemplatePath() const
{
	const PArguments& arguments = getArguments();
	return arguments.BuildScriptTemplatePath;
}
