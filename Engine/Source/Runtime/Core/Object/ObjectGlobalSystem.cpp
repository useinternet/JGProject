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

PSharedPtr<JGEnum> GObjectGlobalSystem::GetStaticEnum(const JGType& type) const
{
	return nullptr;
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

bool GObjectGlobalSystem::RegisterJGInterface(PSharedPtr<JGInterface> ifObject)
{
	return false;
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

bool GObjectGlobalSystem::BindCreateObjectFunc(const JGType& type, const HCreateObjectFunc& func)
{
	const PName& typeName = type.GetName();

	if (_createObjectFuncPool.contains(type) == true)
	{
		JG_LOG(Core, ELogLevel::Critical, "BindCreateObjectFunc: Duplicate Type Name : {0}", typeName);
		return false;
	}

	_createObjectFuncPool.emplace(type, func);
	return true;
}

bool GObjectGlobalSystem::BindSaveObjectFunc(const JGType& type, const HSaveObjectFunc& func)
{
	const PName& typeName = type.GetName();

	if (_saveObjectFuncPool.contains(type) == true)
	{
		JG_LOG(Core, ELogLevel::Critical, "BindSaveObjectFunc: Duplicate Type Name : {0}", typeName);
		return false;
	}

	_saveObjectFuncPool.emplace(type, func);
	return true;
}

bool GObjectGlobalSystem::BindLoadObjectFunc(const JGType& type, const HLoadObjectFunc& func)
{
	const PName& typeName = type.GetName();

	if (_loadObjectFuncPool.contains(type) == true)
	{
		JG_LOG(Core, ELogLevel::Critical, "BindLoadObjectFunc: Duplicate Type Name : {0}", typeName);
		return false;
	}

	_loadObjectFuncPool.emplace(type, func);
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
	if (ins == nullptr)
	{
		// Error Log
		return false;
	}

	HPlatformFunction<void, GCoreSystem*> linkModuleFunc = HPlatform::LoadFuncInDll<void, GCoreSystem*>(ins, "Link_Module");
	if (linkModuleFunc.IsVaild() == false)
	{
		// Error Log
		return false;
	}

	linkModuleFunc(&GCoreSystem::GetInstance());

	HPlatformFunction<bool, GObjectGlobalSystem*> codeGenFunc = HPlatform::LoadFuncInDll<bool, GObjectGlobalSystem*>(ins, "Engine_CodeGenerate");
	if (codeGenFunc.IsVaild() == false)
	{
		// Error Log
		codeGenFunc(this);
	}

	HPlatform::UnLoadDll(ins);
	return true;
}