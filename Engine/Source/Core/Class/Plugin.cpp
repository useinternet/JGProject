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

}