#include "pch.h"
#include "Plugin.h"


namespace JG
{



	Plugin::Plugin(const String& dllPath)
	{
		mDll = ::LoadLibraryA(dllPath.c_str());
	}

	Plugin::~Plugin()
	{
		if (mDll != nullptr)
		{
			::FreeLibrary(mDll);
		}
	}

	bool Plugin::IsVaild() const
	{
		return mDll != nullptr;;
	}

	PluginLinker::PluginLinker()
	{
		pGlobalSingletoneManager = GlobalSingletonManager::GetInstancePtr();
	}

	void PluginLinker::Apply()
	{
		GlobalSingletonManager::SetInstancePtr(pGlobalSingletoneManager);
	}

}