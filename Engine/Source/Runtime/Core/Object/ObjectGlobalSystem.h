#pragma once


#include "CoreDefines.h"
#include "CoreSystem.h"
#include "String/String.h"

#define TYPE_NULL_ID -1

enum class EPropertyType
{
	Unknown,
	Bool,
	Int8,
	Int16,
	Int32,
	Int64,
	Uint8,
	Uint16,
	Uint32,
	Uint64,
	Float,
	Double,
	List,
	Map,
	HashMap,
	Set,
	HashSet,
	Struct,
	Class,
	Enum,
	EnumFlags,
};


class PProperty
{
	// 변수
	// 이름
public:
	EPropertyType PropertyType = EPropertyType::Unknown;
	PString Name;
	void* PropertyPtr = nullptr;
};

class PField
{
public:
	PList<PProperty> Properties;
};

class PType
{
public:
	PString Name;
	uint64  ID = TYPE_NULL_ID;
};

class PStruct
{
public:
	PType  Type;
	PField Field;
	void* StructPtr = nullptr;
};


class AObjectGlobalSystem : public AGlobalSystemInstance<AObjectGlobalSystem>
{


public:
	AObjectGlobalSystem();

	virtual ~AObjectGlobalSystem() = default;
private:






};





