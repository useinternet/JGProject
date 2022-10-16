#pragma once
#include "Core.h"
#include "Arguments.h"

struct HMeta
{
	HHashMap<PString, HHashSet<PString>> Metas;

public:
	void AddMeta(const PString& key, const PString& value)
	{
		Metas[key].insert(value);
	}
};

struct HProperty
{
	PString Type;
	PString Name;
	HMeta MetaData;
};

struct HFunction
{
	HList<HProperty> Argmuments;
	PString Return;
	PString Name;
	HMeta MetaData;
};

struct HClass
{
	PString Name;
	HList<PString>   BaseClasses;
	HList<HProperty> Properties;
	HList<HFunction> Functions;
	HMeta MetaData;
};

struct HEnumElement
{
	HMeta MetaData;
	PString Name;
};

struct HEnum
{
	HList<HEnumElement> Elements;
	HMeta MetaData;
};

struct HHeaderInfo
{
	PString ModuleName;
	PString FileName;
	PString Contents;

	HList<HClass> Classes;
	HList<HEnum> Enums;
};

class PHeaderTool : public IMemoryObject
{
public:
	static constexpr char const* ARGUMENTS_JSON_FILE_NAME = "headertool_arguments.json";

private:
	// Module이름, Engine
	// C:\JGProject\Engine\Temp\CodeGen <- 여기에 cpp 파일로 생성

	PArguments _args;
	HList<HHeaderInfo> _engineHeaderInfos;
	HList<HHeaderInfo> _userHeaderInfos;

	HHashSet<PString> _moduleSet;

public:
	static const PString& HeaderToolDirectory();

public:
	PHeaderTool(const PArguments& args);
	virtual ~PHeaderTool() = default;

public:
	bool Run();

private:
	bool collectionHeaderFiles();
	bool collectionHeaderFiles_Internal(const PString& inDir, HList<HHeaderInfo>& inHeaderInfos);

	bool extractReflectionDatas();
	bool extractReflectionDatasInternal(HList<HHeaderInfo>& inHeaderInfos);

	bool isCanAnalysisClass(const PString& line);
	bool isCanAnalysisProperty(const PString& line);
	bool isCanAnalysisFunction(const PString& line);
	bool isCanAnalysisEnum(const PString& line);
	bool isCanAnalysisEnumMeta(const PString& line);

	bool analysisBaseClass(const PString& line, HClass* outClass);
	bool analysisProperty(const PString& line, HProperty* outProperty);
	bool analysisFunction(const PString& line, HFunction* outFunction);
	bool analysisMetaDatas(const PString& line, const PString& token, HMeta* pMeta);
	bool analysisEnumElement(const PString& line, HEnum* pEnum);

	bool generateCodeGenFiles();
	bool generateCodeGenHeaderSourceCode(const HHeaderInfo& headerInfo, PString* outCode);
	bool generateCodeGenCPPSoucreCode(const HHeaderInfo& headerInfo, PString* outCode);
	bool generateCodeGenRegistration(const HQueue<const HClass*>& collectedClassQueue);

	const PArguments& getArguments() const;
};