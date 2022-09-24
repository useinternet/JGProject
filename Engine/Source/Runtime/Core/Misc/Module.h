#pragma once
#include "CoreDefines.h"
#include "CoreSystem.h"
#include "Object/ObjectGlobals.h"

#define JG_MODULE_IMPL(ModuleName)

class IModuleInterface : public IMemoryObject
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

	template<class T>
	bool ConnectModule()
	{
		return ConnectModule(JGTYPE(T));
	}

	template<class T>
	bool DisconnectModule()
	{
		return DisconnectModule(JGTYPE(T));
	}

	template<class T>
	bool ReconnectModule()
	{
		return ReconnectModule(JGTYPE(T));
	}

	IModuleInterface* FindModule(const JGType& type);
	bool ConnectModule(const JGType& type);
	bool DisconnectModule(const JGType& type);
	bool ReconnectModule(const JGType& type);
};