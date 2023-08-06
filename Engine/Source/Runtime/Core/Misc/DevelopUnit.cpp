#include "PCH/PCH.h"
#include "DevelopUnit.h"
#include "Platform/Platform.h"

bool TryGetDevelopUnitDllName(PSharedPtr<JGClass> inClass, PString& outDllName)
{
	if (inClass == nullptr)
	{
		return false;
	}

	PString className = inClass->GetClassType()->GetName().ToString();
	PString dllName   = className + "_Dynamic.dll";
	if (dllName.StartWidth("JG"))
	{
		dllName.Remove(0, 2);
	}

	outDllName = dllName;

	return true;
}

JGDevelopUnit* JGDevelopUnit::LoadDevelopUnit(PSharedPtr<JGClass> inClass, const HGuid& inGuid)
{
	PString dllName;
	if (TryGetDevelopUnitDllName(inClass, dllName) == false)
	{
		return nullptr;
	}

	HGuid   guid = inGuid;
	PString dllOnlyName;
	HFileHelper::FileNameOnly(dllName, &dllOnlyName);
	PString newDllName = dllOnlyName + "_" + guid.ToString() + ".dll";
	if(false == HFileHelper::Exists(newDllName) || guid.IsValid() == false)
	{
		guid = HGuid::New();
		newDllName = dllOnlyName + "_" + guid.ToString() + ".dll";
	}

	if (false == HFileHelper::CopyFileOrDirectory(dllName, newDllName))
	{
		JG_LOG(Core, ELogLevel::Error, "%s : Load Fail DevelopUnit", newDllName);
		return nullptr;
	}

	dllName = newDllName;
	HJInstance dllInstance = HPlatform::LoadDll(dllName);
	if (dllInstance == nullptr)
	{
		JG_LOG(Core, ELogLevel::Error, "Fail Load DevelopUnit(%s)", dllName);
		return nullptr;
	}

	HPlatformFunction<void, GCoreSystem*> linkModuleFunc = HPlatform::LoadFuncInDll<void, GCoreSystem*>(dllInstance, JG_LINK_DEVELOPUNIT_FUNCTION_NAME);
	if (linkModuleFunc.IsVaild() == false)
	{
		JG_LOG(Core, ELogLevel::Error, "Fail Load DevelopUnit(%s)", dllName);
		return nullptr;
	}

	linkModuleFunc(&GCoreSystem::GetInstance());

	HPlatformFunction<JGDevelopUnit*> createDevelopUnit = HPlatform::LoadFuncInDll<JGDevelopUnit*>(dllInstance, JG_CREATE_DEVELOPUNIT_FUNCTION_NAME);
	if (createDevelopUnit.IsVaild() == false)
	{
		JG_LOG(Core, ELogLevel::Error, "Fail Load DevelopUnit(%s)", dllName);
		return nullptr;
	}

	JGDevelopUnit* developUnit = createDevelopUnit();
	if (nullptr == developUnit)
	{
		return nullptr;
	}

	developUnit->_dllInstance = dllInstance;
	developUnit->_dllName = dllName;
	developUnit->_guid = guid;

	JG_LOG(Core, ELogLevel::Info, "Load DevelopUnit : %s", dllName);
    return developUnit;
}

bool JGDevelopUnit::ReloadDevelopUnit(JGDevelopUnit*& outDevelopUnit)
{
	if (outDevelopUnit == nullptr)
	{
		return false;
	}

	HGuid guid = outDevelopUnit->_guid;
	UnloadDevelopUnit(outDevelopUnit);
	JGDevelopUnit* developUnit = LoadDevelopUnit(outDevelopUnit->GetClass(), guid);
	if (developUnit == nullptr)
	{
		return false;
	}

	outDevelopUnit = developUnit;

	return true;
}

void JGDevelopUnit::UnloadDevelopUnit(JGDevelopUnit*& outDevelopUnit, bool bRevemoDll)
{
	if (outDevelopUnit == nullptr)
	{
		return;
	}

	if (bRevemoDll)
	{
		PString dllName = outDevelopUnit->_dllName.ToString();
		HJInstance dllInstance = outDevelopUnit->_dllInstance;
		HPlatform::Deallocate(outDevelopUnit);
		HPlatform::UnLoadDll(dllInstance);
		
		if (true == HFileHelper::Exists(dllName))
		{
			JG_LOG(Core, ELogLevel::Info, "Unload DevelopUnit : %s", dllName);
			HFileHelper::RemoveFileOrDirectory(dllName);
		}
	}
	
	
}
