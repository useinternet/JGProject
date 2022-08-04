#include "ObjectGlobalSystem.h"
#include "ObjectGlobals.h"
#include "Misc/Log.h"




GObjectGlobalSystem::GObjectGlobalSystem()
{

}

GObjectGlobalSystem::~GObjectGlobalSystem()
{

}

void GObjectGlobalSystem::Destroy()
{
	_enumMap.clear();
	_structMap.clear();
	_classMap.clear();
	_typeIDMap.clear();
}

PSharedPtr<JGStruct> GObjectGlobalSystem::GetStaticStruct(uint64 typeID) const
{
	if (_structMap.find(typeID) == _structMap.end())
	{
		return nullptr;
	}

	return _structMap.at(typeID);
}

bool GObjectGlobalSystem::registerClass(PSharedPtr<JGClass> classObject)
{
	if (classObject.IsValid() == false)
	{
		return false;
	}

	PSharedPtr<JGType> classType = classObject->GetType();
	if (classType.IsValid() == false)
	{
		JG_LOG(Core, ELogLevel::Critical, "{0} : Invalid Type", classObject->GetName());
		return false;
	}

	if (registerType(classType) == false)
	{
		return false;
	}

	const uint64 typeID = classType->GetID();
	_classMap.emplace(typeID, classObject);

	return true;
}

bool GObjectGlobalSystem::registerStruct(PSharedPtr<JGStruct> structObject)
{
	if (structObject.IsValid() == false)
	{
		return false;
	}

	PSharedPtr<JGType> structType = structObject->GetType();
	if (structType.IsValid() == false)
	{
		JG_LOG(Core, ELogLevel::Critical, "{0} : Invalid Type", structObject->GetName());
		return false;
	}

	if (registerType(structType) == false)
	{
		return false;
	}

	const uint64 typeID = structType->GetID();
	_structMap.emplace(typeID, structObject);

	return true;
}

bool GObjectGlobalSystem::registerEnum(PSharedPtr<JGEnum> enumObject)
{
	if (enumObject.IsValid() == false)
	{
		return false;
	}

	PSharedPtr<JGType> enumType = enumObject->GetType();
	if (enumType.IsValid() == false)
	{
		JG_LOG(Core, ELogLevel::Critical, "{0} : Invalid Type", enumObject->GetName());
		return false;
	}

	if (registerType(enumType) == false)
	{
		return false;
	}

	const uint64 typeID = enumType->GetID();
	_enumMap.emplace(typeID, enumObject);

	return true;
}

bool GObjectGlobalSystem::registerType(PSharedPtr<JGType> type)
{
	if (type.IsValid() == false)
	{
		return false;
	}

	const PString& typeName   = type->GetName();
	const uint64   typeID     = type->GetID();

	if (_typeIDMap.find(typeName) != _typeIDMap.end())
	{
		JG_LOG(Core, ELogLevel::Critical, "Duplicate Type Name : {0}", typeName);
		return false;
	}

	_typeIDMap.emplace(typeName, typeID);

	return true;
}

PSharedPtr<JGStruct> GObjectGlobalSystem::copyStruct(uint64 typeID)
{
	PSharedPtr<JGStruct> staticStruct = GetStaticStruct(typeID);
	if (staticStruct == nullptr)
	{
		return nullptr;
	}

	PSharedPtr<JGStruct> result = Allocate<JGStruct>();
	result->Type     = staticStruct->Type;
	result->MetaData = staticStruct->MetaData;

	for (const PSharedPtr<JGProperty>& property : staticStruct->Properties)
	{
		result->Properties.push_back(Allocate<JGProperty>(*property));
	}

	for (const PSharedPtr<JGFunction>& function : staticStruct->Functions)
	{
		result->Functions.push_back(Allocate<JGFunction>(*function));
	}

	result->SetName(staticStruct->GetName());

	return result;
}
