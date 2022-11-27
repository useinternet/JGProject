#pragma once
#include "CoreDefines.h"
#include "CoreSystem.h"
#include "Object/ObjectGlobals.h"
#pragma warning(disable : 4251)
#define JG_MODULE_IMPL(ModuleName, APIDefine) \
APIDefine IModuleInterface* _Create_Module_Interface_()\
{\
	return HPlatform::Allocate<##ModuleName>();\
}\
APIDefine void Link_Module(GCoreSystem* ins)\
{ \
HCoreSystemPrivate::SetInstance(ins); \
} \

class IModuleInterface
{
	friend class GModuleGlobalSystem;
protected:
// 시작/끝 함수
	virtual JGType GetModuleType() const = 0;
	virtual void StartupModule()  = 0;
	virtual void ShutdownModule() = 0;
};

class GModuleGlobalSystem : public GGlobalSystemInstance<GModuleGlobalSystem>
{
	HHashMap<PName, IModuleInterface*>  _modulesByName;
	HHashMap<JGType, IModuleInterface*> _modulesByType;

	HMutex _mutex;
public:
	virtual ~GModuleGlobalSystem() = default;

public:
	template<class T>
	T* FindModule() const
	{
		return static_cast<T*>(FindModule(JGTYPE(T)));
	}

	IModuleInterface* FindModule(const JGType& type);
	IModuleInterface* FindModule(const PName& moduleName);

	bool ConnectModule(const PString& moduleName);
	bool DisconnectModule(const PString& moduleName);
	bool ReconnectModule(const PString& moduleName);

protected:
	virtual void Destroy() override;
};