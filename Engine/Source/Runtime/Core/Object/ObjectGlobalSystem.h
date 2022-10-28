#pragma once


#include "CoreDefines.h"
#include "CoreSystem.h"
#include "String/String.h"
#include "Memory/Memory.h"
#include "Platform/Platform.h"
#include "ObjectGlobals.h"

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

	using HCreateObjectFunc = HPlatformFunction<PSharedPtr<JGClass>, const JGObject*>;
private:
	HHashMap<PName, JGType> _typeMap;

	HHashMap<JGType, PSharedPtr<JGClass>>  _classMap;
	HHashMap<JGType, PSharedPtr<JGEnum>>   _enumMap;

	HHashMap<JGType, HCreateObjectFunc> _createObjectFuncPool;

public:
	virtual ~GObjectGlobalSystem() = default;

protected:
	virtual void Destroy() override;

public:
	PSharedPtr<JGEnum> GetStaticEnum(const JGType& type) const;
	PSharedPtr<JGClass> GetStaticClass(const JGType& type) const;
	PSharedPtr<JGInterface> GetStaticInterface(const JGType& type) const;

	template<class T>
	PSharedPtr<JGEnum> GetStaticEnum() const
	{
		return GetStaticEnum(JGTYPE(T));
	}

	template<class T>
	PSharedPtr<JGClass> GetStaticClass() const
	{
		return GetStaticClass(JGTYPE(T));
	}

	template<class T>
	PSharedPtr<JGClass> GetClass(const JGType& type, T* fromThis)
	{
		return nullptr;
	}


	bool CanCast(const JGType& destType, const JGType& srcType) const;

	template<class T, class U>
	bool CanCast() const
	{
		return CanCast(JGTYPE(T), JGTYPE(U));
	}

	bool RegisterJGClass(PSharedPtr<JGClass> classObject, const HCreateObjectFunc& func);
	bool RegisterJGInterface(PSharedPtr<JGInterface> ifObject);
	bool RegisterJGEnum(PSharedPtr<JGEnum> enumObject);

private:
	bool registerType(PSharedPtr<JGType> type);
	bool canCastInternal(const JGType& destType, const JGType& srcType) const;

private:
	bool codeGen();
};

template<class T>
inline PSharedPtr<JGClass> StaticClass()
{
	return GObjectGlobalSystem::GetInstance().GetStaticClass<T>();
}


template<class T>
inline PSharedPtr<JGEnum> StaticEnum()
{
	return GObjectGlobalSystem::GetInstance().GetStaticEnum<T>();
}


template<class T, class U>
inline PSharedPtr<T> Cast(PSharedPtr<U> ptr)
{
	if (GObjectGlobalSystem::GetInstance().CanCast<T, U>() == false)
	{
		return RawFastCast<T, U>(ptr);
	}

	return GMemoryGlobalSystem::GetInstance().RawFastCastUnChecked<T, U>(ptr);
}

template<class T, class U>
inline PWeakPtr<T> Cast(PWeakPtr<U> ptr)
{
	if (GObjectGlobalSystem::GetInstance().CanCast<T, U>() == false)
	{
		return RawFastCast<T, U>(ptr);
	}

	return GMemoryGlobalSystem::GetInstance().RawFastCastUnChecked<T, U>(ptr);
}


