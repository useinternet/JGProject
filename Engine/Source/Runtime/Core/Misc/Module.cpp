#include "PCH/PCH.h"
#include "Module.h"
#include "Platform/Platform.h"

IModuleInterface* GModuleGlobalSystem::FindModule(const JGType& type) const
{
	HLockGuard<HMutex> lock(_mutex);

	if (_modulesByType.find(type) == _modulesByType.end())
	{
		return nullptr;
	}
	return _modulesByType.at(type);
}

IModuleInterface* GModuleGlobalSystem::FindModule(const PName& moduleName) const
{
	HLockGuard<HMutex> lock(_mutex);

	if (_modulesByName.find(moduleName) == _modulesByName.end())
	{
		return nullptr;
	}

	return _modulesByName.at(moduleName);
}

bool GModuleGlobalSystem::ConnectModule(const PString& moduleName)
{
	if (FindModule(moduleName) != nullptr)
	{
		JG_LOG(Core, ELogLevel::Error, "Fail Connect Module:%s", moduleName);
		return false;
	}

	PString dllName = PString::Format("%s.dll", moduleName);;
	PString getTypeFuncName = PString::Format("_Get_%s_Type_", moduleName);
	PString createModuleFuncName = "_Create_Module_Interface_";

	HJInstance dllIns = HPlatform::LoadDll(dllName);
	if (dllIns == 0)
	{
		JG_LOG(Core, ELogLevel::Error, "Fail Connect Module:%s", moduleName);
		return false;
	}

	HPlatformFunction<void, GCoreSystem*> linkModuleFunc = HPlatform::LoadFuncInDll<void, GCoreSystem*>(dllIns, "Link_Module");
	if (linkModuleFunc.IsVaild() == false)
	{
		// Error Log
		HPlatform::UnLoadDll(dllIns);
		return false;
	}

	linkModuleFunc(&GCoreSystem::GetInstance());

	HPlatformFunction<IModuleInterface*> createModuleFunc = HPlatform::LoadFuncInDll<IModuleInterface*>(dllIns, createModuleFuncName);
	if (createModuleFunc.IsVaild() == false)
	{
		JG_LOG(Core, ELogLevel::Error, "Fail Connect Module:%s", moduleName);

		HPlatform::UnLoadDll(dllIns);
		return false;
	}

	IModuleInterface* moduleIf = createModuleFunc();
	if (moduleIf == nullptr)
	{
		JG_LOG(Core, ELogLevel::Error, "Fail Connect Module:%s", moduleName);
		HPlatform::UnLoadDll(dllIns);
		return false;
	}

	if (FindModule(moduleIf->GetModuleType()) != nullptr)
	{
		JG_LOG(Core, ELogLevel::Error, "Fail Connect Module:%s", moduleName);

		HPlatform::UnLoadDll(dllIns);
		HPlatform::Deallocate(moduleIf);
		return false;
	}

	{
		HLockGuard<HMutex> lock(_mutex);
		_modulesByType.emplace(moduleIf->GetModuleType(), moduleIf);
		_modulesByName.emplace(PName(moduleName), moduleIf);
	}

	moduleIf->StartupModule();

	HPlatform::UnLoadDll(dllIns);
	return true;
}

bool GModuleGlobalSystem::DisconnectModule(const PString& moduleName)
{
	HLockGuard<HMutex> lock(_mutex);

	if (_modulesByName.find(moduleName) == _modulesByName.end())
	{
		return true;
	}

	IModuleInterface* moduleIf = _modulesByName[moduleName];
	if (moduleIf == nullptr)
	{
		return false;
	}

	moduleIf->ShutdownModule();

	_modulesByType.erase(moduleIf->GetModuleType());
	_modulesByName.erase(moduleName);

	HPlatform::Deallocate(moduleIf);

	return true;
}

bool GModuleGlobalSystem::ReconnectModule(const PString& moduleName)
{
	JG_ASSERT("not impl reconnectModule");
	return true;
}

void GModuleGlobalSystem::Destroy()
{
	for (HPair<const JGType, IModuleInterface*>& pair : _modulesByType)
	{
		pair.second->ShutdownModule();
	}

	for (HPair<const JGType, IModuleInterface*>& pair : _modulesByType)
	{
		HPlatform::Deallocate(pair.second);
	}

	_modulesByType.clear();
	_modulesByName.clear();
}