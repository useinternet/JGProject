#pragma once

#include "JGObject.h"
#include "JGType.h"
#include "Math/Math.h"
#include "Memory/Memory.h"
#include "String/String.h"
#include "String/Name.h"
#include "Misc/Delegate.h"
#include "Misc/StdExternal.h"

class JGMeta;
class JGProperty;
class JGField;
class JGType;
class JGFunction;
class JGStruct;
class GObjectGlobalSystem;
class JGClass;
class JGEnum;

class PObjectGlobalsPrivateUtils
{
	using address = uint64;
public:
	static PSharedPtr<JGMeta>     MakeStaticMeta(const HList<HPair<PName, HHashSet<PName>>>& pairList);
	static PSharedPtr<JGProperty> MakeStaticProperty(const JGType& type, const PString& name, PSharedPtr<JGMeta> metaData = nullptr);
	static PSharedPtr<JGFunction> MakeStaticFunction(const PString& name, const JGType& returnType, const HList<JGType>& args = HList<JGType>(), PSharedPtr<JGMeta> metaData = nullptr);
	static PSharedPtr<JGClass>    MakeStaticClass(const JGType& type, const HList<JGType>& virtualTypeList, const HList<PSharedPtr<JGProperty>>& properties, const HList<PSharedPtr<JGFunction>>& functions, PSharedPtr<JGMeta> metaData = nullptr);
	static PSharedPtr<JGEnum> MakeStaticEnum(const JGType& type, const PString& name, const HHashMap<int32, int32>& enumValueRedirectMap, const HList<PName>& enumElementNames, const HList<PSharedPtr<JGMeta>>& metas);

	template<class T>
	static PSharedPtr<JGClass> MakeClass(const T* fromThis, PSharedPtr<JGClass> staticClass);

	template<class T, class Ret, class ... Args>
	static bool BindFunction(const T* fromThis, PSharedPtr<JGFunction> function, const std::function<Ret(Args...)>& func, const HList<JGType>& funArgTypes);

	template<class T, class U>
	static bool BindProperty(const T* fromThis, PSharedPtr<JGProperty> inProperty, U* value);
};

class JGMeta : public JGObject
{
	JG_GENERATED_SIMPLE_BODY
	friend PObjectGlobalsPrivateUtils;
	friend GObjectGlobalSystem;
protected:
	HHashMap<PName, HHashSet<PName>> MetaDataMap;

public:
	bool HasMeta(const PName& key) const;
	bool HasMeta(const PName& key, const PName& data) const;
	bool GetMetaValues(const PName& Key, HHashSet<PName>& outDatas) const;

	const HHashMap<PName, HHashSet<PName>>& GetMetaDatas() const;
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

		if (*Type != JGTYPE(T))
		{
			return false;
		}

		uint64 dataSize = Type->GetSize();
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

		if (*Type != JGTYPE(T))
		{
			return false;
		}

		uint64 dataSize = Type->GetSize();

		outData = *((T*)(DataPtr));

		return true;
	}

	PSharedPtr<JGMeta> GetMeta() const;
};

class JGFunction : public JGObject
{
	JG_GENERATED_SIMPLE_BODY
	friend PObjectGlobalsPrivateUtils;
	friend GObjectGlobalSystem;

protected:
	PSharedPtr<JGMeta> MetaData;
	JGType Return;
	HList<JGType> Arguments;

	PWeakPtr<JGObject> OwnerObject;
	PSharedPtr<IDelegateInstanceBase> FunctionReference;

public:
	JGFunction();
	virtual ~JGFunction() = default;

	bool IsBound() const;

	template<class Ret, class ... Args>
	Ret Invoke(Args ... args)
	{
		if (FunctionReference == nullptr)
		{
			return Ret();
		}

		if (FunctionReference->GetDelegateInstanceType() != JGType::GenerateType<IDelegateInstance<Ret, Args...>>())
		{
			return Ret();
		}

		IDelegateInstance<Ret, Args...>* pFuncRef = static_cast<IDelegateInstance<Ret, Args...>*>(FunctionReference.GetRawPointer());
		if (pFuncRef->IsBound() == false)
		{
			return Ret();
		}

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

class JGEnum : public JGObject
{
	JG_GENERATED_SIMPLE_BODY
	friend PObjectGlobalsPrivateUtils;
	friend GObjectGlobalSystem;
protected:
	PSharedPtr<JGType>  Type;
	HList<PSharedPtr<JGMeta>>  ElementMetaDatas;
	HList<PName> ElementNames;
	HHashMap<int32, int32> EnumValueRedirectMap;
public:
	JGEnum();
	virtual ~JGEnum() = default;

public:
	PName GetEnumNameByIndex(int32 index) const;
	PName GetEnumNameByValue(int32 value) const;
	PSharedPtr<JGMeta> GetMetaDataByIndex(int32 index) const;
	PSharedPtr<JGMeta> GetMetaDataByValue(int32 value) const;
	PSharedPtr<JGType> GetEnumType() const;

	int32 GetIndexByValue(int32 value) const;
};

/* JGClass
* 프로퍼티 정보만 저장 / 관리 ( Struct 기능 )
* 함수 Invoke 기능 추가 -> 함수 저장 / 관리
* VTable 관리 -> VTable에는 Class, Interface만 상속할 수 있음.
* 상속 검사 시 중복 상속 시 에러 검출
*/
class JGClass : public JGField
{
	JG_GENERATED_SIMPLE_BODY
	friend PObjectGlobalsPrivateUtils;
	friend GObjectGlobalSystem;
protected:
	PSharedPtr<JGType>  Type;
	PSharedPtr<JGMeta>  MetaData;

	HHashSet<JGType> VTypeSet; // 1차적으로 상속받은 타입들

public:
	JGClass();
	virtual ~JGClass() = default;

	PSharedPtr<JGType> GetClassType() const;
	HList<PSharedPtr<JGClass>> GetChildClasses(bool bRecursive = false) const;
};

template<class T>
inline PSharedPtr<JGClass> PObjectGlobalsPrivateUtils::MakeClass(const T* fromThis, PSharedPtr<JGClass> staticClass)
{
	if (fromThis == nullptr)
	{
		return nullptr;
	}

	address rawAddr = (address)fromThis;

	int32 propertyCount = (int32)staticClass->Properties.size();

	for (int32 i = 0; i < propertyCount; ++i)
	{
		PSharedPtr<JGProperty> property = staticClass->Properties[i];
		PSharedPtr<JGType>	   type = property->Type;

		staticClass->PropertyMap.emplace(property->GetName(), i);
	}

	int32 functionCount = (int32)staticClass->Functions.size();

	for (int32 i = 0; i < functionCount; ++i)
	{
		PSharedPtr<JGFunction> function = staticClass->Functions[i];
		staticClass->FunctionMap.emplace(function->GetName(), i);
	}

	return staticClass;
}

template<class T, class Ret, class ...Args>
inline bool PObjectGlobalsPrivateUtils::BindFunction(const T* fromThis, PSharedPtr<JGFunction> function, const std::function<Ret(Args...)>& func, const HList<JGType>& funArgTypes)
{
	if (fromThis == nullptr || function == nullptr)
	{
		return false;
	}

	if (function->checkRetType(JGTYPE(Ret)) == false)
	{
		return false;
	}

	if (function->checkArgsType(funArgTypes) == false)
	{
		return false;
	}

	function->FunctionReference = Allocate(PDelegateInstance<T, Ret, Args...>::Create(fromThis, func));

	return true;
}

template<class T, class U>
inline bool PObjectGlobalsPrivateUtils::BindProperty(const T* fromThis, PSharedPtr<JGProperty> inProperty, U* value)
{
	if (fromThis == nullptr || inProperty == nullptr || value == nullptr)
	{
		return false;
	}

	if (inProperty->GetPropertyType() != JGTYPE(U))
	{
		return false;
	}

	inProperty->OwnerObject = SharedWrap<T>(fromThis);
	inProperty->DataPtr = value;

	return true;
}