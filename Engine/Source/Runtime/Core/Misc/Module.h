#pragma once
#include "CoreDefines.h"
#include "CoreSystem.h"
#include "Object/ObjectGlobals.h"

#define JG_MODULE_IMPL(ModuleName, APIDefine) \
APIDefine void _Get_##ModuleName_Type_(JGType* outType) \
{ \
	*outType = JGTYPE(##ModuleName); \
}\
APIDefine IModuleInterface* _Create_##ModuleName_Module_Interface()\
{\
	return new ModuleName();\
}\

class IModuleInterface
{
protected:
// 시작/끝 함수
	virtual void StartupModule()  = 0;
	virtual void ShutdownModule() = 0;
};

class GModuleGlobalSystem : public GGlobalSystemInstance<GModuleGlobalSystem>
{
	HHashMap<JGType, IModuleInterface*> _modules;

public:
	virtual ~GModuleGlobalSystem() = default;

public:
	template<class T>
	T* FindModule() const
	{
		return static_cast<T*>(FindModule(JGTYPE(T)));
	}

	IModuleInterface* FindModule(const JGType& type);

	bool ConnectModule(const PString& moduleName);
	bool DisconnectModule(const PString& moduleName);
	bool ReconnectModule(const PString& moduleName);
};