#include "PCH/PCH.h"
#include "ObjectGlobals.h"
#include "Misc/Log.h"
#include "ObjectGlobalSystem.h"


PSharedPtr<JGMeta> PObjectGlobalsPrivateUtils::MakeStaticMeta(const HList<HPair<PName, HHashSet<PName>>>& pairList)
{
	PSharedPtr<JGMeta> result = Allocate<JGMeta>();
	for (const HPair<PName, HHashSet<PName>>& pair : pairList)
	{
		result->MetaDataMap.emplace(pair.first, pair.second);
	}
	
	return result;
}

PSharedPtr<JGProperty> PObjectGlobalsPrivateUtils::MakeStaticProperty(const JGType& type, const PString& name, PSharedPtr<JGMeta> metaData)
{
	PSharedPtr<JGProperty> result = Allocate<JGProperty>();
	result->Type     = Allocate(type);
	result->MetaData = metaData;
	result->SetName(name);

	return result;
}

PSharedPtr<JGFunction> PObjectGlobalsPrivateUtils::MakeStaticFunction(const PString& name, const JGType& returnType, const HList<JGType>& args, PSharedPtr<JGMeta> metaData)
{
	PSharedPtr<JGFunction> result = Allocate<JGFunction>();
	result->Return    = returnType;
	result->Arguments = args;
	result->MetaData  = metaData;
	result->SetName(name);

	return result;
}

PSharedPtr<JGClass> PObjectGlobalsPrivateUtils::MakeStaticClass(const JGType& type, const HList<JGType>& virtualTypeList, const HList<PSharedPtr<JGProperty>>& properties, const HList<PSharedPtr<JGFunction>>& functions, PSharedPtr<JGMeta> metaData)
{
	PSharedPtr<JGClass> result = Allocate<JGClass>();
	result->Type = Allocate<JGType>(type);
	result->Properties = properties;
	result->Functions  = functions;
	result->MetaData   = metaData;

	for (const JGType& type : virtualTypeList)
	{
		result->VTypeSet.insert(type);
	}

	return result;
}

PSharedPtr<JGEnum> PObjectGlobalsPrivateUtils::MakeStaticEnum(const JGType& type, const PString& name, const HHashMap<int32, int32>& enumValueRedirectMap, const HList<PName>& enumElementNames, const HList<PSharedPtr<JGMeta>>& metas)
{
	PSharedPtr<JGEnum> Enum = Allocate<JGEnum>();
	Enum->Type = Allocate<JGType>(type);
	Enum->ElementMetaDatas = metas;
	Enum->ElementNames = enumElementNames;
	Enum->EnumValueRedirectMap = enumValueRedirectMap;
	return Enum;
}

JGType::JGType()
{}

uint64 JGType::GetID() const
{
	return ID;
}

uint64 JGType::GetSize() const
{
	return Size;
}

bool JGMeta::HasMeta(const PName& key) const
{
	if (MetaDataMap.contains(key) == false)
	{
		return false;
	}

	return true;
}

bool JGMeta::HasMeta(const PName& key, const PName& data) const
{
	if (MetaDataMap.contains(key) == false)
	{
		return false;
	}

	return MetaDataMap.at(key).contains(data);
}

bool JGMeta::GetMetaValues(const PName& key, HHashSet<PName>& outDatas) const
{
	if (MetaDataMap.contains(key) == false)
	{
		return false;
	}

	outDatas = MetaDataMap.at(key);

	return true;
}

const HHashMap<PName, HHashSet<PName>>& JGMeta::GetMetaDatas() const
{
	return MetaDataMap;
}

JGProperty::JGProperty()
{}
bool JGProperty::IsValid() const
{
	if (OwnerObject.IsValid() == false)
	{
		return false;
	}

	if (DataPtr == nullptr)
	{
		return false;
	}

	return true;
}
const JGType& JGProperty::GetPropertyType() const
{
	if (Type == nullptr)
	{
		static JGType nullType = JGType();
		return nullType;
	}
	return *Type;
}

PSharedPtr<JGMeta> JGProperty::GetMeta() const
{
	return MetaData;
}

JGFunction::JGFunction() {}

bool JGFunction::IsBound() const
{
	if (FunctionReference == nullptr || FunctionReference->IsBound() == false)
	{
		return false;
	}

	return true;
}

bool JGFunction::checkArgsType(const HList<JGType>& compareArgsList)
{
	int32 count = (int32)Arguments.size();
	if (count > compareArgsList.size())
	{
		return false;
	}

	for (int32 i = 0; i < count; ++i)
	{
		if (Arguments[i] != compareArgsList[i])
		{
			return false;
		}
	}

	return true;
}

bool JGFunction::checkRetType(JGType compareRetType)
{
	if (Return != compareRetType)
	{
		return false;
	}

	return true;
}

JGField::JGField() {}

bool JGField::HasProperty(const PName& name) const
{
	if (PropertyMap.find(name) == PropertyMap.end())
	{
		return false;
	}

	return true;
}

PSharedPtr<JGProperty> JGField::FindProperty(const PName& name) const
{
	if (HasProperty(name) == false)
	{
		return nullptr;
	}

	uint64 index = PropertyMap.at(name);

	return Properties[index];
}

bool JGField::HasFunction(const PName& name) const
{
	if (FunctionMap.find(name) == FunctionMap.end())
	{
		return false;
	}

	return true;
}

PSharedPtr<JGFunction> JGField::FindFunction(const PName& name) const
{
	if (HasFunction(name) == false)
	{
		return nullptr;
	}

	uint64 index = FunctionMap.at(name);

	return Functions[index];
}

const HList<PSharedPtr<JGProperty>>& JGField::GetPropertyList() const
{
	return Properties;
}

const HList<PSharedPtr<JGFunction>>& JGField::GetFunctionList() const
{
	return Functions;
}

JGEnum::JGEnum() {}

PName JGEnum::GetEnumNameByIndex(int32 index) const
{
	if (ElementNames.size() <= index || index == INDEX_NONE)
	{
		return NAME_NONE;
	}

	return ElementNames[index];
}

PName JGEnum::GetEnumNameByValue(int32 value) const
{
	int32 index = GetIndexByValue(value);
	return GetEnumNameByIndex(index);
}

PSharedPtr<JGMeta> JGEnum::GetMetaDataByIndex(int32 index) const
{
	if (ElementNames.size() <= index || index == INDEX_NONE)
	{
		return nullptr;
	}

	return ElementMetaDatas[index];
}

PSharedPtr<JGMeta> JGEnum::GetMetaDataByValue(int32 value) const
{
	int32 index = GetIndexByValue(value);
	return GetMetaDataByIndex(index);
}

PSharedPtr<JGType> JGEnum::GetEnumType() const
{
	return Type;
}

int32 JGEnum::GetIndexByValue(int32 value) const
{
	if (EnumValueRedirectMap.contains(value))
	{
		return EnumValueRedirectMap.at(value);
	}

	return INDEX_NONE;
}

JGClass::JGClass() {}
PSharedPtr<JGType> JGClass::GetClassType() const
{
	return Type;
}