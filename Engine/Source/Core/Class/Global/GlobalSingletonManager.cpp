#include "pch.h"
#include "GlobalSingletonManager.h"

namespace JG
{
	GlobalSingletonManager* GlobalSingletonManager::smGlobalSingletonManager = nullptr;

	void GlobalSingletonManager::Create()
	{
		if (smGlobalSingletonManager == nullptr)
		{
			smGlobalSingletonManager = new GlobalSingletonManager();
		}
	}
	void GlobalSingletonManager::Destroy()
	{
		if (smGlobalSingletonManager != nullptr)
		{
			delete smGlobalSingletonManager;
			smGlobalSingletonManager = nullptr;
		}
	}
	GlobalSingletonManager& GlobalSingletonManager::GetInstance()
	{
		return *smGlobalSingletonManager;
	}

	GlobalSingletonManager* GlobalSingletonManager::GetInstancePtr()
	{
		return smGlobalSingletonManager;
	}

	void GlobalSingletonManager::SetInstancePtr(GlobalSingletonManager* ptr)
	{
		if (smGlobalSingletonManager == nullptr)
		{
			smGlobalSingletonManager = ptr;
		}
	
	}



}