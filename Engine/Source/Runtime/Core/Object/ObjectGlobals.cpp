#include "ObjectGlobals.h"
#include "Misc/Log.h"
#include "ObjectGlobalSystem.h"


PSharedPtr<JGMeta> PObjectGlobalsPrivateUtils::MakeStaticMeta(const HList<HPair<PName, HHashSet<PString>>>& pairList)
{
	PSharedPtr<JGMeta> result = Allocate<JGMeta>();
	for (const HPair<PName, HHashSet<PString>>& pair : pairList)
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

PSharedPtr<JGFunction> PObjectGlobalsPrivateUtils::MakeStaticFunction(const PString& name, PSharedPtr<JGProperty> returnProperty, const HList<PSharedPtr<JGProperty>>& args, PSharedPtr<JGMeta> metaData)
{
	PSharedPtr<JGFunction> result = Allocate<JGFunction>();
	result->Return    = returnProperty;
	result->Arguments = args;
	result->MetaData  = metaData;
	result->SetName(name);

	return result;
}

PSharedPtr<JGStruct> PObjectGlobalsPrivateUtils::MakeStaticStruct(const JGType& type, const HList<PSharedPtr<JGProperty>>& properties, const HList<PSharedPtr<JGFunction>>& functions, PSharedPtr<JGMeta> metaData)
{
	PSharedPtr<JGStruct> result = Allocate<JGStruct>();
	result->Type = Allocate<JGType>(type);
	result->Properties = properties;
	result->Functions  = functions;
	result->MetaData   = metaData;

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

PSharedPtr<JGInterface> PObjectGlobalsPrivateUtils::MakeStaticInterface(const JGType& type, const HList<JGType>& virtualTypeList, const HList<PSharedPtr<JGFunction>>& functions, PSharedPtr<JGMeta> metaData)
{
	PSharedPtr<JGInterface> result = Allocate<JGInterface>();
	result->Type = Allocate<JGType>(type);
	result->Functions = functions;
	result->MetaData  = metaData;

	for (const JGType& type : virtualTypeList)
	{
		result->VTypeSet.insert(type);
	}

	return result;
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
	for (int32 i = 0; i < count; ++i)
	{
		if (Arguments[i]->GetPropertyType() != compareArgsList[i])
		{
			return false;
		}
	}

	return true;
}

bool JGFunction::checkRetType(JGType compareRetType)
{
	if (Return->GetPropertyType() != compareRetType)
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

JGStruct::JGStruct() {}

PSharedPtr<JGType> JGStruct::GetClassType() const
{
	return Type;
}

JGEnum::JGEnum() {}

PSharedPtr<JGType> JGEnum::GetEnumType() const
{
	return Type;
}

JGClass::JGClass() {}

JGInterface::JGInterface() {}

