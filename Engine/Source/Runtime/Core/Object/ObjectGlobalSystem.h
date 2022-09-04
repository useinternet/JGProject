#pragma once


#include "CoreDefines.h"
#include "CoreSystem.h"
#include "String/String.h"
#include "Memory/Memory.h"
#include "ObjectGlobals.h"

#define CODE_GENERATION_INCLUDE_BEGIN(...)
#define CODE_GENERATION_INCLUDE_END(...)

#define CODE_GENERATION_BEGIN(...)
#define CODE_GENERATION_END(...)

enum class EPropertyType
{
	Unknown,
	Void,
	Bool,
	Int8,
	Int16,
	Int32,
	Int64,
	Uint8,
	Uint16,
	Uint32,
	Uint64,
	Float32,
	Float64,
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

class JGType;
class JGStruct;
class JGClass;
class JGEnum;

class GObjectGlobalSystem : public GGlobalSystemInstance<GObjectGlobalSystem>
{
	friend class GCoreSystem;
private:
	HHashMap<PName, uint64> _typeIDMap;

	HHashMap<uint64, PSharedPtr<JGClass>>  _classMap;
	HHashMap<uint64, PSharedPtr<JGStruct>> _structMap;
	HHashMap<uint64, PSharedPtr<JGEnum>>   _enumMap;
public:
	virtual ~GObjectGlobalSystem() = default;

protected:
	virtual void Destroy() override;

public:
	PSharedPtr<JGStruct> GetStaticStruct(uint64 typeID) const;

	template<class T>
	PSharedPtr<JGStruct> GetStaticStruct() const
	{
		uint64 typeID = JGTYPEID(T);
		return GetStaticStruct(typeID);
	}

private:
	bool registerClass(PSharedPtr<JGClass> classObject);
	bool registerStruct(PSharedPtr<JGStruct> structObject);
	bool registerEnum(PSharedPtr<JGEnum> enumObject);

	bool registerType(PSharedPtr<JGType> type);

	PSharedPtr<JGStruct> copyStruct(uint64 typeID);

private:
	bool codeGen();
};





