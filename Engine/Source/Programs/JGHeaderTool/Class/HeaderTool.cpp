#include "HeaderTool.h"
#include "HeaderToolConstants.h"


void HClass::GetCodeGenStaticCreateFuncName(PString* outName) const
{
	if (outName == nullptr || OwnerHeaderInfo == nullptr)
	{
		return;
	}
	outName->Reset();

	*outName = PString::ReplaceAll(HHeaderToolConstants::Template::CodeGenCreateStaticClassFunction, HHeaderToolConstants::Token::ModuleName, (OwnerHeaderInfo->ModuleName));
	*outName = PString::ReplaceAll(*outName, HHeaderToolConstants::Token::ClassName, Name);
}

void HClass::GetCodeGenCreateFuncName(PString* outName) const
{
	if (outName == nullptr || OwnerHeaderInfo == nullptr)
	{
		return;
	}
	outName->Reset();

	*outName = PString::ReplaceAll(HHeaderToolConstants::Template::CodeGenCreateClassFunction, HHeaderToolConstants::Token::ModuleName, (OwnerHeaderInfo->ModuleName));
	*outName = PString::ReplaceAll(*outName, HHeaderToolConstants::Token::ClassName, Name);
}

PHeaderTool::PHeaderTool(const PArguments& args) : _args(args) {}

const PString& PHeaderTool::HeaderToolDirectory()
{
	static PString headerToolDir;
	if (headerToolDir.Empty() == true)
	{
		HFileHelper::CombinePath(HFileHelper::EngineProgramsSourceDirectory(), "JGHeaderTool", &headerToolDir);
	}

	return headerToolDir;
}

bool PHeaderTool::Run()
{
	// header file 수집
	collectionHeaderFiles();
	extractReflectionDatas();
	generateCodeGenFiles();
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

		headerInfo.ModuleName = headerFilePath;
		headerInfo.ModuleName.ReplaceAll(inDir, "");

		if (headerInfo.ModuleName.StartWidth("/") == true)
		{
			headerInfo.ModuleName.SubString(&headerInfo.ModuleName, 1);
		}

		uint64 pos = headerInfo.ModuleName.Find("/");
		if (pos == PString::NPOS)
		{
			//Error Log
			continue;
		}
		headerInfo.ModuleName.SubString(&headerInfo.ModuleName, 0, pos);
		
		if (HFileHelper::ReadAllText(headerFilePath, &headerInfo.Contents) == false)
		{
			// Error Log
			continue;
		}

		// Source
		PString sourcePath = HFileHelper::EngineSourceDirectory();
		HFileHelper::AbsolutePath(sourcePath, &sourcePath);

		PString tempPath;
		HFileHelper::AbsolutePath(headerFilePath, &tempPath);
		tempPath.ReplaceAll(sourcePath, "");

		PString moduleName = headerInfo.ModuleName;
		pos = tempPath.Find(moduleName.Append("/"));
		if (pos != PString::NPOS)
		{
			pos += moduleName.Length();
			tempPath.SubString(&headerInfo.LocalRelativePath, pos);
		}

		inHeaderInfos.push_back(headerInfo);
	}

	return true;
}

bool PHeaderTool::extractReflectionDatas()
{
	if (extractReflectionDatasInternal(_engineHeaderInfos) == false)
	{
		// Error Log
		return false;
	}

	if (extractReflectionDatasInternal(_userHeaderInfos) == false)
	{
		// Error Log
		return false;
	}

	return true;
}

bool PHeaderTool::extractReflectionDatasInternal(HList<HHeaderInfo>& inHeaderInfos)
{
	enum EAnalysisCommand
	{
		None = 0,
		AnalysisClass = 0x0001,
		AnalysisProperty = 0x0002,
		AnalysisFunction = 0x0004,
		AnalysisEnum = 0x0008,
		AnalysisEnumElement = 0x0010,
		AnalysisBaseClass = 0x0020,
	};

	for (HHeaderInfo& headerInfo : inHeaderInfos)
	{
		int32      Mode = EAnalysisCommand::None;
		HEnum* pEnum = nullptr;
		HClass*    pClass = nullptr;
		HProperty* pProperty = nullptr;
		HFunction* pFunction = nullptr;

		HList<PString> lines = headerInfo.Contents.Split('\n');
		for (const PString& line : lines)
		{
			if (isCanAnalysisClass(line) == true)
			{
				if (pClass != nullptr)
				{
					headerInfo.Classes.push_back(*pClass);
					HPlatform::Deallocate(pClass);
				}
				if (pEnum != nullptr)
				{
					headerInfo.Enums.push_back(*pEnum);
					HPlatform::Deallocate(pEnum);
				}

				Mode |= EAnalysisCommand::AnalysisClass;
				Mode |= EAnalysisCommand::AnalysisBaseClass;
				pClass = HPlatform::Allocate<HClass>(&headerInfo);

				if (analysisMetaDatas(line, HHeaderToolConstants::Token::Class, &(pClass->MetaData)) == false)
				{
					// ErrorLog
					Mode = EAnalysisCommand::None;
					HPlatform::Deallocate(pClass);
				}
			}
			else if (isCanAnalysisEnum(line) == true)
			{
				if (pClass != nullptr)
				{
					headerInfo.Classes.push_back(*pClass);
					HPlatform::Deallocate(pClass);
				}
				if (pEnum != nullptr)
				{
					headerInfo.Enums.push_back(*pEnum);
					HPlatform::Deallocate(pEnum);
				}

				Mode |= EAnalysisCommand::AnalysisEnum;
				pEnum = HPlatform::Allocate<HEnum>();

				if (analysisMetaDatas(line, HHeaderToolConstants::Token::Enum, &(pEnum->MetaData)) == false)
				{
					// ErrorLog
					Mode = EAnalysisCommand::None;
					HPlatform::Deallocate(pEnum);
				}
			}
			else if (Mode & EAnalysisCommand::AnalysisClass)
			{
				if (line.Contains("{") == true)
				{
					Mode &= ~EAnalysisCommand::AnalysisBaseClass;
				}
				else if (Mode & EAnalysisCommand::AnalysisBaseClass)
				{
					if (analysisBaseClass(line, pClass) == false)
					{
						// ErrorLog
						HPlatform::Deallocate(pClass);
					}
				}
				else if (isCanAnalysisProperty(line) == true)
				{
					pProperty = HPlatform::Allocate<HProperty>();

					if (analysisMetaDatas(line, HHeaderToolConstants::Token::Property, &pProperty->MetaData) == false)
					{
						HPlatform::Deallocate(pProperty);
					}
					else
					{
						Mode |= EAnalysisCommand::AnalysisProperty;
					}
				}
				else if (isCanAnalysisFunction(line) == true)
				{
					pFunction = HPlatform::Allocate<HFunction>();

					if (analysisMetaDatas(line, HHeaderToolConstants::Token::Function, &pFunction->MetaData) == false)
					{
						HPlatform::Deallocate(pFunction);
					}
					else
					{
						Mode |= EAnalysisCommand::AnalysisFunction;
					}
				}
				else if (Mode & EAnalysisCommand::AnalysisProperty)
				{
					if (pClass != nullptr && pProperty != nullptr && analysisProperty(line, pProperty) == true)
					{
						pClass->Properties.push_back(*pProperty);
					}
					else
					{
						// Error Log
					}

					Mode &= ~EAnalysisCommand::AnalysisProperty;
					HPlatform::Deallocate(pProperty);
				}
				else if (Mode & EAnalysisCommand::AnalysisFunction)
				{
					if (pClass != nullptr && pFunction != nullptr && analysisFunction(line, pFunction) == true)
					{
						pClass->Functions.push_back(*pFunction);
					}
					else
					{
						// Error Log
					}

					Mode &= ~EAnalysisCommand::AnalysisFunction;
					HPlatform::Deallocate(pFunction);
				}
				else
				{
					
				}
			}
			else if(Mode & EAnalysisCommand::AnalysisEnum)
			{
				if (pEnum != nullptr && isCanAnalysisEnumMeta(line) == true)
				{
					if (analysisEnumElement(line, pEnum) == false)
					{
						// Error Log
					}
				}
			}
		}

		if (pProperty != nullptr && pFunction != nullptr)
		{
			// Error Log
			HPlatform::Deallocate(pProperty);
			HPlatform::Deallocate(pFunction);
			HPlatform::Deallocate(pClass);
			HPlatform::Deallocate(pEnum);
			continue;
		}

		if (pClass != nullptr)
		{
			headerInfo.Classes.push_back(*pClass);

			HPlatform::Deallocate(pClass);
		}
		if (pEnum != nullptr)
		{
			headerInfo.Enums.push_back(*pEnum);

			HPlatform::Deallocate(pEnum);
		}
	}

	return true;
}

bool PHeaderTool::isCanAnalysisClass(const PString& line) const
{
	return line.Contains(HHeaderToolConstants::Token::Class);
}

bool PHeaderTool::isCanAnalysisProperty(const PString& line) const
{
	return line.Contains(HHeaderToolConstants::Token::Property);
}

bool PHeaderTool::isCanAnalysisFunction(const PString& line) const
{
	return line.Contains(HHeaderToolConstants::Token::Function);
}

bool PHeaderTool::isCanAnalysisEnum(const PString& line) const
{
	return line.Contains(HHeaderToolConstants::Token::Enum) == true && isCanAnalysisEnumMeta(line) == false;
}

bool PHeaderTool::isCanAnalysisEnumMeta(const PString& line) const
{
	return line.Contains(HHeaderToolConstants::Token::EnumMeta);
}

bool PHeaderTool::analysisBaseClass(const PString& line, HClass* outClass)
{
	if (outClass == nullptr)
	{
		return false;
	}
	PString tempLine = line;
	tempLine.Trim();

	if (tempLine.StartWidth("//") == true)
	{
		return true;
	}
	
	if (tempLine.Contains("class ") == true)
	{
		PString tempStr;
		uint64 pos = tempLine.Find("class ");
		tempLine.SubString(&tempStr, pos);
		tempStr = tempStr.ReplaceAll("class ", "");
		tempStr.SubString(&tempStr, 0, tempStr.Find(" "));

		outClass->Name = tempStr.Trim();
	}


	PString baseClassName = PString::ReplaceAll(line, outClass->Name, "").ReplaceAll(":", "").ReplaceAll(",", "").ReplaceAll("class ", "").Trim();
	baseClassName.SubString(&baseClassName, 0, baseClassName.Find("//"));
	baseClassName.Trim();

	HList<PString> tokens = baseClassName.Split(' ');
	if (tokens.empty() == false)
	{
		baseClassName = (tokens.size() > 1) ? tokens[1].Trim() : tokens[0].Trim();
	}

	if (baseClassName.Empty() == true)
	{
		return false;
	}

	outClass->BaseClasses.push_back(baseClassName);
	return true;
}

bool PHeaderTool::analysisProperty(const PString& line, HProperty* outProperty)
{
	if (outProperty == nullptr)
	{
		return false;
	}

	uint64 commentPos  = line.Find("//");

	HList<PString> tokens = line.Split(' ');
	if (tokens.empty() == true)
	{
		return false;
	}

	if (tokens[0].Contains("//") == true)
	{
		return false;
	}

	PString property_type = tokens[0];
	PString property_name = tokens[1];
	if (property_name.Contains("=") == true)
	{
		property_name = property_name.Split('=')[0];
	}

	outProperty->Type = property_type.Trim();
	outProperty->Name = property_name.Trim();

	return true;
}

bool PHeaderTool::analysisFunction(const PString& line, HFunction* outFunction)
{
	if (outFunction == nullptr)
	{
		return false;
	}
	uint64 commentPos = line.Find("//");

	HList<PString> tokens = line.Split(' ');
	if (tokens.empty() == true)
	{
		return false;
	}

	if (tokens[0].Contains("//") == true)
	{
		return false;
	}

	PString returnType;
	PString funcBody;

	if (tokens[0].Trim() == "virtual")
	{
		returnType = tokens[1];
		funcBody = tokens[2];
	}
	else
	{
		returnType = tokens[0];
		funcBody = tokens[1];
	}

	
	PString funcName;
	PString funcArgsBody;

	HList<PString> funcArgsType;
	HList<PString> funcArgsName;

	uint64 argsStartPos = line.Find("(");
	uint64 argsEndPos   = line.Find(")");
	if (argsStartPos == PString::NPOS || argsEndPos == PString::NPOS)
	{
		return false;
	}

	uint64 funcBodyPos = line.Find(funcBody);
	line.SubString(&funcName, line.Find(funcBody), argsStartPos - funcBodyPos);
	line.SubString(&funcArgsBody, argsStartPos, argsEndPos - argsStartPos);

	HList<PString> argTokens = funcArgsBody.ReplaceAll("(", "").Trim().Split(',');
	for (const PString& argToken : argTokens)
	{
		HList<PString> token = argToken.Split(' ');

		uint64 tokenLen = token.size();
		if (tokenLen < 2)
		{
			return false;
		}

		for (auto iter = token.begin(); iter != token.end();)
		{
			if ((*iter).Length() == 0)
			{
				iter = token.erase(iter);
			}
			else ++iter;
		}

		if (token[0].Trim() == "const")
		{
			funcArgsType.push_back(token[0].Append(" ").Append(token[1]));
			funcArgsName.push_back(token[2].Trim());
		}
		else
		{
			funcArgsType.push_back(token[0].Trim());
			funcArgsName.push_back(token[1].Trim());
		}
	}

	outFunction->Name   = funcName.Trim();
	outFunction->Return = returnType.Trim();

	uint64 len = funcArgsType.size();
	for (uint64 i = 0; i < len; ++i)
	{
		HProperty inProperty;
		inProperty.Type = funcArgsType[i].Trim();
		inProperty.Name = funcArgsName[i].Trim();
		outFunction->Argmuments.push_back(inProperty);
	}
	
	return true;
}

bool PHeaderTool::analysisMetaDatas(const PString& line, const PString& analysisToken, HMeta* pMeta)
{
	if (pMeta == nullptr)
	{
		return false;
	}

	uint64 declarePos  = line.Find("#");
	uint64 commentPos  = line.Find("//");
	uint64 defineStart = line.Find(analysisToken);
	
	if (defineStart == PString::NPOS || defineStart >= commentPos || declarePos != PString::NPOS)
	{
		return false;
	}

	uint64 defineEnd = line.Find(")", defineStart);
	if (defineEnd == PString::NPOS)
	{
		return false;
	}

	PString metaStr;
	line.SubString(&metaStr, defineStart, defineEnd - defineStart);
	metaStr.ReplaceAll(analysisToken, "").ReplaceAll("(", "").ReplaceAll(")", "").ReplaceAll(" ", "").Trim();

	PString token;
	PString key_token;
	uint64 len = metaStr.Length();

	if (len > 0)
	{
		bool bIsMultipleMetaData = false;
		bool bIsEndMultipleMetaData = false;
		for (uint64 i = 0; i <= len; ++i)
		{
			char c = 0;
			if (i < len)
			{
				c = metaStr[i];
			}

			if (c == ',' || i == len)
			{
				if (bIsMultipleMetaData == true)
				{
					pMeta->AddMeta(key_token, token);
				}
				else
				{
					pMeta->AddMeta(token, token);
				}

				token.Reset();

				if (bIsEndMultipleMetaData == true)
				{
					bIsEndMultipleMetaData = false;
					bIsMultipleMetaData = false;
				}
			}
			else if (c == '=')
			{
				bIsMultipleMetaData = true;
				key_token = token;
				token.Reset();
			}
			else if (c == '{') {}
			else if (c == '}')
			{
				bIsEndMultipleMetaData = true;
			}
			else
			{
				token.Append(c);
			}
		}
	}

	return true;
}

bool PHeaderTool::analysisEnumElement(const PString& line, HEnum* pEnum)
{
	if (pEnum == nullptr)
	{
		return false;
	}

	if (line.Contains(HHeaderToolConstants::Token::EnumMeta) == false)
	{
		return false;
	}

	HEnumElement enumElement;

	if (analysisMetaDatas(line, HHeaderToolConstants::Token::EnumMeta, &(enumElement.MetaData)) == false)
	{
		return false;
	}
	
	HList<PString> tokens = line.Split(',');
	if (tokens.empty() == true)
	{
		return false;
	}

	enumElement.Name = tokens[0].ReplaceAll("{", "").Trim();
	pEnum->Elements.push_back(enumElement);

	return true;
}

bool PHeaderTool::generateCodeGenFiles()
{
	const PString& engineCodeGenPath = HFileHelper::EngineCodeGenDirectory();
	HQueue<const HClass*> collectedClassQueue;

	for (const HHeaderInfo& headerInfo : _engineHeaderInfos)
	{
		if (headerInfo.Classes.empty() == true)
		{
			continue;
		}

		if (headerInfo.Classes.size() > 1)
		{
			// Error Log
			continue;
		}

		PString headerFileName;
		headerFileName = PString::ReplaceAll(HHeaderToolConstants::Template::CodeGenHeaderFileName, HHeaderToolConstants::Token::ModuleName, headerInfo.ModuleName);
		headerFileName = PString::ReplaceAll(headerFileName, HHeaderToolConstants::Token::FileName, headerInfo.FileName);

		PString headerFilePath;
		HFileHelper::CombinePath(engineCodeGenPath, headerFileName, &headerFilePath);

		PString headerSourceCode;
		if (generateCodeGenHeaderSourceCode(headerInfo, &headerSourceCode) == false)
		{
			// Error Log
			continue;
		}

		if (HFileHelper::WriteAllText(headerFilePath, headerSourceCode) == false)
		{
			// Error Log
			continue;
		}


		PString cppFileName;
		cppFileName = PString::ReplaceAll(HHeaderToolConstants::Template::CodeGenCppFileName, HHeaderToolConstants::Token::ModuleName, headerInfo.ModuleName);
		cppFileName = PString::ReplaceAll(cppFileName, HHeaderToolConstants::Token::FileName, headerInfo.FileName);

		PString cppFilePath;
		HFileHelper::CombinePath(engineCodeGenPath, cppFileName, &cppFilePath);
		
		PString cppSourceCode;
		if (generateCodeGenCPPSoucreCode(headerInfo, &cppSourceCode) == false)
		{
			// Error Log
			continue;
		}

		if (HFileHelper::WriteAllText(cppFilePath, cppSourceCode) == false)
		{
			// Error Log
			continue;
		}

		collectedClassQueue.push(&(headerInfo.Classes[0]));
		// 생성
	}

	for (const HHeaderInfo& headerInfo : _userHeaderInfos)
	{
		// 아직
	}

	PString engineCodeGenRegisterCppCode;

	if (generateCodeGenRegistration(collectedClassQueue, &engineCodeGenRegisterCppCode) == false)
	{
		// Error Log
		return false;
	}

	PString codeGenPath = HFileHelper::EngineCodeGenDirectory();
	HFileHelper::CombinePath(codeGenPath, "JG.generate.cpp", &codeGenPath);
	if (HFileHelper::WriteAllText(codeGenPath, engineCodeGenRegisterCppCode) == false)
	{
		// Error Log
		return false;
	}

	return true;
}

bool PHeaderTool::generateCodeGenHeaderSourceCode(const HHeaderInfo& headerInfo, PString* outCode)
{	
	if (outCode == nullptr)
	{
		return false;
	}

	outCode->Reset();

	if (headerInfo.Classes.empty())
	{
		return false;
	}

	const HClass& targetClass = headerInfo.Classes[0];

	PString codeGenStaticCreateFuncName;
	PString codeGenCreateFuncName;

	targetClass.GetCodeGenStaticCreateFuncName(&codeGenStaticCreateFuncName);
	targetClass.GetCodeGenCreateFuncName(&codeGenCreateFuncName);
	
	outCode->AppendLine("#include \"CoreDefines.h\"");
	outCode->AppendLine("#if JGCLASS")
		.AppendLine("#undef JGCLASS(...)")
		.AppendLine("#endif").AppendLine("")
		.AppendLine("#if JG_GENERATED_CLASS_BODY")
		.AppendLine("#undef JG_GENERATED_CLASS_BODY()")
		.AppendLine("#endif").AppendLine("")
		.AppendLine("#define JGCLASS(...)  \\")
		.Append("class ").Append(targetClass.Name).AppendLine(";")
		.Append("extern ").Append(codeGenStaticCreateFuncName).AppendLine("\\")
		.Append("extern ").Append(codeGenCreateFuncName).AppendLine("\\").AppendLine("")
		.AppendLine("#define JG_GENERATED_CLASS_BODY() \\")
		.AppendLine("public: \\")
		.AppendLine("PSharedPtr<JGClass> GetClass() const\\")
		.AppendLine("{ \\")
		.Append("\treturn ").Append(PString::ReplaceAll(codeGenCreateFuncName, PString::Format("const %s* fromThis", targetClass.Name), "this")).AppendLine("\\")
		.AppendLine("} \\")
		.AppendLine("static PSharedPtr<JGStruct> GetStaticClass()\\")
		.AppendLine("{ \\")
		.AppendLine(PString::Format("\treturn GObjectGlobalSystem::GetInstance().GetStaticClass<%s>();\\", targetClass.Name))
		.AppendLine("} \\");

	return true;
}

bool PHeaderTool::generateCodeGenCPPSoucreCode(const HHeaderInfo& headerInfo, PString* outCode)
{
	// Class
	if (outCode == nullptr)
	{
		return false;
	}

	outCode->Reset();

	for (const HClass& Class : headerInfo.Classes)
	{
		PString codeGenStaticCreateFuncName;
		PString codeGenCreateFuncName;

		Class.GetCodeGenStaticCreateFuncName(&codeGenStaticCreateFuncName);
		Class.GetCodeGenCreateFuncName(&codeGenCreateFuncName);

		PString staticFuncCode;
		staticFuncCode.AppendLine("\tHList<PSharedPtr<JGFunction>> FunctionMap;");
		staticFuncCode.AppendLine("\tHList<PSharedPtr<JGProperty>> PropertyMap;");

		for (const HProperty& Property : Class.Properties)
		{
			PString typeSourceCode;
			typeSourceCode.Append("JGTYPE(").Append(Property.Type).Append("),");

			PString propNameSourceCode;
			propNameSourceCode.Append("\"").Append(Property.Name).Append("\",");

			PString metaSourceCode;
			if (Property.MetaData.Metas.empty() == true)
			{
				metaSourceCode = "nullptr";
			}
			else
			{
				metaSourceCode.AppendLine("PObjectGlobalsPrivateUtils::MakeStaticMeta(").AppendLine("{");
				for (const HPair<PString, HHashSet<PString>>& pair : Property.MetaData.Metas)
				{
					metaSourceCode.Append("\t\tHPair<PString, HHashSet<PString>>(\"").Append(pair.first).Append("\", {");
					for (const PString& valueToken : pair.second)
					{
						metaSourceCode.Append("\"").Append(valueToken).Append("\"").Append(",");
					}
					metaSourceCode.AppendLine("\t\t}),");
				}

				metaSourceCode.AppendLine("\t})");
			}

			staticFuncCode.Append("\tPropertyMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticProperty(")
				.Append(typeSourceCode)
				.Append(propNameSourceCode)
				.Append(metaSourceCode)
				.AppendLine("));");
		}


		for (const HFunction& function : Class.Functions)
		{
			staticFuncCode.Append("\tFunctionMap.push_back(PObjectGlobalsPrivateUtils::MakeStaticFunction(");
			staticFuncCode.Append("\"").Append(function.Name).AppendLine("\",");
			staticFuncCode.Append("\t\tPObjectGlobalsPrivateUtils::MakeStaticProperty(JGTYPE(").Append(function.Return).AppendLine("), \"\"),");
			staticFuncCode.AppendLine("\t\t{");

			for (const HProperty& Property : Class.Properties)
			{
				staticFuncCode.Append("\t\t\tPObjectGlobalsPrivateUtils::MakeStaticProperty(JGTYPE(")
					.Append(Property.Type)
					.Append("), \"")
					.Append(Property.Name)
					.AppendLine("\"),");
			}

			staticFuncCode.AppendLine("\t\t},");
			staticFuncCode.AppendLine("\t\tPObjectGlobalsPrivateUtils::MakeStaticMeta(");
			staticFuncCode.AppendLine("\t\t{");

			for (const HPair<PString, HHashSet<PString>>& pair : function.MetaData.Metas)
			{
				staticFuncCode.Append("\t\t\tHPair<PString, HHashSet<PString>>(\"").Append(pair.first).Append("\", {");
				for (const PString& token : pair.second)
				{
					staticFuncCode.Append("\"").Append(token).Append("\", ");
				}
				staticFuncCode.AppendLine("\t\t\t}), ");
			}

			staticFuncCode.AppendLine("\t\t})));");
		}

		staticFuncCode
			.Append(PString::Format("\treturn PObjectGlobalsPrivateUtils::MakeStaticClass(JGTYPE(%s),", Class.Name))
			.AppendLine("{");
		
		for (const PString& baseClass : Class.BaseClasses)
		{
			staticFuncCode.AppendLine(PString::Format("\t\t\tJGTYPE(%s), ", baseClass));
		}

		staticFuncCode.AppendLine("\t\t},");
		staticFuncCode.Append("\t\tPropertyMap, FunctionMap,");
		if (Class.MetaData.Metas.empty() == true)
		{
			staticFuncCode.Append("nullptr);");
		}
		else
		{
			staticFuncCode.Append("PObjectGlobalsPrivateUtils::MakeStaticMeta(").AppendLine("{");
			for (const HPair<PString, HHashSet<PString>>& pair : Class.MetaData.Metas)
			{
				staticFuncCode.Append("\t\t\tHPair<PString, HHashSet<PString>>(\"").Append(pair.first).Append("\", {");
				for (const PString& valueToken : pair.second)
				{
					staticFuncCode.Append("\"").Append(valueToken).Append("\"").Append(",");
				}
				staticFuncCode.AppendLine("}),");
			}

			staticFuncCode.AppendLine("\t\t}));");
		}

		PString funcCode;

		funcCode.AppendLine(PString::Format("\t%s* noneConstThisPtr = const_cast<%s*>(fromThis);", Class.Name, Class.Name));
		funcCode.AppendLine(PString::Format("\tPSharedPtr<JGClass> Class =  PObjectGlobalsPrivateUtils::MakeClass(fromThis, GObjectGlobalSystem::GetInstance().GetStaticClass(JGTYPE(%s)));", Class.Name));


		for (const HFunction& function : Class.Functions)
		{
			funcCode.AppendLine(PString::Format("\tif(Class->HasFunction(PName(\"%s\")) == true)", function.Name));
			funcCode.AppendLine("\t{");
			funcCode.Append("\t\tstd::function<").Append(function.Return).Append("(");

			uint64 len = function.Argmuments.size();
			for (uint64 i = 0; i < len; ++i)
			{
				funcCode.Append(function.Argmuments[i].Type);
				if (i < len - 1)
				{
					funcCode.Append(",");
				}
			}
			funcCode.Append(")> functionRef = std::bind(&").Append(Class.Name).Append("::").Append(function.Name).Append(", noneConstThisPtr");
			std::placeholders::_1;
			for (uint64 i = 0; i < len; ++i)
			{
				funcCode.Append(PString::Format(", std::placeholders::_%d", i+ 1));
			}
			funcCode.AppendLine(");");
			funcCode.AppendLine(PString::Format("\t\tif (PObjectGlobalsPrivateUtils::BindFunction(fromThis, Class->FindFunction(PName(\"%s\")), functionRef) == false)", function.Name));
			funcCode.AppendLine("\t\t{");
			funcCode.AppendLine(PString::Format("\t\t\tJG_LOG(CodeGen, ELogLevel::Error, \"%s: Fail Bind Function : %s\");", Class.Name, function.Name));
			funcCode.AppendLine("\t\t}");
			funcCode.AppendLine("\t}");

		}

		funcCode.AppendLine("\treturn Class;");

		outCode->AppendLine("#include \"Core.h\"");
		outCode->AppendLine(PString::Format("#include \"%s\"", Class.OwnerHeaderInfo->LocalRelativePath)).AppendLine("");
		outCode->AppendLine(codeGenStaticCreateFuncName).AppendLine("{").AppendLine(staticFuncCode).AppendLine("}\n");
		outCode->AppendLine(codeGenCreateFuncName).AppendLine("{").AppendLine(funcCode).AppendLine("}\n");

		return true;
	}

	return true;
}

bool PHeaderTool::generateCodeGenRegistration(HQueue<const HClass*>& collectedClassQueue, PString* outCode)
{
	if (outCode == nullptr)
	{
		return false;
	}
	outCode->AppendLine("#include \"CoreSystem.h\"");
	outCode->AppendLine("#include \"Object/ObjectGlobalSystem.h\"");
	outCode->AppendLine("#include \"Object/ObjectGlobals.h\"");
	outCode->AppendLine("#include \"Misc/Log.h\"").AppendLine("");

	outCode->AppendLine(R"(
#ifdef _CODEGEN
#define CODEGEN_API extern "C" __declspec(dllexport)
#else
#define CODEGEN_API extern "C" __declspec(dllimport)
#endif


CODEGEN_API void Link_Module(GCoreSystem* ins)
{
	HCoreSystemPrivate::SetInstance(ins);
}

)");


	PString newCodeIncludeBody;
	PString newCodeGenBody;
	newCodeIncludeBody.AppendLine("");
	newCodeGenBody.AppendLine("");
	while (collectedClassQueue.empty() == false)
	{
		const HClass* targetClass = collectedClassQueue.front(); collectedClassQueue.pop();

		PString codeGenStaticCreateFuncName;
		targetClass->GetCodeGenStaticCreateFuncName(&codeGenStaticCreateFuncName);

		PString onlyCodeGenStaticFuncName = PString::ReplaceAll(codeGenStaticCreateFuncName, "PSharedPtr<JGClass> ", "");

		newCodeIncludeBody.Append("extern ").Append(codeGenStaticCreateFuncName).AppendLine(";");
		newCodeGenBody.Append("    objectGlobalSystem->RegisterJGClass(").Append(onlyCodeGenStaticFuncName).AppendLine(");");
	}

	outCode->AppendLine(newCodeIncludeBody).AppendLine("");
	outCode->AppendLine("CODEGEN_API bool Engine_CodeGenerate(GObjectGlobalSystem* objectGlobalSystem)");
	outCode->AppendLine("{");
	outCode->AppendLine(newCodeGenBody).AppendLine("");
	outCode->AppendLine("    return true;");
	outCode->AppendLine("}");

	return true;
}

const PArguments& PHeaderTool::getArguments() const
{
	return _args;
}

