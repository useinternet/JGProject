#pragma once

#include "JGObject.h"
#include "Math/Math.h"
#include "Memory/Memory.h"
#include "String/String.h"
#include "String/Name.h"
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
class JGClass;
class JGInterface;

class PObjectGlobalsPrivateUtils
{
	using address = uint64;
public:
	static PSharedPtr<JGMeta>     MakeStaticMeta(const PList<PPair<PName, PString>>& pairList);
	static PSharedPtr<JGProperty> MakeStaticProperty(const JGType& type, const PString& name, PSharedPtr<JGMeta> metaData = nullptr);
	static PSharedPtr<JGFunction> MakeStaticFunction(const PString& name, PSharedPtr<JGProperty> returnProperty, const PList<PSharedPtr<JGProperty>>& args = PList<PSharedPtr<JGProperty>>(), PSharedPtr<JGMeta> metaData = nullptr);
	static PSharedPtr<JGStruct>   MakeStaticStruct(const JGType& type, const PList<PSharedPtr<JGProperty>>& properties, const PList<PSharedPtr<JGFunction>>& functions, PSharedPtr<JGMeta> metaData = nullptr);
	static PSharedPtr<JGClass>    MakeStaticClass(const JGType& type, const PList<JGType>& virtualTypeList, const PList<PSharedPtr<JGProperty>>& properties, const PList<PSharedPtr<JGFunction>>& functions, PSharedPtr<JGMeta> metaData = nullptr);
	static PSharedPtr<JGInterface> MakeStaticInterface(const JGType& type, const PList<JGType>& virtualTypeList, const PList<PSharedPtr<JGFunction>>& functions, PSharedPtr<JGMeta> metaData = nullptr);

	template<class T>
	static PSharedPtr<JGStruct> MakeStruct(const T* fromThis, PSharedPtr<JGStruct> staticStruct);
};



class JGType : public JGObject
{
	GENERATED_SIMPLE_BODY
	friend PObjectGlobalsPrivateUtils;
	friend GObjectGlobalSystem;
protected:
	uint64 ID = TYPE_NULL_ID;
	uint64 Size = 0;
public:
	JGType();
	virtual ~JGType() = default;
public:
	bool operator==(const JGType& type) const
	{
		return GetID() == type.GetID();
	}
	bool operator!=(const JGType& type) const 
	{
		return GetID() != type.GetID();
	}
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

	template<class T>
	static JGType GenerateType(T* _this)
	{
		return GenerateType<T>();
	}
};

namespace std {
	template <>
	struct hash<JGType>
	{
		std::size_t operator()(const JGType& k) const noexcept
		{
			return k.GetID();
		}
	};
}

class JGMeta : public JGObject
{
	GENERATED_SIMPLE_BODY
	friend PObjectGlobalsPrivateUtils;
	friend GObjectGlobalSystem;
protected:
	PHashMap<PName, PString> MetaDataMap;
};

class JGProperty : public JGObject
{
	GENERATED_SIMPLE_BODY
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

public:
	bool IsValid() const;

	template<class T>
	bool SetValue(const T& data)
	{
		if (IsValid() == false)
		{
			return false;
		}

		if (Type != JGTYPE(T))
		{
			return false;
		}

		uint64 dataSize = Type->Size();
		memcpy_s(DataPtr, dataSize, &data, dataSize);

		return true;
	}

	template<class T>
	bool GetValue(T& outData) const
	{
		if (IsValid() == false)
		{
			return false;
		}

		if (Type != JGTYPE(T))
		{
			return false;
		}

		uint64 dataSize = Type->Size();
		memcpy_s(&outData, dataSize, DataPtr, dataSize);

		return true;
	}
};

class JGFunction : public JGObject
{
	GENERATED_SIMPLE_BODY
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

	PHashMap<PName, uint64> PropertyMap;
	PHashMap<PName, uint64> FunctionMap;

public:
	JGField();
	virtual ~JGField() = default;

public:
	bool HasProperty(const PName& name) const;

	template<class T>
	bool SetPropertyValue(const PName& name, const T& data)
	{
		PSharedPtr<JGProperty> property = findProperty(name);
		if (property == nullptr)
		{
			return false;
		}

		return property->SetValue(data);
	}

	template<class T>
	bool GetPropertyValue(const PName& name, T& outData) const
	{
		PSharedPtr<JGProperty> property = findProperty(name);
		if (property == nullptr)
		{
			return false;
		}

		return property->GetValue(outData);
	}

private:
	PSharedPtr<JGProperty> findProperty(const PName& name) const;
};

/* Struct 정보
* 기본적으로 프로퍼티 정보만 저장 / 관리
* 함수는 메타 정보 / 이름 정보만 
*/
class JGStruct : public JGField
{
	GENERATED_SIMPLE_BODY
	friend PObjectGlobalsPrivateUtils;
	friend GObjectGlobalSystem;
protected:
	PSharedPtr<JGType>  Type;
	PSharedPtr<JGMeta>  MetaData;

public:
	JGStruct();
	virtual ~JGStruct() = default;

public:
	PSharedPtr<JGType> GetClassType() const;
};

class JGEnum : public JGObject
{
	GENERATED_SIMPLE_BODY
	friend PObjectGlobalsPrivateUtils;
	friend GObjectGlobalSystem;
protected:
	PSharedPtr<JGType>  Type;
	PHashMap<uint64, PSharedPtr<JGMeta>>  MetaDataMap;
public:
	JGEnum();
	virtual ~JGEnum() = default;

public:
	PSharedPtr<JGType> GetEnumType() const;
};

/* JGClass
* 프로퍼티 정보만 저장 / 관리 ( Struct 기능 )
* 함수 Invoke 기능 추가 -> 함수 저장 / 관리
* VTable 관리 -> VTable에는 Class, Interface만 상속할 수 있음.
* 상속 검사 시 중복 상속 시 에러 검출
*/
class JGClass : public JGStruct
{
	GENERATED_SIMPLE_BODY
	friend PObjectGlobalsPrivateUtils;
	friend GObjectGlobalSystem;
protected:
	PList<PWeakPtr<JGStruct>> VList;
	PHashSet<JGType> VTypeSet;

public:
	JGClass();
	virtual ~JGClass() = default;
};

/* JGInterface
* 함수 Invoke 기능 추가 -> 함수 저장 / 관리
* VTable 관리 -> Interface만 상속 가능, 다른 것이 상속되어져 잇으면 오류 검출
* 상속 검사 시 중복 상속 시 에러 검출
*/
class JGInterface : public JGStruct
{
	GENERATED_SIMPLE_BODY
	friend PObjectGlobalsPrivateUtils;
	friend GObjectGlobalSystem;
public:
	PList<PWeakPtr<JGStruct>> VList;
	PHashSet<JGType> VTypeSet;

public:
	JGInterface();
	virtual ~JGInterface() = default;
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
