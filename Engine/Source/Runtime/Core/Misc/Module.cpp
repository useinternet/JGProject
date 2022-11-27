#include "PCH/PCH.h"
#include "Module.h"
#include "Platform/Platform.h"

IModuleInterface* GModuleGlobalSystem::FindModule(const JGType& type)
{
	return nullptr;
}

bool ConnectModule(const PString& moduleName)
{
	
	PString dllName = PString::Format("%s.dll", moduleName);;
	PString getTypeFuncName = PString::Format("_Get_%s_Type_", moduleName);
	PString createModuleFuncName = PString::Format("_Create_%s_Module_Interface", moduleName);

	HJInstance dllIns = HPlatform::LoadDll(dllName);

	/*
	APIDefine void _Get_##ModuleName_Type_(JGType* outType) \
{ \
	*outType = JGTYPE(##ModuleName); \
}\
APIDefine IModuleInterface* _Create_##ModuleName_Module_Interface()\
{\
	return new ModuleName();\
}\
	*/
	// Type
	//HPlatformFunction<void, JGType> getTypeFunc = HPlatform::LoadFuncInDll<void, JGType>(dllIns, getTypeFuncName);

	// »ý¼º
	//HPlatformFunction<IModuleInterface*> getTypeFunc = HPlatform::LoadFuncInDll<IModuleInterface*>(dllIns, getTypeFuncName);

	return true;
}

bool DisconnectModule(const PString& moduleName)
{
	return true;
}

bool ReconnectModule(const PString& moduleName)
{
	return true;
}