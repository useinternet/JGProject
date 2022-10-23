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

class JGStruct;
class JGClass;
class JGEnum;

class GObjectGlobalSystem : public GGlobalSystemInstance<GObjectGlobalSystem>
{
	friend class GCoreSystem;
private:
	HHashMap<PName, JGType> _typeMap;

	HHashMap<JGType, PSharedPtr<JGClass>>  _classMap;
	HHashMap<JGType, PSharedPtr<JGEnum>>   _enumMap;

public:
	virtual ~GObjectGlobalSystem() = default;

protected:
	virtual void Destroy() override;

public:
	PSharedPtr<JGClass> GetStaticClass(const JGType& type) const;
	PSharedPtr<JGInterface> GetStaticInterface(const JGType& type) const;

	template<class T>
	PSharedPtr<JGClass> GetStaticClass() const
	{
		return GetStaticClass(JGTYPE(T));
	}

	bool CanCast(const JGType& destType, const JGType& srcType) const;

	template<class T, class U>
	bool CanCast() const
	{
		return CanCast(JGTYPE(T), JGTYPE(U));
	}

	bool RegisterJGClass(PSharedPtr<JGClass> classObject);
	bool RegisterJGEnum(PSharedPtr<JGEnum> enumObject);

private:
	bool registerType(PSharedPtr<JGType> type);

	bool canCastInternal(const JGType& destType, const JGType& srcType) const;

private:
	bool codeGen();
};

template<class T, class U>
inline PSharedPtr<T> Cast(PSharedPtr<U> ptr)
{
	if (GObjectGlobalSystem::GetInstance().CanCast<T, U>() == false)
	{
		return nullptr;
	}

	return RawFastCast<T, U>(ptr);
}

template<class T, class U>
inline PWeakPtr<T> Cast(PWeakPtr<U> ptr)
{
	if (GObjectGlobalSystem::GetInstance().CanCast<T, U>() == false)
	{
		return nullptr;
	}

	return RawFastCast<T, U>(ptr);
}


