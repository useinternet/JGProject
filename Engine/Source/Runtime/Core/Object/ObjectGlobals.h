#pragma once

#include "JGObject.h"
#include "Math/Math.h"
#include "Memory/Memory.h"
#include "String/String.h"
#include "Misc/StdExternal.h"

#define TYPE_NULL_ID -1

#define JGTYPE(x)    JGType::GenerateType<##x>()
#define JGTYPEID(x)  JGType::GenerateTypeID<##x>()

class JGMeta;
class JGProperty;
class JGField;
class JGType;
class JGFunction;
class JGStruct;
class GObjectGlobalSystem;

class PObjectGlobalsPrivateUtils
{
	using address = uint64;
public:
	static PSharedPtr<JGMeta>     MakeStaticMeta(const PList<PPair<PString, PString>>& pairList);
	static PSharedPtr<JGProperty> MakeStaticProperty(const JGType& type, const PString& name, PSharedPtr<JGMeta> metaData = nullptr);
	static PSharedPtr<JGFunction> MakeStaticFunction(const PString& name, PSharedPtr<JGProperty> returnProperty, const PList<PSharedPtr<JGProperty>>& args = PList<PSharedPtr<JGProperty>>(), PSharedPtr<JGMeta> metaData = nullptr);
	static PSharedPtr<JGStruct>   MakeStaticStruct(const JGType& type, const PList<PSharedPtr<JGProperty>>& properties, const PList<PSharedPtr<JGFunction>>& functions, PSharedPtr<JGMeta> metaData = nullptr);


	template<class T>
	static PSharedPtr<JGStruct> MakeStruct(const T* fromThis, PSharedPtr<JGStruct> staticStruct);
};

class JGMeta : public JGObject
{
	friend PObjectGlobalsPrivateUtils;
	friend GObjectGlobalSystem;
protected:
	PHashMap<PString, PString> MetaDataMap;
};

class JGType : public JGObject
{
	friend PObjectGlobalsPrivateUtils;
	friend GObjectGlobalSystem;
protected:
	uint64 ID = TYPE_NULL_ID;
	uint64 Size = 0;
public:
	JGType();
	virtual ~JGType() = default;

public:
	uint64 GetID() const;
	uint64 GetSize() const;
public:
	template<class T>
	static uint64 GenerateTypeID()
	{
		static uint64 type_id = TYPE_NULL_ID;
		if (type_id == TYPE_NULL_ID)
		{
			type_id = typeid(T).hash_code();
		}
		return type_id;
	}

	template<class T>
	static JGType GenerateType()
	{
		static const char* type_name = typeid(T).name();

		JGType result;
		result.ID = GenerateTypeID<T>();
		result.Size = sizeof(T);
		result.SetName(type_name);
		
		return result;
	}

};

class JGProperty : public JGObject
{
	friend PObjectGlobalsPrivateUtils;
	friend GObjectGlobalSystem;
protected:
	PSharedPtr<JGMeta> MetaData;
	PSharedPtr<JGType> Type;

	PWeakPtr<JGObject> OwnerObject;
	void* DataPtr;
public:
	JGProperty();
	virtual ~JGProperty() = default;
};

class JGFunction : public JGObject
{
	friend PObjectGlobalsPrivateUtils;
	friend GObjectGlobalSystem;
protected:
	PSharedPtr<JGMeta>			  MetaData;
	PSharedPtr<JGProperty>		  Return;
	PList<PSharedPtr<JGProperty>> Arguments;

	PWeakPtr<JGObject> OwnerObject;
public:
	JGFunction();
	virtual ~JGFunction() = default;
};

class JGField : public JGObject
{
	friend PObjectGlobalsPrivateUtils;
	friend GObjectGlobalSystem;
protected:
	PList<PSharedPtr<JGProperty>> Properties;
	PList<PSharedPtr<JGFunction>> Functions;

	PHashMap<PString, uint64> PropertyMap;
	PHashMap<PString, uint64> FunctionMap;
public:
	//bool HasProperty(const PString& name);

public:
	JGField();
	virtual ~JGField() = default;
};


class JGStruct : public JGField
{
	friend PObjectGlobalsPrivateUtils;
	friend GObjectGlobalSystem;
protected:
	PSharedPtr<JGType>  Type;
	PSharedPtr<JGMeta>  MetaData;

public:
	JGStruct();
	virtual ~JGStruct() = default;

public:
	PSharedPtr<JGType> GetType() const;

};

class JGEnum : public JGObject
{
	friend PObjectGlobalsPrivateUtils;
	friend GObjectGlobalSystem;
protected:
	PSharedPtr<JGType>  Type;
	PHashMap<uint64, PSharedPtr<JGMeta>>  MetaDataMap;
public:
	JGEnum();
	virtual ~JGEnum() = default;

public:
	PSharedPtr<JGType> GetType() const;
};


class JGVTable : public JGObject
{


};

class JGClass : public JGStruct
{
	friend PObjectGlobalsPrivateUtils;
	friend GObjectGlobalSystem;
protected:
	PSharedPtr<JGVTable> VTable;

public:
	JGClass();
	virtual ~JGClass() = default;
};

template<class T>
inline PSharedPtr<JGStruct> PObjectGlobalsPrivateUtils::MakeStruct(const T* fromThis, PSharedPtr<JGStruct> staticStruct)
{
	if (fromThis == nullptr)
	{
		return nullptr;
	}

	address rawAddr = (address)fromThis;

	int32 propertyCount = (int32)staticStruct->Properties.size();

	for (int32 i = 0; i < propertyCount; ++i)
	{
		PSharedPtr<JGProperty> property = staticStruct->Properties[i];
		PSharedPtr<JGType>	   type		= property->Type;
		
		staticStruct->PropertyMap.emplace(property->GetName(), i);

		property->DataPtr = (void*)rawAddr;

		rawAddr += PMath::Align<int64>((int64)type->GetSize(), JG_MEMORY_OFFSET);
	}

	int32 functionCount = (int32)staticStruct->Functions.size();

	for (int32 i = 0; i < functionCount; ++i)
	{
		PSharedPtr<JGFunction> function = staticStruct->Functions[i];

		staticStruct->FunctionMap.emplace(function->GetName(), i);
	}

	return staticStruct;
}
