#include "Platform.h"

HPlatformInstance HPlatform::LoadDll(const PString& dllPath)
{
	return ::LoadLibraryA(dllPath.GetCStr());
}


void HPlatform::UnLoadDll(HPlatformInstance instanceID)
{
	if (instanceID == nullptr)
	{
		::FreeLibrary(instanceID);
	}
}