#include "HeaderTool.h"
#include "HeaderToolConstants.h"

PHeaderTool::PHeaderTool(const PArguments& args) : _args(args) {}

bool PHeaderTool::Run()
{
	// header file 수집
	collectionHeaderFiles();
	extractReflectionDatas();
	generateGenCPPFiles();
	// 분석 ( meta, 함수, 변수 등등 )

	// codeGen .cpp 파일 생성
	return false;
}

bool PHeaderTool::collectionHeaderFiles()
{
	const PArguments& args = getArguments();
	
	for (const PString& workCategory : _args.EngineWorkCategories)
	{
		PString dirPath;
		HFileHelper::CombinePath(_args.EngineWorkDirectory, workCategory, &dirPath);

		if (collectionHeaderFiles_Internal(dirPath, _engineHeaderInfos) == false)
		{
			// Error Log
			return false;
		}
	}

	for (const PString& workCategory : _args.UserWorkCategories)
	{
		PString dirPath;
		HFileHelper::CombinePath(_args.UserWorkDirectory, workCategory, &dirPath);

		if (collectionHeaderFiles_Internal(dirPath, _userHeaderInfos) == false)
		{
			// Error Log
			return false;
		}
	}

	return true;
}

bool PHeaderTool::collectionHeaderFiles_Internal(const PString& inDir, HList<HHeaderInfo>& inHeaderInfos)
{

	if (HFileHelper::Exists(inDir) == false)
	{
		// Error Log
		return false;
	}

	if (HFileHelper::IsDirectory(inDir) == false)
	{
		// Error Log
		return false;
	}
	HList<PString> headerFileList;
	HFileHelper::FileListInDirectory(inDir, &headerFileList, true, { ".h" });

	for (const PString& headerFilePath : headerFileList)
	{
		HHeaderInfo headerInfo;

		HFileHelper::FileNameOnly(headerFilePath, &headerInfo.FileName);
		HFileHelper::FilePathOnly(headerFilePath, &headerInfo.ModuleName);
		HFileHelper::FileNameOnly(headerFilePath, &headerInfo.ModuleName);

		if (HFileHelper::ReadAllText(headerFilePath, &headerInfo.Conents) == false)
		{
			// Error Log
		}

		inHeaderInfos.push_back(headerInfo);
	}

	return true;
}

bool PHeaderTool::extractReflectionDatas()
{
	// 정보


	return false;
}

bool PHeaderTool::extractReflectionDatasInternal(HList<HHeaderInfo>& inHeaderInfos)
{
	for (HHeaderInfo& headerInfo : inHeaderInfos)
	{
		
	}


	return false;
}

bool PHeaderTool::extractProperty(HHeaderInfo& headerInfo, HClass& inClass)
{
	uint64 offset = 0;
	while (offset != PString::NPOS)
	{
		uint64 defineStart = headerInfo.Conents.Find(HHeaderToolConstants::Token::Property, offset);
		if (defineStart == PString::NPOS)
		{
			// ErrorLog
			continue;
		}

		uint64 defineEnd   = headerInfo.Conents.Find(")", defineStart);
		if (defineStart == PString::NPOS)
		{
			// ErrorLog
			continue;
		}

		uint64 propertyStart = defineEnd + 1;
		uint64 propertyEnd    = headerInfo.Conents.Find(";", propertyStart);
		if (propertyEnd == PString::NPOS)
		{
			// ErrorLog
			continue;
		}

		PString defineStr;
		PString propertyStr;

		headerInfo.Conents.SubString(defineStr, defineStart, defineEnd - defineStart);
		headerInfo.Conents.SubString(propertyStr, propertyStart, propertyEnd - propertyStart);

		defineStr.ReplaceAll(HHeaderToolConstants::Token::Property, "").ReplaceAll("(", "").ReplaceAll(")", "").Trim();
		propertyStr.ReplaceAll(";", "").ReplaceAll("=", " ").Trim();

		// Meta Data 수집
		HList<PString> metaDatas = defineStr.Split(',');
		HList<PString> propertys = propertyStr.Split(' ');

		if (propertys.size() < 2)
		{
			//ErrorLog
			continue;
		}


	}

	return false;
}

bool PHeaderTool::generateGenCPPFiles()
{
	return false;
}

const PArguments& PHeaderTool::getArguments() const
{
	return _args;
}
