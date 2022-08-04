#include "ObjectGlobals.h"
#include "Misc/Log.h"


PSharedPtr<JGMeta> PObjectGlobalsPrivateUtils::MakeStaticMeta(const PList<PPair<PString, PString>>& pairList)
{
	PSharedPtr<JGMeta> result = Allocate<JGMeta>();
	for (const PPair<PString, PString>& pair : pairList)
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
JGFunction::JGFunction()
{
	SetName("JGFunction");
}
JGField::JGField()
{
	SetName("JGField");
}

JGStruct::JGStruct()
{
	SetName("JGStruct");
}

PSharedPtr<JGType> JGStruct::GetType() const
{
	return Type;
}

JGEnum::JGEnum()
{
	SetName("JGEnum");
}

PSharedPtr<JGType> JGEnum::GetType() const
{
	return Type;
}

JGClass::JGClass()
{
	SetName("JGEnum");
}


