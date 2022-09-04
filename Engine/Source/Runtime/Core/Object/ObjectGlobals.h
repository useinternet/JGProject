#pragma once

#include "JGObject.h"
#include "Math/Math.h"
#include "Memory/Memory.h"
#include "String/String.h"
#include "String/Name.h"
#include "Misc/Delegate.h"
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
	static PSharedPtr<JGMeta>     MakeStaticMeta(const HList<HPair<PName, PString>>& pairList);
	static PSharedPtr<JGProperty> MakeStaticProperty(const JGType& type, const PString& name, PSharedPtr<JGMeta> metaData = nullptr);
	static PSharedPtr<JGFunction> MakeStaticFunction(const PString& name, PSharedPtr<JGProperty> returnProperty, const HList<PSharedPtr<JGProperty>>& args = HList<PSharedPtr<JGProperty>>(), PSharedPtr<JGMeta> metaData = nullptr);
	static PSharedPtr<JGStruct>   MakeStaticStruct(const JGType& type, const HList<PSharedPtr<JGProperty>>& properties, const HList<PSharedPtr<JGFunction>>& functions, PSharedPtr<JGMeta> metaData = nullptr);
	static PSharedPtr<JGClass>    MakeStaticClass(const JGType& type, const HList<JGType>& virtualTypeList, const HList<PSharedPtr<JGProperty>>& properties, const HList<PSharedPtr<JGFunction>>& functions, PSharedPtr<JGMeta> metaData = nullptr);
	static PSharedPtr<JGInterface> MakeStaticInterface(const JGType& type, const HList<JGType>& virtualTypeList, const HList<PSharedPtr<JGFunction>>& functions, PSharedPtr<JGMeta> metaData = nullptr);

	template<class T>
	static PSharedPtr<JGStruct> MakeStruct(const T* fromThis, PSharedPtr<JGStruct> staticStruct);

	template<class T, class Ret, class ... Args>
	static bool BindFunction(const T* fromThis, PSharedPtr<JGFunction> function, const std::function<Ret(Args...)>& func);
};

class JGType : public JGObject
{
	JG_GENERATED_SIMPLE_BODY
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
		result.ID   = GenerateTypeID<T>();
		result.Size = sizeof(T);
		result.SetName(PName(type_name));
		
		return result;
	}

	template<>
	static JGType GenerateType<void>()
	{
		static const char* type_name = typeid(void).name();

		JGType result;
		result.ID = GenerateTypeID<void>();
		result.Size = 0;
		result.SetName(PName(type_name));

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
	JG_GENERATED_SIMPLE_BODY
	friend PObjectGlobalsPrivateUtils;
	friend GObjectGlobalSystem;
protected:
	HHashMap<PName, PString> MetaDataMap;
};

class JGProperty : public JGObject
{
	JG_GENERATED_SIMPLE_BODY
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

	const JGType& GetPropertyType() const;

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
	JG_GENERATED_SIMPLE_BODY
	friend PObjectGlobalsPrivateUtils;
	friend GObjectGlobalSystem;

protected:
	PSharedPtr<JGMeta>			  MetaData;
	PSharedPtr<JGProperty>		  Return;
	HList<PSharedPtr<JGProperty>> Arguments;

	PWeakPtr<JGObject> OwnerObject;
	PSharedPtr<IDelegateInstanceBase> FunctionReference;

public:
	JGFunction();
	virtual ~JGFunction() = default;

	bool IsBound() const;

	template<class Ret, class ... Args>
	Ret Invoke(Args ... args)
	{
		IDelegateInstance<Ret, Args...>* pFuncRef = static_cast<IDelegateInstance<Ret, Args...>*>(FunctionReference.GetRawPointer());

		return pFuncRef->Execute(args...);
	}

private:
	bool checkArgsType(const HList<JGType>& compareArgsList);
	bool checkRetType(JGType compareRetType);

};

class JGField : public JGObject
{
	friend PObjectGlobalsPrivateUtils;
	friend GObjectGlobalSystem;
protected:
	HList<PSharedPtr<JGProperty>> Properties;
	HList<PSharedPtr<JGFunction>> Functions;

	HHashMap<PName, uint64> PropertyMap;
	HHashMap<PName, uint64> FunctionMap;

public:
	JGField();
	virtual ~JGField() = default;

public:
	bool HasProperty(const PName& name) const;

	PSharedPtr<JGProperty> FindProperty(const PName& name) const;

	template<class T>
	bool SetPropertyValue(const PName& name, const T& data)
	{
		PSharedPtr<JGProperty> property = FindProperty(name);
		if (property == nullptr)
		{
			return false;
		}

		return property->SetValue(data);
	}

	template<class T>
	bool GetPropertyValue(const PName& name, T& outData) const
	{
		PSharedPtr<JGProperty> property = FindProperty(name);
		if (property == nullptr)
		{
			return false;
		}

		return property->GetValue(outData);
	}

	bool HasFunction(const PName& name) const;

	PSharedPtr<JGFunction> FindFunction(const PName& name) const;

	const HList<PSharedPtr<JGProperty>>& GetPropertyList() const;
	const HList<PSharedPtr<JGFunction>>& GetFunctionList() const;
};

/* Struct 정보
* 기본적으로 프로퍼티 정보만 저장 / 관리
* 함수는 메타 정보 / 이름 정보만 
*/
class JGStruct : public JGField
{
	JG_GENERATED_SIMPLE_BODY
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
	JG_GENERATED_SIMPLE_BODY
	friend PObjectGlobalsPrivateUtils;
	friend GObjectGlobalSystem;
protected:
	PSharedPtr<JGType>  Type;
	HHashMap<uint64, PSharedPtr<JGMeta>>  MetaDataMap;
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
	JG_GENERATED_SIMPLE_BODY
	friend PObjectGlobalsPrivateUtils;
	friend GObjectGlobalSystem;
protected:
	HHashSet<JGType> VTypeSet; // 1차적으로 상속받은 타입들

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
	JG_GENERATED_SIMPLE_BODY
	friend PObjectGlobalsPrivateUtils;
	friend GObjectGlobalSystem;

public:
	HHashSet<JGType> VTypeSet; // 1차적으로 상속받은 타입들

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

		rawAddr += HMath::Align<int64>((int64)type->GetSize(), JG_MEMORY_OFFSET);
	}

	int32 functionCount = (int32)staticStruct->Functions.size();

	for (int32 i = 0; i < functionCount; ++i)
	{
		PSharedPtr<JGFunction> function = staticStruct->Functions[i];
		staticStruct->FunctionMap.emplace(function->GetName(), i);
	}

	return staticStruct;
}

template<class T, class Ret, class ...Args>
inline bool PObjectGlobalsPrivateUtils::BindFunction(const T* fromThis, PSharedPtr<JGFunction> function, const std::function<Ret(Args...)>& func)
{
	if (fromThis == nullptr || function == nullptr)
	{
		return false;
	}

	if (function->checkRetType(JGTYPE(Ret)) == false)
	{
		return false;
	}

	if (function->checkArgsType({ JGTYPE(Args)... }) == false)
	{
		return false;
	}

	function->FunctionReference = Allocate(PDelegateInstance<T, Ret, Args...>::Create(fromThis, func));

	return true;
}
