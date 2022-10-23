#include "ObjectGlobalSystem.h"
#include "ObjectGlobals.h"
#include "Misc/Log.h"
#include "Platform/Platform.h"

void GObjectGlobalSystem::Destroy()
{
	_enumMap.clear();
	_classMap.clear();
	_typeMap.clear();
}

PSharedPtr<JGClass> GObjectGlobalSystem::GetStaticClass(const JGType& type) const
{
	if (_classMap.find(type) == _classMap.end())
	{
		return nullptr;
	}

	return _classMap.at(type);
}

PSharedPtr<JGInterface> GObjectGlobalSystem::GetStaticInterface(const JGType& type) const
{
	return nullptr;
}

bool GObjectGlobalSystem::CanCast(const JGType& destType, const JGType& srcType) const
{
	if (canCastInternal(destType, srcType) == true)
	{
		return true;
	}

	if (canCastInternal(srcType, destType) == true)
	{
		return true;
	}

	return false;
}

bool GObjectGlobalSystem::RegisterJGClass(PSharedPtr<JGClass> classObject)
{
	if (classObject.IsValid() == false)
	{
		return false;
	}

	PSharedPtr<JGType> classType = classObject->GetClassType();
	if (classType.IsValid() == false)
	{
		JG_LOG(Core, ELogLevel::Critical, "{0} : Invalid Type", classObject->GetName());
		return false;
	}
	
	if (registerType(classType) == false)
	{
		return false;
	}

	_classMap.emplace(*classType, classObject);

	return true;
}


bool GObjectGlobalSystem::RegisterJGEnum(PSharedPtr<JGEnum> enumObject)
{
	if (enumObject.IsValid() == false)
	{
		return false;
	}

	PSharedPtr<JGType> enumType = enumObject->GetEnumType();
	if (enumType.IsValid() == false)
	{
		JG_LOG(Core, ELogLevel::Critical, "{0} : Invalid Type", enumObject->GetName());
		return false;
	}

	if (registerType(enumType) == false)
	{
		return false;
	}

	_enumMap.emplace(*enumType, enumObject);

	return true;
}

bool GObjectGlobalSystem::registerType(PSharedPtr<JGType> type)
{
	if (type.IsValid() == false)
	{
		return false;
	}

	const PName&   typeName   = type->GetName();

	if (_typeMap.find(typeName) != _typeMap.end())
	{
		JG_LOG(Core, ELogLevel::Critical, "Duplicate Type Name : {0}", typeName);
		return false;
	}

	_typeMap.emplace(typeName, *type);

	return true;
}

// Base,  Dervie
// Dervie Base
bool GObjectGlobalSystem::canCastInternal(const JGType& destType, const JGType& srcType) const
{
	PSharedPtr<JGClass> destClass = GetStaticClass(destType);
	if (destClass == nullptr)
	{
		destClass = GetStaticInterface(destType);
	}

	PSharedPtr<JGClass> srcClass  = GetStaticClass(srcType);
	if (srcClass == nullptr)
	{
		srcClass = GetStaticInterface(srcType);
	}

	if (destClass == nullptr || srcClass == nullptr)
	{
		return false;
	}

	if (destClass->VTypeSet.contains(srcType) == true)
	{
		return true;
	}

	for (const JGType& type : destClass->VTypeSet)
	{
		if (canCastInternal(type, srcType) == true)
		{
			return true;
		}
	}

	return false;
}

bool GObjectGlobalSystem::codeGen()
{
	HPlatformInstance ins = HPlatform::LoadDll("CodeGen.dll");

	auto func1 = HPlatform::LoadFuncInDll<void, GCoreSystem*>(ins, "Link_Module");
	func1(&GCoreSystem::GetInstance());

	auto func = HPlatform::LoadFuncInDll<bool, GObjectGlobalSystem*>(ins, "Engine_CodeGenerate");
	func(this);

	HPlatform::UnLoadDll(ins);
	return true;
}