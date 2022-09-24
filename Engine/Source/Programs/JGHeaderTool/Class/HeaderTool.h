#pragma once
#include "Core.h"
#include "Arguments.h"



struct HProperty
{
	HHashMap<PString, PString> Metas;
	PString Type;
	PString Name;
};

struct HFunction
{
	HHashMap<PString, PString> Metas;
	HList<HProperty> Argmuments;
	PString Return;
	PString Name;
};

struct HClass
{
	PString Name;
	HList<HProperty> Properties;
	HList<HFunction> Functions;
};

struct HHeaderInfo
{
	PString ModuleName;
	PString FileName;
	PString Conents;

	HList<HClass> Classes;
};




class PHeaderTool : public IMemoryObject
{

	// Module이름, Engine
	// C:\JGProject\Engine\Temp\CodeGen <- 여기에 cpp 파일로 생성

	PArguments _args;
	HList<HHeaderInfo> _engineHeaderInfos;
	HList<HHeaderInfo> _userHeaderInfos;

	HHashSet<PString> _moduleSet;
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


	bool extractProperty(HHeaderInfo& headerInfo, HClass& inClass);

	bool generateGenCPPFiles();

	const PArguments& getArguments() const;
};