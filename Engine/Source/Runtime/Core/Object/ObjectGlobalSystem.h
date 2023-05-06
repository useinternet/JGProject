#pragma once


#include "CoreDefines.h"
#include "CoreSystem.h"
#include "String/String.h"
#include "Memory/Memory.h"
#include "Platform/Platform.h"
#include "FileIO/Json.h"
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
class PJson;

class GObjectGlobalSystem : public GGlobalSystemInstance<GObjectGlobalSystem>
{
	friend class GCoreSystem;

	using HCreateClassFunc = HPlatformFunction<PSharedPtr<JGClass>, const JGObject*>;
	using HCreateObjectFunc = HPlatformFunction<PSharedPtr<JGObject>>;
private:
	HHashMap<PName, JGType> _typeMap;

	HHashMap<JGType, PSharedPtr<JGClass>>  _classMap;
	HHashMap<JGType, PSharedPtr<JGEnum>>   _enumMap;

	HHashMap<JGType, HCreateClassFunc>  _createClassFuncPool;
	HHashMap<JGType, HCreateObjectFunc> _createObjectFuncPool;

public:
	virtual ~GObjectGlobalSystem() = default;

protected:
	virtual void Destroy() override;

public:
	PSharedPtr<JGEnum>  GetStaticEnum(const JGType& type) const;
	PSharedPtr<JGClass> GetStaticClass(const JGType& type) const;
	PSharedPtr<JGClass> GetClass(const JGType& type, const JGObject* object) const;
	const JGType& GetType(const PName& typeName) const;
	PSharedPtr<JGObject> NewObject(const JGType& type) const;

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
	PSharedPtr<JGClass> GetClass(const JGObject* object) const
	{
		return GetClass(JGTYPE(T), object);
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

	bool RegisterJGClass(PSharedPtr<JGClass> classObject, const HCreateClassFunc& func, const HCreateObjectFunc& createObjectFunc);
	bool RegisterJGEnum(PSharedPtr<JGEnum> enumObject);
	bool IsRegisteredType(const JGType& type) const;
private:
	bool registerType(PSharedPtr<JGType> type);
	bool canCastInternal(const JGType& destType, const JGType& srcType) const;

private:
	bool codeGen();
	bool auditClassMultipleInheritance() const;
	bool auditClassMultipleInheritanceInteral(const JGType& inType, HHashSet<JGType>& typeVisitor) const;
};

template<class T>
inline PSharedPtr<JGClass> StaticClass()
{
	return GObjectGlobalSystem::GetInstance().GetStaticClass<T>();
}

inline PSharedPtr<JGClass> StaticClass(const JGType& type)
{
	return GObjectGlobalSystem::GetInstance().GetStaticClass(type);
}

template<class T>
inline PSharedPtr<JGClass> BindedClass(const JGObject* obj)
{
	return GObjectGlobalSystem::GetInstance().GetClass<T>(obj);
}

inline PSharedPtr<JGClass> BindedClass(const JGType& type, const JGObject* obj)
{
	return GObjectGlobalSystem::GetInstance().GetClass(type, obj);
}

template<class T>
inline PSharedPtr<JGEnum> StaticEnum()
{
	return GObjectGlobalSystem::GetInstance().GetStaticEnum<T>();
}


template<class T>
inline bool SaveObject(const PString& path, T* obj)
{
	PString typeName = JGTYPE(T).GetName().ToString();

	if (std::is_base_of<JGObject, T>::value == true && obj != nullptr)
	{
		PJson Json;
		Json.AddMember("JGObject", *obj);

		PString jsonString;
		if (PJson::ToString(Json, &jsonString) == true)
		{
			if (HFileHelper::WriteAllText(path, jsonString) == true)
			{
				JG_LOG(Core, ELogLevel::Trace, PString::Format("%s: Success Save at %s", typeName, path));
				return true;
			}
		}
	}

	JG_LOG(Core, ELogLevel::Error, PString::Format("%s: Fail Save", typeName, path));
	return false;
}

template<class T>
inline PSharedPtr<T> LoadObject(const PString& path)
{
	PString typeName = JGTYPE(T).GetName().ToString();

	PString jsonString;
	if (HFileHelper::ReadAllText(path, &jsonString) == true)
	{
		PJson Json;

		if (PJson::ToObject(jsonString, &Json) == true)
		{
			PJsonData ObjectData;
			Json.FindMember("JGObject", &ObjectData);
			PSharedPtr<T> result = Allocate<T>();
			if (ObjectData.GetData(result.GetRawPointer()) == true)
			{
				JG_LOG(Core, ELogLevel::Trace, PString::Format("%s: Success Load at %s", typeName, path));
				return result;
			}
		}
	}

	JG_LOG(Core, ELogLevel::Error, PString::Format("%s: Fail Load at %s", typeName, path));
	return nullptr;
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

inline PSharedPtr<JGObject> AllocateByClass(PSharedPtr<JGClass> inClass)
{
	if (inClass == nullptr)
	{
		return Allocate<JGObject>();
	}

	return GObjectGlobalSystem::GetInstance().NewObject(*(inClass->GetClassType()));
}

template<class T>
inline PSharedPtr<T> AllocateByClass(PSharedPtr<JGClass> inClass)
{
	return Cast<T>(AllocateByClass(inClass));
}





