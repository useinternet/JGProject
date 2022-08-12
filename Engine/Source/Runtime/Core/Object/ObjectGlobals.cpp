#include "ObjectGlobals.h"
#include "Misc/Log.h"
#include "ObjectGlobalSystem.h"


PSharedPtr<JGMeta> PObjectGlobalsPrivateUtils::MakeStaticMeta(const PList<PPair<PName, PString>>& pairList)
{
	PSharedPtr<JGMeta> result = Allocate<JGMeta>();
	for (const PPair<PName, PString>& pair : pairList)
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

PSharedPtr<JGFunction> PObjectGlobalsPrivateUtils::MakeStaticFunction(const PString& name, PSharedPtr<JGProperty> returnProperty, const PList<PSharedPtr<JGProperty>>& args, PSharedPtr<JGMeta> metaData)
{
	PSharedPtr<JGFunction> result = Allocate<JGFunction>();
	result->Return    = returnProperty;
	result->Arguments = args;
	result->MetaData  = metaData;
	result->SetName(name);

	return result;
}

PSharedPtr<JGStruct> PObjectGlobalsPrivateUtils::MakeStaticStruct(const JGType& type, const PList<PSharedPtr<JGProperty>>& properties, const PList<PSharedPtr<JGFunction>>& functions, PSharedPtr<JGMeta> metaData)
{
	PSharedPtr<JGStruct> result = Allocate<JGStruct>();
	result->Type = Allocate<JGType>(type);
	result->Properties = properties;
	result->Functions  = functions;
	result->MetaData   = metaData;

	return result;
}

PSharedPtr<JGClass> PObjectGlobalsPrivateUtils::MakeStaticClass(const JGType& type, const PList<JGType>& virtualTypeList, const PList<PSharedPtr<JGProperty>>& properties, const PList<PSharedPtr<JGFunction>>& functions, PSharedPtr<JGMeta> metaData)
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

PSharedPtr<JGInterface> PObjectGlobalsPrivateUtils::MakeStaticInterface(const JGType& type, const PList<JGType>& virtualTypeList, const PList<PSharedPtr<JGFunction>>& functions, PSharedPtr<JGMeta> metaData)
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
{
	SetName("JGType");
}

uint64 JGType::GetID() const
{
	return ID;
}

uint64 JGType::GetSize() const
{
	return Size;
}

JGProperty::JGProperty()
{
	SetName("JGProperty");
}
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
JGFunction::JGFunction()
{
	SetName("JGFunction");
}

JGField::JGField()
{
	SetName("JGField");
}

bool JGField::HasProperty(const PName& name) const
{
	if (PropertyMap.find(name) == PropertyMap.end())
	{
		return false;
	}

	return true;
}

PSharedPtr<JGProperty> JGField::findProperty(const PName& name) const
{
	if (HasProperty(name) == false)
	{
		return nullptr;
	}

	uint64 index = PropertyMap.at(name);
	return Properties[index];
}



JGStruct::JGStruct()
{
	SetName("JGStruct");
}

PSharedPtr<JGType> JGStruct::GetClassType() const
{
	return Type;
}

JGEnum::JGEnum()
{
	SetName("JGEnum");
}

PSharedPtr<JGType> JGEnum::GetEnumType() const
{
	return Type;
}

JGClass::JGClass()
{
	SetName("JGClass");
}

JGInterface::JGInterface()
{
	SetName("JGInterface");
}

